#include "Curs.h"
#include <algorithm>
#include <iomanip>
#include <optional>

// Parameters are taken by value and moved into the members.
// std::move avoids copying each string a second time: the constructor's local
// copy hands its internal buffer directly to the member variable.
Curs::Curs(std::string id, std::string titlu, std::string descriere, double pret)
    : id_curs(std::move(id)), titlu(std::move(titlu)),
      descriere(std::move(descriere)), pret(pret) {}

// ── Getters ───────────────────────────────────────────────────────────────────

const std::string&         Curs::getId()        const { return id_curs; }
const std::string&         Curs::getTitlu()     const { return titlu; }
const std::string&         Curs::getDescriere() const { return descriere; }
double                     Curs::getPret()      const { return pret; }
const std::vector<Lectie>& Curs::getLectii()    const { return lectii; }

// ── Setters ───────────────────────────────────────────────────────────────────

void Curs::setTitlu(const std::string& t)     { titlu = t; }
void Curs::setDescriere(const std::string& d) { descriere = d; }
void Curs::setPret(double p)                  { pret = p; }

// ── Lesson management ─────────────────────────────────────────────────────────

void Curs::addLectie(const Lectie& lectie) {
    lectii.push_back(lectie);
}

// std::erase_if (C++20) removes every matching element in a single pass.
void Curs::removeLectie(const std::string& id_lectie) {
    std::erase_if(lectii, [&](const Lectie& l) {
        return l.getId() == id_lectie;
    });
}

// Returns a copy of the found lesson wrapped in optional, or nullopt if missing.
std::optional<Lectie> Curs::findLectie(const std::string& id_lectie) const {
    auto it = std::ranges::find_if(lectii, [&](const Lectie& l) {
        return l.getId() == id_lectie;
    });
    if (it == lectii.end()) return std::nullopt;
    return *it;
}

// ── Serialization ─────────────────────────────────────────────────────────────

// On-disk format — one field per line, so titles and descriptions may freely
// contain spaces:
//   <id>
//   <titlu>
//   <descriere>
//   <pret>
//   <numar_lectii>
//   then for each lesson: <id> / <titlu> / <durata>, each on its own line.
void Curs::serialize(std::ostream& os) const {
    os << id_curs << "\n" << titlu << "\n" << descriere << "\n" << pret << "\n";
    os << lectii.size() << "\n";
    for (const auto& l : lectii)
        os << l.getId() << "\n" << l.getTitlu() << "\n" << l.getDurata() << "\n";
}

// Reads back the format written by serialize.
// Mixing getline with ">>" needs care: ">>" reads a value but leaves the
// newline behind it in the stream, so the next getline would read an empty
// string. The is.ignore() calls consume exactly that leftover newline.
Curs Curs::deserialize(std::istream& is) {
    std::string id, titlu, descriere;
    double pret;
    std::getline(is, id);
    std::getline(is, titlu);
    std::getline(is, descriere);
    is >> pret;
    int n = 0;
    is >> n;
    is.ignore();

    Curs c(id, titlu, descriere, pret);
    for (int i = 0; i < n; i++) {
        std::string lid, ltitlu;
        int durata;
        std::getline(is, lid);
        std::getline(is, ltitlu);
        is >> durata;
        is.ignore();
        c.addLectie(Lectie(lid, ltitlu, durata));
    }
    return c;
}

// ── Display ───────────────────────────────────────────────────────────────────

// Prints the price with exactly 2 decimals and lists all lessons indented.
std::ostream& operator<<(std::ostream& os, const Curs& c) {
    os << "[" << c.id_curs << "] " << c.titlu << "\n"
       << "  Descriere : " << c.descriere << "\n"
       << "  Pret      : " << std::fixed << std::setprecision(2) << c.pret << " RON\n"
       << "  Lectii    : " << c.lectii.size() << "\n";
    for (const auto& l : c.lectii)
        os << "    - " << l << "\n";
    return os;
}
