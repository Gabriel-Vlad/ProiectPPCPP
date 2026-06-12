#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../models/Curs.h"
#include "../models/Inscriere.h"
#include "../infra/Repository.h"
#include "../infra/constants.h"

// All course and lesson management — used by app1 and the GUI.
// Every operation loads the file, applies the change, and saves it back,
// so the file on disk is always the single source of truth.
class CursService {
public:
    // Creates the repositories and binds them to the data files.
    CursService();

    // ── Course operations ────────────────────────────────────────────────────

    // Prints all courses to stdout, or a message if none exist.
    void vizualizareCursuri() const;

    // Adds a new course with an auto-generated id.
    // Throws if the title is empty or the price is not a valid positive number.
    void adaugareCurs(const std::string& titlu,
                      const std::string& descriere, const std::string& pretStr);

    // Removes a course, its lessons, and every enrollment that references it.
    // Throws if the id does not exist.
    void stergereCurs(const std::string& id);

    // Modifies one field (titlu, descriere or pret) of an existing course.
    // Throws if the id is missing, the field name is invalid or the price is invalid.
    void modificareCurs(const std::string& camp, const std::string& id,
                        const std::string& valoare);

    // ── Lesson operations ────────────────────────────────────────────────────

    // Adds a lesson with an auto-generated id to an existing course.
    // Throws if the course is missing, the title is empty or the duration is invalid.
    void adaugareLectie(const std::string& id_curs,
                        const std::string& titlu, const std::string& durataStr);

    // Removes a lesson from a course and cleans it out of every enrollment's
    // completed-lessons list. Throws if the course or lesson id does not exist.
    void stergereLectie(const std::string& id_curs, const std::string& id_lectie);

    // Returns all courses — used by the GUI to fill its lists.
    std::vector<Curs> getCursuri() const;

private:
    // std::unique_ptr is a smart pointer: it owns the object it points to and
    // deletes it automatically when the service is destroyed. No manual
    // new/delete is needed, and ownership cannot be accidentally shared —
    // a unique_ptr cannot be copied, only moved.
    std::unique_ptr<Repository<Curs>>      repo;
    std::unique_ptr<Repository<Inscriere>> inscriereRepo; // for cascade deletes only
};
