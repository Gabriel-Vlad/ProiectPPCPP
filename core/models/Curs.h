#pragma once

#include <string>
#include <vector>
#include <ostream>
#include <istream>
#include <optional>
#include <algorithm>
#include <ranges>
#include "Lectie.h"

// A course on the platform, holding its own list of lessons
// Composition relationship with Lectie: the course owns its lessons directly
// (they live inside the "lectii" vector) — deleting the course deletes them
class Curs {
private:
    std::string         id_curs;    // numeric, auto-assigned
    std::string         titlu;
    std::string         descriere;
    double              pret;       // price in RON
    std::vector<Lectie> lectii;     // owned by this course (composition)
public:
    // Builds a course with no lessons; lessons are added afterwards
    Curs(std::string id, std::string titlu, std::string descriere, double pret);

    // Getters
    const std::string&         getId()        const;
    const std::string&         getTitlu()     const;
    const std::string&         getDescriere() const;
    double                     getPret()      const;
    const std::vector<Lectie>& getLectii()    const;

    // Setters
    void setTitlu(const std::string& titlu);
    void setDescriere(const std::string& descriere);
    void setPret(double pret);

    // Lesson management
    //
    // Appends a lesson at the end of this course
    void addLectie(const Lectie& lectie);

    // Removes the lesson with the given id. Does nothing if not found
    void removeLectie(const std::string& id_lectie);

    // Returns a copy of the lesson with the given id, or std::nullopt.
    // std::optional expresses "may not exist" without using null pointers —
    // the caller checks .has_value() before using the result
    std::optional<Lectie> findLectie(const std::string& id_lectie) const;

    // Serialization

    // Writes this course (and all its lessons) in the cursuri.txt format
    void serialize(std::ostream& out) const;

    // Reads one course back from the stream (the reverse of serialize)
    static Curs deserialize(std::istream& in);

    // Display
    //
    // Prints the full course info followed by its lessons
    friend std::ostream& operator<<(std::ostream& out, const Curs& c);
};
