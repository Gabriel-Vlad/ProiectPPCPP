#pragma once

#include <string>
#include <ostream>
#include <istream>

// A registered user on the platform. Holds only an id and a name —
// enrollments are stored separately in Inscriere, linked by the user id.
class Utilizator {
private:
    std::string id;    // numeric, auto-assigned, rebalanced after deletes
    std::string nume;  // full name, may contain spaces
public:
    Utilizator(std::string id, std::string nume);

    // Getters
    const std::string& getId()   const;
    const std::string& getNume() const;

    // Serialization
    //
    // Writes this user to the stream as one line: "<id> <nume>".
    void serialize(std::ostream& out) const;

    // Reads one user back from the stream (the reverse of serialize).
    static Utilizator deserialize(std::istream& in);

    // Display
    //
    // Prints the user as: "[<id>] <nume>"
    friend std::ostream& operator<<(std::ostream& out, const Utilizator& u);
};
