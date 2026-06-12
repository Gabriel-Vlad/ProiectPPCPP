#include <iostream>
#include <string>
#include <stdexcept>
#include "../core/services/CursService.h"
#include "../core/infra/colors.h"

// Prints the command reference shown by "app1 --help".
void printHelp() {
    std::cout
        << Color::CYAN << Color::BOLD << "Platforma Cursuri Online" << Color::RESET << " -- Gestiune cursuri si lectii\n"
        << "\n"
        << Color::YELLOW << "UTILIZARE:" << Color::RESET << "\n"
        << "  app1 <comanda> [argumente]\n"
        << "\n"
        << Color::YELLOW << "COMENZI:" << Color::RESET << "\n"
        << "  vizualizare_cursuri                                         Afiseaza toate cursurile de pe platforma\n"
        << "  adaugare_curs    <titlu> <descriere> <pret>                 Adauga un curs nou (ID auto)\n"
        << "  stergere_curs    <id>                                       Sterge cursul si toate lectiile lui\n"
        << "  modificare_curs  <camp> <id> <valoare_noua>                 Modifica un camp al cursului\n"
        << "  adaugare_lectie  <id_curs> <titlu> <durata>                 Adauga o lectie la un curs (ID auto)\n"
        << "  stergere_lectie  <id_curs> <id_lectie>                      Sterge o lectie dintr-un curs\n"
        << "  --help                                                      Afiseaza acest mesaj\n"
        << "\n"
        << Color::MUTED << "CAMPURI pentru modificare_curs:  titlu | descriere | pret" << Color::RESET << "\n";
}

// app1 — course and lesson management CLI. It is a thin dispatcher:
// it validates the argument count for each command and delegates all the
// real work (and the real validation) to CursService.
int main(int argc, char* argv[]) {
    // argv[0] is the program name, so argc < 2 means no command was given.
    if (argc < 2) {
        std::cerr << Color::RED << "Eroare:" << Color::RESET
                  << " Utilizare: app1 <comanda> [argumente]\n"
                  << "Rulati " << Color::CYAN << "app1 --help" << Color::RESET << " pentru lista de comenzi.\n";
        return 1;
    }

    // --help is handled before constructing the service, so it works even
    // when the data files are missing.
    if (std::string(argv[1]) == "--help") {
        printHelp();
        return 0;
    }

    std::string comanda = argv[1];
    CursService service;

    try {
        if (comanda == "vizualizare_cursuri") {
            if (argc != 2) throw std::invalid_argument("Utilizare: app1 vizualizare_cursuri");
            service.vizualizareCursuri();

        } else if (comanda == "adaugare_curs") {
            if (argc != 5) throw std::invalid_argument("Utilizare: app1 adaugare_curs <titlu> <descriere> <pret>");
            service.adaugareCurs(argv[2], argv[3], argv[4]);

        } else if (comanda == "stergere_curs") {
            if (argc != 3) throw std::invalid_argument("Utilizare: app1 stergere_curs <id>");
            service.stergereCurs(argv[2]);

        } else if (comanda == "modificare_curs") {
            if (argc != 5) throw std::invalid_argument("Utilizare: app1 modificare_curs <titlu|descriere|pret> <id> <valoare_noua>");
            service.modificareCurs(argv[2], argv[3], argv[4]);

        } else if (comanda == "adaugare_lectie") {
            if (argc != 5) throw std::invalid_argument("Utilizare: app1 adaugare_lectie <id_curs> <titlu> <durata_minute>");
            service.adaugareLectie(argv[2], argv[3], argv[4]);

        } else if (comanda == "stergere_lectie") {
            if (argc != 4) throw std::invalid_argument("Utilizare: app1 stergere_lectie <id_curs> <id_lectie>");
            service.stergereLectie(argv[2], argv[3]);

        } else {
            throw std::invalid_argument("Comanda necunoscuta: \"" + comanda + "\". Rulati app1 --help pentru ajutor.");
        }

    // Two error categories: invalid_argument = the user's input was wrong,
    // runtime_error = a data file could not be opened. Both exit with code 1
    // so scripts can detect failure.
    } catch (const std::invalid_argument& e) {
        std::cerr << Color::RED << "Eroare:" << Color::RESET << " " << e.what() << "\n";
        return 1;
    } catch (const std::runtime_error& e) {
        std::cerr << Color::RED << "Eroare I/O:" << Color::RESET << " " << e.what() << "\n";
        return 1;
    }

    return 0;
}
