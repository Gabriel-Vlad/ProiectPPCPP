#pragma once

#include <string>
#include <ostream>

// A single lesson belonging to a course.
// Composition relationship: a lesson never exists on its own — it lives inside
// a Curs and is destroyed together with it.
class Lectie {
public:
    // Builds a lesson from its id, title and duration in minutes.
    Lectie(std::string id, std::string titlu, int durata_minute);

    // ── Getters ──────────────────────────────────────────────────────────────

    const std::string& getId()    const;
    const std::string& getTitlu() const;
    int                getDurata() const;

    // ── Setters ──────────────────────────────────────────────────────────────

    void setTitlu(const std::string& titlu);
    void setDurata(int durata_minute);

    // ── Display ──────────────────────────────────────────────────────────────

    // Prints the lesson as: "<id> - <titlu> (<durata> min)"
    friend std::ostream& operator<<(std::ostream& os, const Lectie& l);

private:
    std::string id_lectie;      // unique only within the parent course
    std::string titlu;
    int         durata_minute;
};
