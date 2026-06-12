#include "Utilizator.h"
#include <sstream>

// Parameters are moved into the members to avoid copying the strings.
Utilizator::Utilizator(std::string id, std::string nume)
    : id(std::move(id)), nume(std::move(nume)) {}

const std::string& Utilizator::getId()   const { return id; }
const std::string& Utilizator::getNume() const { return nume; }

// One user per line: "<id> <nume>".
void Utilizator::serialize(std::ostream& out) const {
    out << id << " " << nume << "\n";
}

// Reads back one "<id> <nume>" line.
// The name can contain spaces, so we cannot simply do "ss >> id >> nume"
// (that would stop at the first space). Instead: extract the first token as
// the id, then take the entire rest of the line as the name.
Utilizator Utilizator::deserialize(std::istream& in) {
    std::string line;
    std::getline(in, line);
    std::istringstream ss(line);
    std::string id;
    ss >> id;
    std::string nume;
    std::getline(ss, nume);
    // getline keeps the separator space between id and name — strip it.
    if (!nume.empty() && nume.front() == ' ') nume.erase(0, 1);
    return Utilizator(id, nume);
}

// Format: "[<id>] <nume>"
std::ostream& operator<<(std::ostream& out, const Utilizator& u) {
    out << "[" << u.id << "] " << u.nume;
    return out;
}
