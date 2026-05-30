#pragma once

#include <string>
#include <vector>
#include <ostream>
#include <istream>
#include <algorithm>
#include <ranges>

// Represents an enrollment of a user in a course.
// Association relationship with Curs: an enrollment references a course but does not own it.
// Tracks which lessons the user has completed.
class Inscriere {
public:
    // Constructs an enrollment for the given user and course ids.
    Inscriere(std::string id_user, std::string id_curs);

    // ── Getters ──────────────────────────────────────────────────────────────

    const std::string&              getIdUser()           const;
    const std::string&              getIdCurs()           const;
    const std::vector<std::string>& getLectiiFinalizate() const;

    // ── Lesson completion ────────────────────────────────────────────────────

    // Marks a lesson as completed. Does nothing if already marked.
    void finalizareLectie(const std::string& id_lectie);

    // Marks a lesson as not completed. Does nothing if not marked.
    void nefinalizareLectie(const std::string& id_lectie);

    // Returns true if the lesson with the given id is marked as completed.
    bool esteLectieFinalizata(const std::string& id_lectie) const;

    // ── Serialization ────────────────────────────────────────────────────────

    // Writes this enrollment to the output stream in the inscrieri.txt format.
    void serialize(std::ostream& os) const;

    // Reads and constructs an Inscriere from the input stream.
    static Inscriere deserialize(std::istream& is);

    // ── Display ──────────────────────────────────────────────────────────────

    // Prints: "<id_user> inscris la [<id_curs>] — X lectii finalizate"
    friend std::ostream& operator<<(std::ostream& os, const Inscriere& i);

private:
    std::string              id_user;
    std::string              id_curs;
    std::vector<std::string> lectii_finalizate;  // ids of completed lessons
};
