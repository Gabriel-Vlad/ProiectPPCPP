#include "Inscriere.h"
#include <algorithm>
#include <sstream>

// Parameters are moved into the members to avoid copying the strings.
Inscriere::Inscriere(std::string id_user, std::string id_curs)
    : id_user(std::move(id_user)), id_curs(std::move(id_curs)) {}

Inscriere::Inscriere(std::string id_user, std::string id_curs, std::vector<std::string> lectii_finalizate)
    : id_user(std::move(id_user)), id_curs(std::move(id_curs)),
      lectii_finalizate(std::move(lectii_finalizate)) {}

// Getters
const std::string&              Inscriere::getIdUser()           const { return id_user; }
const std::string&              Inscriere::getIdCurs()           const { return id_curs; }
const std::vector<std::string>& Inscriere::getLectiiFinalizate() const { return lectii_finalizate; }

// Lesson completion
//
// The guard prevents the same lesson id from appearing twice in the list.
void Inscriere::finalizareLectie(const std::string& id_lectie) {
    if (!esteLectieFinalizata(id_lectie)) {
        lectii_finalizate.push_back(id_lectie);
    }
}

// std::erase removes every occurrence of the value in one pass.
void Inscriere::nefinalizareLectie(const std::string& id_lectie) {
    std::erase(lectii_finalizate, id_lectie);
}

bool Inscriere::esteLectieFinalizata(const std::string& id_lectie) const {
    return std::ranges::contains(lectii_finalizate, id_lectie);
}

// Serialization
//
// On-disk format (matches the project specification):
//   <id_user> <id_curs> <numar_lectii_finalizate>
//   <id_lectie_1> <id_lectie_2> ...        <- this line only exists if count > 0
void Inscriere::serialize(std::ostream& out) const {
    out << id_user << " " << id_curs << " " << lectii_finalizate.size() << "\n";
    for (const auto& id : lectii_finalizate) out << id << " ";
    if (!lectii_finalizate.empty()) out << "\n";
}

// Reads back the format written by serialize. The count on the first line
// tells us whether a second line with lesson ids follows at all.
Inscriere Inscriere::deserialize(std::istream& in) {
    std::string line;
    std::getline(in, line);
    std::istringstream ss(line);

    std::string id_user, id_curs;
    int n = 0;
    ss >> id_user >> id_curs >> n;

    Inscriere ins(id_user, id_curs);
    if (n > 0) {
        std::string lline;
        std::getline(in, lline);
        std::istringstream ls(lline);
        std::string id_lectie;
        while (ls >> id_lectie)
            ins.finalizareLectie(id_lectie);
    }
    return ins;
}

// Display
std::ostream& operator<<(std::ostream& out, const Inscriere& i) {
    out << i.id_user << " inscris la [" << i.id_curs << "] — "
       << i.lectii_finalizate.size() << " lectii finalizate";
    return out;
}
