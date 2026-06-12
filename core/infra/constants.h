#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

// Paths to the data files, relative to the project root.
// This is why all executables must be started from the project root directory.
inline const std::string CURSURI_FILE     = "data/cursuri.txt";
inline const std::string INSCRIERI_FILE   = "data/inscrieri.txt";
inline const std::string UTILIZATORI_FILE = "data/utilizatori.txt";

// Generates the next available numeric ID: highest existing ID + 1.
//
// This is a template: the compiler generates a separate version of this
// function for every type it is called with (Curs, Lectie, Utilizator...).
// The only requirement is that T has a getId() method returning a string.
// Non-numeric IDs are skipped by the catch block, so a corrupt entry cannot
// crash ID generation — it is simply ignored.
template<typename T>
std::string nextId(const std::vector<T>& items) {
    int maxId = 0;
    for (const auto& x : items)
        try { maxId = std::max(maxId, std::stoi(x.getId())); } catch (...) {}
    return std::to_string(maxId + 1);
}

// Finds an item by its ID. Returns an iterator to the item, or end() if missing.
// The caller must compare the result against items.end() before using it.
//
// Vec is deduced as either "std::vector<T>" or "const std::vector<T>", so the
// same template works for both read-only and mutable lookups — that is why
// the parameter is "Vec&" and not "std::vector<T>&".
template<typename Vec>
auto findById(Vec& items, const std::string& id) {
    return std::ranges::find_if(items, [&](const auto& x) { return x.getId() == id; });
}

// Rejects strings containing newline characters.
// The data files store one field per line, so a newline inside a title or a
// name would shift every following line and corrupt the whole file on save.
inline void validateNoNewline(const std::string& s, const std::string& fieldName) {
    if (s.find('\n') != std::string::npos || s.find('\r') != std::string::npos)
        throw std::invalid_argument(fieldName + " nu poate contine caractere newline.");
}
