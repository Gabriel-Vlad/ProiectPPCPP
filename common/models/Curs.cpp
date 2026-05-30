#include "Curs.h"
#include <algorithm>
#include <iomanip>
#include <optional>

// Moves strings into members to avoid unnecessary copies.
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

// Uses std::erase_if (C++20) to remove by id in a single pass.
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

// Format: id, titlu, descriere, pret, numar_lectii, then each lesson on its own lines.
void Curs::serialize(std::ostream& os) const {
    os << id_curs << "\n" << titlu << "\n" << descriere << "\n" << pret << "\n";
    os << lectii.size() << "\n";
    for (const auto& l : lectii)
        os << l.getId() << "\n" << l.getTitlu() << "\n" << l.getDurata() << "\n";
}

// Reads fields line by line to correctly handle titles/descriptions with spaces.
Curs Curs::deserialize(std::istream& is) {
    std::string id, titlu, descriere;
    double pret;
    std::getline(is, id);
    std::getline(is, titlu);
    std::getline(is, descriere);
    is >> pret;
    int n;
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

// Prints price with 2 decimal places and lists all lessons indented.
std::ostream& operator<<(std::ostream& os, const Curs& c) {
    os << "[" << c.id_curs << "] " << c.titlu << "\n"
       << "  Descriere : " << c.descriere << "\n"
       << "  Pret      : " << std::fixed << std::setprecision(2) << c.pret << " RON\n"
       << "  Lectii    : " << c.lectii.size() << "\n";
    for (const auto& l : c.lectii)
        os << "    - " << l << "\n";
    return os;
}
