#include "InscriereService.h"
#include "../infra/colors.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

// std::make_unique builds each repository on the heap; the unique_ptr members
// delete them automatically when the service goes out of scope.
InscriereService::InscriereService()
    : cursRepo(std::make_unique<Repository<Curs>>(CURSURI_FILE)),
      inscriereRepo(std::make_unique<Repository<Inscriere>>(INSCRIERI_FILE)),
      utilizatorRepo(std::make_unique<Repository<Utilizator>>(UTILIZATORI_FILE)) {}

// ── Private helpers ───────────────────────────────────────────────────────────

// Finds the enrollment matching a (user, course) pair — both must match,
// because a user can be enrolled in many courses and a course has many users.
// Returns end() when not found; the caller must check before dereferencing.
// Declared as a template so it accepts const and non-const vectors alike.
template<typename Vec>
static auto findInscriere(Vec& inscrieri,
                          const std::string& id_user, const std::string& id_curs) {
    return std::ranges::find_if(inscrieri, [&](const Inscriere& i) {
        return i.getIdUser() == id_user && i.getIdCurs() == id_curs;
    });
}

// Common guard for every operation that receives a user id from outside:
// the id must be non-empty and must belong to a registered user.
void InscriereService::validateUtilizator(const std::string& id_user) const {
    if (id_user.empty()) throw std::invalid_argument("ID-ul utilizatorului nu poate fi gol.");

    auto utilizatori = utilizatorRepo->loadAll();
    if (findById(utilizatori, id_user) == utilizatori.end())
        throw std::invalid_argument("Nu exista un utilizator cu id-ul: " + id_user);
}

// One implementation for both directions of lesson completion.
// Validation order matters: course first, then lesson, then enrollment —
// so the user always gets the most specific error possible.
void InscriereService::toggleLectie(const std::string& id_user, const std::string& id_curs,
                                    const std::string& id_lectie, bool finalizeaza) {
    auto cursuri   = cursRepo->loadAll();
    auto inscrieri = inscriereRepo->loadAll();

    auto cursIt = findById(cursuri, id_curs);
    if (cursIt == cursuri.end())
        throw std::invalid_argument("Nu exista un curs cu id-ul: " + id_curs);

    // The lesson must belong to this specific course — lesson ids are only
    // unique within a course, so checking the id alone would not be enough.
    if (!cursIt->findLectie(id_lectie).has_value())
        throw std::invalid_argument("Nu exista o lectie cu id-ul: " + id_lectie + " in cursul: " + id_curs);

    auto insIt = findInscriere(inscrieri, id_user, id_curs);
    if (insIt == inscrieri.end())
        throw std::invalid_argument("Utilizatorul " + id_user + " nu este inscris la cursul: " + id_curs);

    // Reject no-op toggles: marking an already-marked lesson (or unmarking an
    // unmarked one) is reported as an error instead of silently succeeding.
    if (insIt->esteLectieFinalizata(id_lectie) == finalizeaza)
        throw std::invalid_argument(finalizeaza
            ? "Lectia [" + id_lectie + "] este deja marcata ca finalizata."
            : "Lectia [" + id_lectie + "] nu este marcata ca finalizata.");

    if (finalizeaza) insIt->finalizareLectie(id_lectie);
    else             insIt->nefinalizareLectie(id_lectie);

    inscriereRepo->saveAll(inscrieri);
    std::cout << Color::GREEN << "Lectia [" << id_lectie << "] a fost marcata ca "
              << (finalizeaza ? "finalizata." : "nefinalizata.") << Color::RESET << "\n";
}

// ── Enrollment operations ─────────────────────────────────────────────────────

std::vector<Inscriere> InscriereService::getInscrieri() const { return inscriereRepo->loadAll(); }

void InscriereService::vizualizareCursuriInscrise(const std::string& id_user) const {
    auto inscrieri = inscriereRepo->loadAll();
    auto cursuri   = cursRepo->loadAll();

    bool gasit = false;
    for (const auto& ins : inscrieri) {
        if (ins.getIdUser() != id_user) continue;
        gasit = true;

        auto it = findById(cursuri, ins.getIdCurs());
        if (it != cursuri.end())
            std::cout << *it << "\n";
        else
            // The enrollment exists but its course was deleted — say so
            // instead of printing nothing.
            std::cout << "[" << ins.getIdCurs() << "] (curs sters de pe platforma)\n";
    }

    if (!gasit)
        std::cout << "Utilizatorul " << id_user << " nu este inscris la niciun curs.\n";
}

void InscriereService::inscriere(const std::string& id_user, const std::string& id_curs) {
    validateUtilizator(id_user);

    auto cursuri   = cursRepo->loadAll();
    auto inscrieri = inscriereRepo->loadAll();

    if (findById(cursuri, id_curs) == cursuri.end())
        throw std::invalid_argument("Nu exista un curs cu id-ul: " + id_curs);

    if (findInscriere(inscrieri, id_user, id_curs) != inscrieri.end())
        throw std::invalid_argument("Utilizatorul " + id_user + " este deja inscris la cursul: " + id_curs);

    inscrieri.emplace_back(id_user, id_curs);
    inscriereRepo->saveAll(inscrieri);
    std::cout << Color::GREEN << "Utilizatorul " << id_user << " a fost inscris la cursul [" << id_curs << "]." << Color::RESET << "\n";
}

void InscriereService::dezinscriere(const std::string& id_user, const std::string& id_curs) {
    validateUtilizator(id_user);

    auto inscrieri = inscriereRepo->loadAll();
    auto it = findInscriere(inscrieri, id_user, id_curs);

    if (it == inscrieri.end())
        throw std::invalid_argument("Utilizatorul " + id_user + " nu este inscris la cursul: " + id_curs);

    inscrieri.erase(it);
    inscriereRepo->saveAll(inscrieri);
    std::cout << Color::GREEN << "Utilizatorul " << id_user << " a fost dezinscris din cursul [" << id_curs << "]." << Color::RESET << "\n";
}

// ── Lesson completion ─────────────────────────────────────────────────────────

// Both public methods delegate to toggleLectie; only the direction differs.

void InscriereService::finalizareLectie(const std::string& id_user, const std::string& id_curs,
                                         const std::string& id_lectie) {
    toggleLectie(id_user, id_curs, id_lectie, true);
}

void InscriereService::nefinalizareLectie(const std::string& id_user, const std::string& id_curs,
                                           const std::string& id_lectie) {
    toggleLectie(id_user, id_curs, id_lectie, false);
}

void InscriereService::vizualizareProgres(const std::string& id_user,
                                           const std::string& id_curs) const {
    auto cursuri   = cursRepo->loadAll();
    auto inscrieri = inscriereRepo->loadAll();

    auto cursIt = findById(cursuri, id_curs);
    if (cursIt == cursuri.end())
        throw std::invalid_argument("Nu exista un curs cu id-ul: " + id_curs);

    auto insIt = findInscriere(inscrieri, id_user, id_curs);
    if (insIt == inscrieri.end())
        throw std::invalid_argument("Utilizatorul " + id_user + " nu este inscris la cursul: " + id_curs);

    size_t total      = cursIt->getLectii().size();
    size_t finalizate = insIt->getLectiiFinalizate().size();

    std::cout << "Progres " << Color::CYAN << id_user << Color::RESET
              << " la [" << Color::CYAN << id_curs << Color::RESET << "] \""
              << cursIt->getTitlu() << "\": "
              << Color::CYAN << finalizate << " / " << total << Color::RESET
              << " lectii finalizate";

    // The percentage is shown only when the course has lessons —
    // dividing by zero would be undefined.
    if (total > 0) {
        int procent = static_cast<int>((finalizate * 100) / total);
        std::cout << " " << Color::YELLOW << "(" << procent << "%)" << Color::RESET;
    }
    std::cout << "\n";
}
