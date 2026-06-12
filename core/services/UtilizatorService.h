#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../models/Utilizator.h"
#include "../models/Inscriere.h"
#include "../infra/Repository.h"
#include "../infra/constants.h"

// User registration, listing and deletion — used by app2 and the GUI.
// User IDs are kept dense (1, 2, 3...): when a user is deleted, everyone
// after them shifts down by one, and inscrieri.txt is updated to match.
class UtilizatorService {
private:
    // Smart pointers: each unique_ptr owns its repository and frees it
    // automatically when the service is destroyed.
    std::unique_ptr<Repository<Utilizator>> repo;
    std::unique_ptr<Repository<Inscriere>>  inscriereRepo; // updated when IDs shift
public:
    // Creates the repositories and binds them to the data files.
    UtilizatorService();

    // Registers a new user with an auto-generated id. Throws if name is empty.
    void inregistrareUtilizator(const std::string& nume);

    // Removes a user, deletes their enrollments, and rebalances the IDs of
    // every remaining user (and of their enrollments) to stay 1, 2, 3...
    // Throws if the id does not exist.
    void stergereUtilizator(const std::string& id);

    // Prints all registered users, or a message if none exist.
    void vizualizareUtilizatori() const;

    // Returns all registered users — used by the GUI to fill its lists.
    std::vector<Utilizator> getUtilizatori() const;
};
