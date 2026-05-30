#include "CursService.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

// Binds the repository to the courses data file defined in constants.h.
CursService::CursService()
    : repo(std::make_unique<Repository<Curs>>(CURSURI_FILE)) {}

// ── Private helpers ───────────────────────────────────────────────────────────

std::vector<Curs> CursService::load() const {
    return repo->loadAll();
}

void CursService::save(const std::vector<Curs>& cursuri) const {
    repo->saveAll(cursuri);
}

// Linear search by id — dataset is small so no index needed.
auto CursService::find(std::vector<Curs>& cursuri, const std::string& id) {
    return std::ranges::find_if(cursuri, [&](const Curs& c) {
        return c.getId() == id;
    });
}

// ── Course operations ─────────────────────────────────────────────────────────

void CursService::vizualizareCursuri() const {
    auto cursuri = load();
    if (cursuri.empty()) {
        std::cout << "Nu exista cursuri pe platforma.\n";
        return;
    }
    // operator<< on Curs prints id, title, description, price and all lessons.
    for (const auto& c : cursuri)
        std::cout << c << "\n";
}

void CursService::adaugareCurs(const std::string& id, const std::string& titlu,
                               const std::string& descriere, const std::string& pretStr) {
    double pret;
    try {
        pret = std::stod(pretStr);
    } catch (...) {
        throw std::invalid_argument("Pretul trebuie sa fie un numar valid: " + pretStr);
    }
    if (pret < 0)
        throw std::invalid_argument("Pretul nu poate fi negativ.");

    auto cursuri = load();
    if (find(cursuri, id) != cursuri.end())
        throw std::invalid_argument("Exista deja un curs cu id-ul: " + id);

    cursuri.emplace_back(id, titlu, descriere, pret);
    save(cursuri);
    std::cout << "Cursul \"" << titlu << "\" a fost adaugat.\n";
}

void CursService::stergereCurs(const std::string& id) {
    auto cursuri = load();
    auto it = find(cursuri, id);
    if (it == cursuri.end())
        throw std::invalid_argument("Nu exista un curs cu id-ul: " + id);

    std::string titlu = it->getTitlu();
    cursuri.erase(it);
    save(cursuri);
    std::cout << "Cursul \"" << titlu << "\" a fost sters.\n";
}

void CursService::modificareCurs(const std::string& camp, const std::string& id,
                                  const std::string& valoare) {
    auto cursuri = load();
    auto it = find(cursuri, id);
    if (it == cursuri.end())
        throw std::invalid_argument("Nu exista un curs cu id-ul: " + id);

    if (camp == "titlu") {
        it->setTitlu(valoare);
    } else if (camp == "descriere") {
        it->setDescriere(valoare);
    } else if (camp == "pret") {
        double pret;
        try {
            pret = std::stod(valoare);
        } catch (...) {
            throw std::invalid_argument("Pretul trebuie sa fie un numar valid: " + valoare);
        }
        if (pret < 0)
            throw std::invalid_argument("Pretul nu poate fi negativ.");
        it->setPret(pret);
    } else {
        throw std::invalid_argument("Camp invalid. Folositi: titlu, descriere sau pret.");
    }

    save(cursuri);
    std::cout << "Cursul [" << id << "] a fost actualizat (camp: " << camp << ").\n";
}

// ── Lesson operations ─────────────────────────────────────────────────────────

void CursService::adaugareLectie(const std::string& id_curs, const std::string& id_lectie,
                                  const std::string& titlu, const std::string& durataStr) {
    int durata;
    try {
        durata = std::stoi(durataStr);
    } catch (...) {
        throw std::invalid_argument("Durata trebuie sa fie un numar intreg valid: " + durataStr);
    }
    if (durata <= 0)
        throw std::invalid_argument("Durata trebuie sa fie pozitiva.");

    auto cursuri = load();
    auto it = find(cursuri, id_curs);
    if (it == cursuri.end())
        throw std::invalid_argument("Nu exista un curs cu id-ul: " + id_curs);

    if (it->findLectie(id_lectie).has_value())
        throw std::invalid_argument("Exista deja o lectie cu id-ul: " + id_lectie);

    it->addLectie(Lectie(id_lectie, titlu, durata));
    save(cursuri);
    std::cout << "Lectia \"" << titlu << "\" a fost adaugata la cursul [" << id_curs << "].\n";
}

void CursService::stergereLectie(const std::string& id_curs, const std::string& id_lectie) {
    auto cursuri = load();
    auto it = find(cursuri, id_curs);
    if (it == cursuri.end())
        throw std::invalid_argument("Nu exista un curs cu id-ul: " + id_curs);

    if (!it->findLectie(id_lectie).has_value())
        throw std::invalid_argument("Nu exista o lectie cu id-ul: " + id_lectie);

    it->removeLectie(id_lectie);
    save(cursuri);
    std::cout << "Lectia [" << id_lectie << "] a fost stearsa din cursul [" << id_curs << "].\n";
}
