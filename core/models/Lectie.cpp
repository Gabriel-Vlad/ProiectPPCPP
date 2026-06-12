#include "Lectie.h"

// Parameters are taken by value and then moved into the members.
// std::move turns a copy into a cheap "steal the contents" operation —
// the caller's string is left empty and no duplicate allocation happens.
Lectie::Lectie(std::string id, std::string titlu, int durata_minute)
    : id_lectie(std::move(id)), titlu(std::move(titlu)), durata_minute(durata_minute) {}

// Getters
const std::string& Lectie::getId()     const { return id_lectie; }
const std::string& Lectie::getTitlu()  const { return titlu; }
int                Lectie::getDurata() const { return durata_minute; }

// Setters
void Lectie::setTitlu(const std::string& t) { titlu = t; }
void Lectie::setDurata(int d)               { durata_minute = d; }

// Display
//
// Format: "<id> - <titlu> (<durata> min)"
std::ostream& operator<<(std::ostream& out, const Lectie& l) {
    out << l.id_lectie << " - " << l.titlu << " (" << l.durata_minute << " min)";
    return out;
}
