#pragma once

#include <string>
#include <vector>
#include <ostream>
#include <istream>
#include <optional>
#include <algorithm>
#include <ranges>
#include "Lectie.h"

// Represents a course on the platform.
// Composition relationship with Lectie: a lesson cannot exist without its course.
// When a Curs is deleted, all its lessons are deleted with it.
class Curs {
public:
    // Constructs a course with a unique id, title, description and price (RON).
    Curs(std::string id, std::string titlu, std::string descriere, double pret);

    // ── Getters ──────────────────────────────────────────────────────────────

    const std::string&         getId()        const;
    const std::string&         getTitlu()     const;
    const std::string&         getDescriere() const;
    double                     getPret()      const;
    const std::vector<Lectie>& getLectii()    const;

    // ── Setters ──────────────────────────────────────────────────────────────

    void setTitlu(const std::string& titlu);
    void setDescriere(const std::string& descriere);
    void setPret(double pret);

    // ── Lesson management ────────────────────────────────────────────────────

    // Appends a lesson to this course.
    void addLectie(const Lectie& lectie);

    // Removes the lesson with the given id. Does nothing if not found.
    void removeLectie(const std::string& id_lectie);

    // Returns the lesson with the given id, or std::nullopt if not found.
    std::optional<Lectie> findLectie(const std::string& id_lectie) const;

    // ── Serialization ────────────────────────────────────────────────────────

    // Writes this course to the output stream in the cursuri.txt format.
    void serialize(std::ostream& os) const;

    // Reads and constructs a Curs from the input stream.
    static Curs deserialize(std::istream& is);

    // ── Display ──────────────────────────────────────────────────────────────

    // Prints course info and all its lessons to the output stream.
    friend std::ostream& operator<<(std::ostream& os, const Curs& c);

private:
    std::string         id_curs;
    std::string         titlu;
    std::string         descriere;
    double              pret;
    std::vector<Lectie> lectii;  // owned by this course (composition)
};
