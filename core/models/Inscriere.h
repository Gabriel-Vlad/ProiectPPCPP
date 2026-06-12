#pragma once

#include <string>
#include <vector>
#include <ostream>
#include <istream>
#include <algorithm>
#include <ranges>

// An enrollment: links a user to a course and tracks which lessons of that
// course the user has completed.
// Association relationship with Curs: the enrollment only stores the course's
// id — it does not own the course object itself.
class Inscriere {
private:
    std::string              id_user;            // references Utilizator::id
    std::string              id_curs;            // references Curs::id_curs
    std::vector<std::string> lectii_finalizate;  // ids of completed lessons
public:
    // Creates a fresh enrollment with no completed lessons.
    Inscriere(std::string id_user, std::string id_curs);

    // Creates an enrollment with an already-known list of completed lessons.
    // Used when user IDs are rebalanced and records must be rebuilt as-is.
    Inscriere(std::string id_user, std::string id_curs, std::vector<std::string> lectii_finalizate);

    // Getters
    const std::string&              getIdUser()           const;
    const std::string&              getIdCurs()           const;
    const std::vector<std::string>& getLectiiFinalizate() const;

    // Lesson completion
    //
    // Marks a lesson as completed. Does nothing if it is already marked.
    void finalizareLectie(const std::string& id_lectie);

    // Removes a lesson from the completed list. Does nothing if not marked.
    void nefinalizareLectie(const std::string& id_lectie);

    // True if the given lesson id is in the completed list.
    bool esteLectieFinalizata(const std::string& id_lectie) const;

    // Serialization
    //
    // Writes this enrollment to the stream in the inscrieri.txt format.
    void serialize(std::ostream& out) const;

    // Reads one enrollment back from the stream (the reverse of serialize).
    static Inscriere deserialize(std::istream& in);

    // Display
    //
    // Prints: "<id_user> inscris la [<id_curs>] — X lectii finalizate"
    friend std::ostream& operator<<(std::ostream& out, const Inscriere& i);
};
