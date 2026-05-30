#include "Lectie.h"

// Moves strings into members to avoid unnecessary copies.
Lectie::Lectie(std::string id, std::string titlu, int durata_minute)
    : id_lectie(std::move(id)), titlu(std::move(titlu)), durata_minute(durata_minute) {}

// ── Getters ───────────────────────────────────────────────────────────────────

const std::string& Lectie::getId()     const { return id_lectie; }
const std::string& Lectie::getTitlu()  const { return titlu; }
int                Lectie::getDurata() const { return durata_minute; }

// ── Setters ───────────────────────────────────────────────────────────────────

void Lectie::setTitlu(const std::string& t) { titlu = t; }
void Lectie::setDurata(int d)               { durata_minute = d; }

// ── Display ───────────────────────────────────────────────────────────────────

// Format: "<id> - <titlu> (<durata> min)"
std::ostream& operator<<(std::ostream& os, const Lectie& l) {
    os << l.id_lectie << " - " << l.titlu << " (" << l.durata_minute << " min)";
    return os;
}
