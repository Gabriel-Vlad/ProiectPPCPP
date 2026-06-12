#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../models/Curs.h"
#include "../models/Inscriere.h"
#include "../models/Utilizator.h"
#include "../infra/Repository.h"
#include "../infra/constants.h"

// All enrollment and progress tracking — used by app2 and the GUI.
// Reads three data files but only ever writes one:
//   - cursRepo       — read-only, validates that courses/lessons exist
//   - utilizatorRepo — read-only, validates that users exist
//   - inscriereRepo  — read/write, owns all enrollment data
class InscriereService {
private:
    // Smart pointers: each unique_ptr owns its repository and frees it
    // automatically when the service is destroyed (no manual delete needed).
    std::unique_ptr<Repository<Curs>>       cursRepo;
    std::unique_ptr<Repository<Inscriere>>  inscriereRepo;
    std::unique_ptr<Repository<Utilizator>> utilizatorRepo;

    // Throws if the user id is empty or does not exist in utilizatori.txt.
    void validateUtilizator(const std::string& id_user) const;

    // Shared implementation behind finalizareLectie / nefinalizareLectie —
    // both run the exact same validations, only the direction differs.
    void toggleLectie(const std::string& id_user, const std::string& id_curs, const std::string& id_lectie, bool finalizeaza);
public:
    // Creates the repositories and binds them to the data files.
    InscriereService();

    // Enrollment operations
    //
    // Prints all courses the user is enrolled in, or a message if none.
    void vizualizareCursuriInscrise(const std::string& id_user) const;

    // Enrolls a user in a course.
    // Throws if the user or course does not exist, or if already enrolled.
    void inscriere(const std::string& id_user, const std::string& id_curs);

    // Removes a user's enrollment.
    // Throws if the user does not exist or is not enrolled in that course.
    void dezinscriere(const std::string& id_user, const std::string& id_curs);

    // Lesson completion
    //
    // Marks a lesson as completed. Throws if course, lesson or enrollment is
    // missing, or if the lesson is already marked.
    void finalizareLectie(const std::string& id_user, const std::string& id_curs, const std::string& id_lectie);

    // Removes the completed mark from a lesson. Throws if course, lesson or
    // enrollment is missing, or if the lesson was not marked.
    void nefinalizareLectie(const std::string& id_user, const std::string& id_curs, const std::string& id_lectie);

    // Prints "X / Y lectii finalizate" plus a percentage for one enrollment.
    void vizualizareProgres(const std::string& id_user, const std::string& id_curs) const;

    // Returns all enrollments — used by the GUI to fill its lists.
    std::vector<Inscriere> getInscrieri() const;
};
