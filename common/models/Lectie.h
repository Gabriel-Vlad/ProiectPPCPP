#pragma once

#include <string>
#include <ostream>

// Represents a single lesson belonging to a course.
// Composition relationship: a lesson cannot exist without its parent course.
class Lectie {
public:
    // Constructs a lesson with a unique id, title and duration in minutes.
    Lectie(std::string id, std::string titlu, int durata_minute);

    // ── Getters ──────────────────────────────────────────────────────────────

    const std::string& getId()    const;
    const std::string& getTitlu() const;
    int                getDurata() const;

    // ── Setters ──────────────────────────────────────────────────────────────

    void setTitlu(const std::string& titlu);
    void setDurata(int durata_minute);

    // ── Display ──────────────────────────────────────────────────────────────

    // Prints the lesson in the format: "<id> - <titlu> (<durata> min)"
    friend std::ostream& operator<<(std::ostream& os, const Lectie& l);

private:
    std::string id_lectie;
    std::string titlu;
    int         durata_minute;
};
