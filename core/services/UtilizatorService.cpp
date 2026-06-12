#include "UtilizatorService.h"
#include "../infra/colors.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

// std::make_unique builds each repository on the heap; the unique_ptr members
// delete them automatically when the service goes out of scope.
UtilizatorService::UtilizatorService()
    : repo(std::make_unique<Repository<Utilizator>>(UTILIZATORI_FILE)),
      inscriereRepo(std::make_unique<Repository<Inscriere>>(INSCRIERI_FILE)) {}

std::vector<Utilizator> UtilizatorService::getUtilizatori() const {
    return repo->loadAll();
}

void UtilizatorService::vizualizareUtilizatori() const {
    auto utilizatori = repo->loadAll();
    if (utilizatori.empty()) {
        std::cout << "Nu exista utilizatori inregistrati.\n";
        return;
    }
    for (const auto& u : utilizatori)
        std::cout << u << "\n";
}

void UtilizatorService::inregistrareUtilizator(const std::string& nume) {
    if (nume.empty()) throw std::invalid_argument("Numele utilizatorului nu poate fi gol.");
    validateNoNewline(nume, "Numele utilizatorului");

    auto utilizatori = repo->loadAll();
    std::string id   = nextId(utilizatori);

    utilizatori.emplace_back(id, nume);
    repo->saveAll(utilizatori);
    std::cout << Color::GREEN << "Utilizatorul \"" << nume << "\" a fost inregistrat (ID: " << id << ")." << Color::RESET << "\n";
}

// Deleting a user is the most involved operation in the project, because user
// IDs must stay dense (1, 2, 3...) and inscrieri.txt references those IDs.
// It happens in three steps:
//   1. erase the user,
//   2. renumber everyone after them (recording old -> new in "remap"),
//   3. fix inscrieri.txt: drop the deleted user's enrollments, then rewrite
//      the user IDs of everyone whose ID just changed.
void UtilizatorService::stergereUtilizator(const std::string& id) {
    auto utilizatori = repo->loadAll();

    auto it = findById(utilizatori, id);
    if (it == utilizatori.end())
        throw std::invalid_argument("Nu exista un utilizator cu id-ul: " + id);

    std::string nume    = it->getNume();
    int         deleted = std::stoi(id);
    utilizatori.erase(it);

    // Step 2 — renumber. After erase, position i in the vector should hold
    // ID i+1. Users before the deleted one already match and are skipped;
    // every change is recorded so enrollments can be fixed in step 3.
    std::vector<std::pair<std::string,std::string>> remap;
    for (int i = 0; i < static_cast<int>(utilizatori.size()); i++) {
        std::string oldId = utilizatori[i].getId();
        std::string newId = std::to_string(i + 1);
        if (oldId != newId) {
            remap.emplace_back(oldId, newId);
            utilizatori[i] = Utilizator(newId, utilizatori[i].getNume());
        }
    }
    repo->saveAll(utilizatori);

    // Step 3 — first drop the deleted user's enrollments, then apply the
    // remap. Each enrollment matches at most one remap entry, so we can
    // stop searching after the first hit (break).
    auto inscrieri = inscriereRepo->loadAll();
    std::erase_if(inscrieri, [&](const Inscriere& i) {
        return i.getIdUser() == id;
    });
    for (auto& ins : inscrieri) {
        for (const auto& [oldId, newId] : remap) {
            if (ins.getIdUser() == oldId) {
                ins = Inscriere(newId, ins.getIdCurs(), ins.getLectiiFinalizate());
                break;
            }
        }
    }
    inscriereRepo->saveAll(inscrieri);

    std::cout << Color::GREEN << "Utilizatorul \"" << nume << "\" (ID: " << deleted
              << ") a fost sters. ID-urile au fost reechilibrate." << Color::RESET << "\n";
}
