#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../models/Curs.h"
#include "../Repository.h"
#include "../constants.h"

// Handles all course and lesson management for app1.
// Owns the course repository via a unique_ptr — lifetime is tied to this service.
class CursService {
public:
    // Initializes the service and binds it to the courses data file.
    CursService();

    // ── Course operations ────────────────────────────────────────────────────

    // Prints all courses to stdout. If no courses exist, prints a message.
    void vizualizareCursuri() const;

    // Adds a new course. Throws if the id already exists or price is invalid.
    void adaugareCurs(const std::string& id, const std::string& titlu,
                      const std::string& descriere, const std::string& pretStr);

    // Removes a course and all its lessons. Throws if the id does not exist.
    void stergereCurs(const std::string& id);

    // Modifies one field (titlu, descriere, or pret) of an existing course.
    // Throws if the id does not exist, the field name is invalid, or price is invalid.
    void modificareCurs(const std::string& camp, const std::string& id,
                        const std::string& valoare);

    // ── Lesson operations ────────────────────────────────────────────────────

    // Adds a lesson to an existing course. Throws if course or lesson id already exists.
    void adaugareLectie(const std::string& id_curs, const std::string& id_lectie,
                        const std::string& titlu, const std::string& durataStr);

    // Removes a lesson from a course. Throws if course or lesson id does not exist.
    void stergereLectie(const std::string& id_curs, const std::string& id_lectie);

private:
    std::unique_ptr<Repository<Curs>> repo;

    std::vector<Curs> load() const;
    void              save(const std::vector<Curs>& cursuri) const;

    // Returns an iterator to the course with the given id, or end() if not found.
    static auto find(std::vector<Curs>& cursuri, const std::string& id);
};
