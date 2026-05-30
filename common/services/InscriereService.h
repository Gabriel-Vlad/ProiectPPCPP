#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../models/Curs.h"
#include "../models/Inscriere.h"
#include "../Repository.h"
#include "../constants.h"

// Handles all enrollment and progress tracking for app2.
// Owns both repositories via unique_ptr:
//   - cursRepo     — read-only, used to validate course/lesson existence
//   - inscriereRepo — read/write, owns all enrollment data
class InscriereService {
public:
    // Initializes the service and binds it to both data files.
    InscriereService();

    // ── Enrollment operations ────────────────────────────────────────────────

    // Prints all courses the user is enrolled in. If none, prints a message.
    void vizualizareCursuriInscrise(const std::string& id_user) const;

    // Enrolls a user in a course. Throws if course does not exist or user is already enrolled.
    void inscriere(const std::string& id_user, const std::string& id_curs);

    // Removes a user's enrollment. Throws if the user is not enrolled in the course.
    void dezinscriere(const std::string& id_user, const std::string& id_curs);

    // ── Lesson completion ────────────────────────────────────────────────────

    // Marks a lesson as completed. Throws if course, lesson or enrollment not found,
    // or if the lesson is already marked as completed.
    void finalizareLectie(const std::string& id_user, const std::string& id_curs,
                          const std::string& id_lectie);

    // Marks a lesson as not completed. Throws if course, lesson or enrollment not found,
    // or if the lesson is not marked as completed.
    void nefinalizareLectie(const std::string& id_user, const std::string& id_curs,
                            const std::string& id_lectie);

    // Prints how many lessons the user has completed out of the total for the course.
    void vizualizareProgres(const std::string& id_user, const std::string& id_curs) const;

private:
    std::unique_ptr<Repository<Curs>>      cursRepo;
    std::unique_ptr<Repository<Inscriere>> inscriereRepo;

    std::vector<Curs>      loadCursuri()   const;
    std::vector<Inscriere> loadInscrieri() const;
    void                   saveInscrieri(const std::vector<Inscriere>& inscrieri) const;

    // Returns an iterator to the course with the given id, or end() if not found.
    static auto findCurs(const std::vector<Curs>& cursuri, const std::string& id);

    // Returns an iterator to the enrollment matching user+course, or end() if not found.
    static auto findInscriere(std::vector<Inscriere>& inscrieri,
                              const std::string& id_user, const std::string& id_curs);
};
