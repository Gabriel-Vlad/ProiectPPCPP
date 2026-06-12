#pragma once

#include <string>
#include <ostream>
#include <istream>

// A registered user on the platform. Holds only an id and a name —
// enrollments are stored separately in Inscriere, linked by the user id.
class Utilizator {
public:
    Utilizator(std::string id, std::string nume);

    // ── Getters ──────────────────────────────────────────────────────────────

    const std::string& getId()   const;
    const std::string& getNume() const;

    // ── Serialization ────────────────────────────────────────────────────────

    // Writes this user to the stream as one line: "<id> <nume>".
    void serialize(std::ostream& os) const;

    // Reads one user back from the stream (the reverse of serialize).
    static Utilizator deserialize(std::istream& is);

    // ── Display ──────────────────────────────────────────────────────────────

    // Prints the user as: "[<id>] <nume>"
    friend std::ostream& operator<<(std::ostream& os, const Utilizator& u);

private:
    std::string id;    // numeric, auto-assigned, rebalanced after deletes
    std::string nume;  // full name, may contain spaces
};
