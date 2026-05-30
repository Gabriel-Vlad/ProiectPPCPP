#include "Inscriere.h"
#include <algorithm>

// Moves strings into members to avoid unnecessary copies.
Inscriere::Inscriere(std::string id_user, std::string id_curs)
    : id_user(std::move(id_user)), id_curs(std::move(id_curs)) {}

// ── Getters ───────────────────────────────────────────────────────────────────

const std::string&              Inscriere::getIdUser()           const { return id_user; }
const std::string&              Inscriere::getIdCurs()           const { return id_curs; }
const std::vector<std::string>& Inscriere::getLectiiFinalizate() const { return lectii_finalizate; }

// ── Lesson completion ─────────────────────────────────────────────────────────

// Checks first to avoid duplicate entries in the completed lessons list.
void Inscriere::finalizareLectie(const std::string& id_lectie) {
    if (!esteLectieFinalizata(id_lectie))
        lectii_finalizate.push_back(id_lectie);
}

// Uses std::erase (C++20) to remove by value in a single pass.
void Inscriere::nefinalizareLectie(const std::string& id_lectie) {
    std::erase(lectii_finalizate, id_lectie);
}

bool Inscriere::esteLectieFinalizata(const std::string& id_lectie) const {
    return std::ranges::find(lectii_finalizate, id_lectie) != lectii_finalizate.end();
}

// ── Serialization ─────────────────────────────────────────────────────────────

// Format: id_user, id_curs, numar_lectii_finalizate, then each completed lesson id.
void Inscriere::serialize(std::ostream& os) const {
    os << id_user << "\n" << id_curs << "\n";
    os << lectii_finalizate.size() << "\n";
    for (const auto& id : lectii_finalizate)
        os << id << "\n";
}

// Reads fields line by line to correctly handle ids that may contain spaces.
Inscriere Inscriere::deserialize(std::istream& is) {
    std::string id_user, id_curs;
    std::getline(is, id_user);
    std::getline(is, id_curs);

    int n;
    is >> n;
    is.ignore();

    Inscriere ins(id_user, id_curs);
    for (int i = 0; i < n; i++) {
        std::string id_lectie;
        std::getline(is, id_lectie);
        ins.finalizareLectie(id_lectie);
    }
    return ins;
}

// ── Display ───────────────────────────────────────────────────────────────────

std::ostream& operator<<(std::ostream& os, const Inscriere& i) {
    os << i.id_user << " inscris la [" << i.id_curs << "] — "
       << i.lectii_finalizate.size() << " lectii finalizate";
    return os;
}
