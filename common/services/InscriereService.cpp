#include "InscriereService.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

// Binds both repositories to the data files defined in constants.h.
InscriereService::InscriereService()
    : cursRepo(std::make_unique<Repository<Curs>>(CURSURI_FILE)),
      inscriereRepo(std::make_unique<Repository<Inscriere>>(INSCRIERI_FILE)) {}

// ── Private helpers ───────────────────────────────────────────────────────────

std::vector<Curs> InscriereService::loadCursuri() const {
    return cursRepo->loadAll();
}

std::vector<Inscriere> InscriereService::loadInscrieri() const {
    return inscriereRepo->loadAll();
}

void InscriereService::saveInscrieri(const std::vector<Inscriere>& inscrieri) const {
    inscriereRepo->saveAll(inscrieri);
}

// Linear search by id — dataset is small so no index needed.
auto InscriereService::findCurs(const std::vector<Curs>& cursuri, const std::string& id) {
    return std::ranges::find_if(cursuri, [&](const Curs& c) {
        return c.getId() == id;
    });
}

// Matches both user id and course id to uniquely identify an enrollment.
auto InscriereService::findInscriere(std::vector<Inscriere>& inscrieri,
                                      const std::string& id_user, const std::string& id_curs) {
    return std::ranges::find_if(inscrieri, [&](const Inscriere& i) {
        return i.getIdUser() == id_user && i.getIdCurs() == id_curs;
    });
}

// ── Enrollment operations ─────────────────────────────────────────────────────

void InscriereService::vizualizareCursuriInscrise(const std::string& id_user) const {
    auto inscrieri = loadInscrieri();
    auto cursuri   = loadCursuri();

    bool gasit = false;
    for (const auto& ins : inscrieri) {
        if (ins.getIdUser() != id_user) continue;
        gasit = true;

        auto it = findCurs(cursuri, ins.getIdCurs());
        if (it != cursuri.end()) {
            // operator<< on Curs prints full course info including lessons.
            std::cout << *it << "\n";
        } else {
            // Course was deleted from cursuri.txt but enrollment still exists.
            std::cout << "[" << ins.getIdCurs() << "] (curs sters de pe platforma)\n\n";
        }
    }

    if (!gasit)
        std::cout << "Utilizatorul " << id_user << " nu este inscris la niciun curs.\n";
}

void InscriereService::inscriere(const std::string& id_user, const std::string& id_curs) {
    auto cursuri   = loadCursuri();
    auto inscrieri = loadInscrieri();

    if (findCurs(cursuri, id_curs) == cursuri.end())
        throw std::invalid_argument("Nu exista un curs cu id-ul: " + id_curs);

    if (findInscriere(inscrieri, id_user, id_curs) != inscrieri.end())
        throw std::invalid_argument("Utilizatorul " + id_user + " este deja inscris la cursul: " + id_curs);

    inscrieri.emplace_back(id_user, id_curs);
    saveInscrieri(inscrieri);
    std::cout << "Utilizatorul " << id_user << " a fost inscris la cursul [" << id_curs << "].\n";
}

void InscriereService::dezinscriere(const std::string& id_user, const std::string& id_curs) {
    auto inscrieri = loadInscrieri();
    auto it = findInscriere(inscrieri, id_user, id_curs);

    if (it == inscrieri.end())
        throw std::invalid_argument("Utilizatorul " + id_user + " nu este inscris la cursul: " + id_curs);

    inscrieri.erase(it);
    saveInscrieri(inscrieri);
    std::cout << "Utilizatorul " << id_user << " a fost dezinscris din cursul [" << id_curs << "].\n";
}

// ── Lesson completion ─────────────────────────────────────────────────────────

void InscriereService::finalizareLectie(const std::string& id_user, const std::string& id_curs,
                                         const std::string& id_lectie) {
    auto cursuri   = loadCursuri();
    auto inscrieri = loadInscrieri();

    auto cursIt = findCurs(cursuri, id_curs);
    if (cursIt == cursuri.end())
        throw std::invalid_argument("Nu exista un curs cu id-ul: " + id_curs);

    // Validate that the lesson belongs to this course before marking it.
    if (!cursIt->findLectie(id_lectie).has_value())
        throw std::invalid_argument("Nu exista o lectie cu id-ul: " + id_lectie + " in cursul: " + id_curs);

    auto insIt = findInscriere(inscrieri, id_user, id_curs);
    if (insIt == inscrieri.end())
        throw std::invalid_argument("Utilizatorul " + id_user + " nu este inscris la cursul: " + id_curs);

    if (insIt->esteLectieFinalizata(id_lectie))
        throw std::invalid_argument("Lectia [" + id_lectie + "] este deja marcata ca finalizata.");

    insIt->finalizareLectie(id_lectie);
    saveInscrieri(inscrieri);
    std::cout << "Lectia [" << id_lectie << "] a fost marcata ca finalizata.\n";
}

void InscriereService::nefinalizareLectie(const std::string& id_user, const std::string& id_curs,
                                           const std::string& id_lectie) {
    auto cursuri   = loadCursuri();
    auto inscrieri = loadInscrieri();

    auto cursIt = findCurs(cursuri, id_curs);
    if (cursIt == cursuri.end())
        throw std::invalid_argument("Nu exista un curs cu id-ul: " + id_curs);

    if (!cursIt->findLectie(id_lectie).has_value())
        throw std::invalid_argument("Nu exista o lectie cu id-ul: " + id_lectie + " in cursul: " + id_curs);

    auto insIt = findInscriere(inscrieri, id_user, id_curs);
    if (insIt == inscrieri.end())
        throw std::invalid_argument("Utilizatorul " + id_user + " nu este inscris la cursul: " + id_curs);

    if (!insIt->esteLectieFinalizata(id_lectie))
        throw std::invalid_argument("Lectia [" + id_lectie + "] nu este marcata ca finalizata.");

    insIt->nefinalizareLectie(id_lectie);
    saveInscrieri(inscrieri);
    std::cout << "Lectia [" << id_lectie << "] a fost marcata ca nefinalizata.\n";
}

void InscriereService::vizualizareProgres(const std::string& id_user,
                                           const std::string& id_curs) const {
    auto cursuri   = loadCursuri();
    auto inscrieri = loadInscrieri();

    auto cursIt = findCurs(cursuri, id_curs);
    if (cursIt == cursuri.end())
        throw std::invalid_argument("Nu exista un curs cu id-ul: " + id_curs);

    // vizualizareProgres is const so we need a mutable copy to call findInscriere.
    auto inscrieri_copy = inscrieri;
    auto insIt = findInscriere(inscrieri_copy, id_user, id_curs);
    if (insIt == inscrieri_copy.end())
        throw std::invalid_argument("Utilizatorul " + id_user + " nu este inscris la cursul: " + id_curs);

    size_t total      = cursIt->getLectii().size();
    size_t finalizate = insIt->getLectiiFinalizate().size();

    std::cout << "Progres " << id_user << " la [" << id_curs << "] \""
              << cursIt->getTitlu() << "\": "
              << finalizate << " / " << total << " lectii finalizate";

    // Show percentage only if the course has at least one lesson.
    if (total > 0) {
        int procent = static_cast<int>((finalizate * 100) / total);
        std::cout << " (" << procent << "%)";
    }
    std::cout << "\n";
}
