#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <optional>
#include <algorithm>
#include <ranges>

// Generic file-backed repository for reading and writing collections of objects.
//
// T must implement:
//   static T deserialize(std::istream&) — construct one object from a stream
//   void     serialize(std::ostream&)   — write one object to a stream
//
// The file format is: <count>\n followed by count serialized objects.
template <typename T>
class Repository {
public:
    // Constructs a repository bound to the given file path.
    explicit Repository(const std::string& filepath) : filepath(filepath) {}

    // Loads all objects from the file.
    // Throws std::runtime_error if the file cannot be opened.
    std::vector<T> loadAll() const {
        std::ifstream file(filepath);
        if (!file.is_open())
            throw std::runtime_error("Nu s-a putut deschide fisierul: " + filepath);

        std::vector<T> items;
        int n = 0;
        file >> n;
        file.ignore();

        for (int i = 0; i < n; i++)
            items.push_back(T::deserialize(file));

        return items;
    }

    // Saves all objects to the file, overwriting existing content.
    // Throws std::runtime_error if the file cannot be opened.
    void saveAll(const std::vector<T>& items) const {
        std::ofstream file(filepath);
        if (!file.is_open())
            throw std::runtime_error("Nu s-a putut scrie in fisierul: " + filepath);

        file << items.size() << "\n";
        for (const auto& item : items)
            item.serialize(file);
    }

    // Returns the first item matching the predicate, or std::nullopt if not found.
    // Predicate is a callable that takes a const T& and returns bool.
    template <typename Predicate>
    std::optional<T> findOne(const std::vector<T>& items, Predicate pred) const {
        auto it = std::ranges::find_if(items, pred);
        if (it == items.end()) return std::nullopt;
        return *it;
    }

private:
    std::string filepath;
};
