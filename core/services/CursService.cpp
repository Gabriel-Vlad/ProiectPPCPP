#include "CursService.h"
#include "../infra/colors.h"
#include <iostream>
#include <stdexcept>

// std::make_unique creates the object on the heap and wraps it in a
// unique_ptr in one step. The repositories live as long as the service does.
CursService::CursService()
    : repo(std::make_unique<Repository<Curs>>(CURSURI_FILE)),
      inscriereRepo(std::make_unique<Repository<Inscriere>>(INSCRIERI_FILE)) {}

// ── Private helpers ───────────────────────────────────────────────────────────

// Converts a price string to a number and validates it.
// Shared by adaugareCurs and modificareCurs so the rules stay identical.
static double parsePret(const std::string& s) {
    double pret;
    try {
        pret = std::stod(s);
    } catch (...) {
        throw std::invalid_argument("Pretul trebuie sa fie un numar valid: " + s);
    }
    if (pret < 0)
        throw std::invalid_argument("Pretul nu poate fi negativ.");
    return pret;
}

// ── Course operations ─────────────────────────────────────────────────────────

std::vector<Curs> CursService::getCursuri() const {
    return repo->loadAll();
}

void CursService::vizualizareCursuri() const {
    auto cursuri = repo->loadAll();
    if (cursuri.empty()) {
        std::cout << "Nu exista cursuri pe platforma.\n";
        return;
    }
    for (const auto& c : cursuri)
        std::cout << c << "\n";
}

void CursService::adaugareCurs(const std::string& titlu,
                               const std::string& descriere, const std::string& pretStr) {
    if (titlu.empty()) throw std::invalid_argument("Titlul cursului nu poate fi gol.");
    validateNoNewline(titlu,    "Titlul");
    validateNoNewline(descriere,"Descrierea");

    double pret = parsePret(pretStr);

    auto cursuri = repo->loadAll();
    std::string id = nextId(cursuri);
    cursuri.emplace_back(id, titlu, descriere, pret);
    repo->saveAll(cursuri);
    std::cout << Color::GREEN << "Cursul \"" << titlu << "\" a fost adaugat (ID: " << id << ")." << Color::RESET << "\n";
}

void CursService::stergereCurs(const std::string& id) {
    auto cursuri = repo->loadAll();
    auto it = findById(cursuri, id);
    if (it == cursuri.end())
        throw std::invalid_argument("Nu exista un curs cu id-ul: " + id);

    std::string titlu = it->getTitlu();
    cursuri.erase(it);
    repo->saveAll(cursuri);

    // Cascade delete: also remove every enrollment that points to this course,
    // otherwise inscrieri.txt would keep references to a course that is gone.
    auto inscrieri = inscriereRepo->loadAll();
    std::erase_if(inscrieri, [&](const Inscriere& i) { return i.getIdCurs() == id; });
    inscriereRepo->saveAll(inscrieri);

    std::cout << Color::GREEN << "Cursul \"" << titlu << "\" a fost sters." << Color::RESET << "\n";
}

void CursService::modificareCurs(const std::string& camp, const std::string& id,
                                  const std::string& valoare) {
    if (valoare.empty())
        throw std::invalid_argument("Valoarea noua nu poate fi goala.");
    if (camp == "titlu" || camp == "descriere")
        validateNoNewline(valoare, "Valoarea");

    auto cursuri = repo->loadAll();
    auto it = findById(cursuri, id);
    if (it == cursuri.end())
        throw std::invalid_argument("Nu exista un curs cu id-ul: " + id);

    if (camp == "titlu") {
        it->setTitlu(valoare);
    } else if (camp == "descriere") {
        it->setDescriere(valoare);
    } else if (camp == "pret") {
        it->setPret(parsePret(valoare));
    } else {
        throw std::invalid_argument("Camp invalid. Folositi: titlu, descriere sau pret.");
    }

    repo->saveAll(cursuri);
    std::cout << Color::GREEN << "Cursul [" << id << "] a fost actualizat (camp: " << camp << ")." << Color::RESET << "\n";
}

// ── Lesson operations ─────────────────────────────────────────────────────────

void CursService::adaugareLectie(const std::string& id_curs,
                                  const std::string& titlu, const std::string& durataStr) {
    if (titlu.empty()) throw std::invalid_argument("Titlul lectiei nu poate fi gol.");
    validateNoNewline(titlu, "Titlul lectiei");

    int durata;
    try {
        durata = std::stoi(durataStr);
    } catch (...) {
        throw std::invalid_argument("Durata trebuie sa fie un numar intreg valid: " + durataStr);
    }
    if (durata <= 0)
        throw std::invalid_argument("Durata trebuie sa fie pozitiva.");

    auto cursuri = repo->loadAll();
    auto it = findById(cursuri, id_curs);
    if (it == cursuri.end())
        throw std::invalid_argument("Nu exista un curs cu id-ul: " + id_curs);

    // Lesson ids restart from 1 inside each course — they only need to be
    // unique within their own course, not across the whole platform.
    std::string id_lectie = nextId(it->getLectii());
    it->addLectie(Lectie(id_lectie, titlu, durata));
    repo->saveAll(cursuri);
    std::cout << Color::GREEN << "Lectia \"" << titlu << "\" a fost adaugata la cursul [" << id_curs << "] (ID: " << id_lectie << ")." << Color::RESET << "\n";
}

void CursService::stergereLectie(const std::string& id_curs, const std::string& id_lectie) {
    auto cursuri = repo->loadAll();
    auto it = findById(cursuri, id_curs);
    if (it == cursuri.end())
        throw std::invalid_argument("Nu exista un curs cu id-ul: " + id_curs);

    if (!it->findLectie(id_lectie).has_value())
        throw std::invalid_argument("Nu exista o lectie cu id-ul: " + id_lectie);

    it->removeLectie(id_lectie);
    repo->saveAll(cursuri);

    // Data integrity: if any user had this lesson marked as completed, remove
    // the stale id from their enrollment so progress counts stay correct.
    // The file is rewritten only when something actually changed.
    auto inscrieri = inscriereRepo->loadAll();
    bool changed = false;
    for (auto& ins : inscrieri) {
        if (ins.getIdCurs() == id_curs && ins.esteLectieFinalizata(id_lectie)) {
            ins.nefinalizareLectie(id_lectie);
            changed = true;
        }
    }
    if (changed)
        inscriereRepo->saveAll(inscrieri);

    std::cout << Color::GREEN << "Lectia [" << id_lectie << "] a fost stearsa din cursul [" << id_curs << "]." << Color::RESET << "\n";
}
