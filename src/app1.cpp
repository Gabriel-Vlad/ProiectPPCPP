#include <iostream>
#include <string>
#include <stdexcept>
#include "../common/services/CursService.h"

void printHelp() {
    std::cout <<
        "Utilizare: app1 <comanda> [argumente]\n"
        "\n"
        "Comenzi disponibile:\n"
        "  vizualizare_cursuri\n"
        "      Afiseaza toate cursurile de pe platforma.\n"
        "\n"
        "  adaugare_curs <id> <titlu> <descriere> <pret>\n"
        "      Adauga un curs nou.\n"
        "\n"
        "  stergere_curs <id>\n"
        "      Sterge un curs impreuna cu lectiile lui.\n"
        "\n"
        "  modificare_curs <titlu|descriere|pret> <id> <valoare_noua>\n"
        "      Modifica titlul, descrierea sau pretul unui curs.\n"
        "\n"
        "  adaugare_lectie <id_curs> <id_lectie> <titlu> <durata_minute>\n"
        "      Adauga o lectie la un curs existent.\n"
        "\n"
        "  stergere_lectie <id_curs> <id_lectie>\n"
        "      Sterge o lectie dintr-un curs.\n"
        "\n"
        "  --help\n"
        "      Afiseaza acest mesaj.\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Utilizare: app1 <comanda> [argumente]\nRulati app1 --help pentru lista de comenzi.\n";
        return 1;
    }

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
            if (argc != 6) throw std::invalid_argument("Utilizare: app1 adaugare_curs <id> <titlu> <descriere> <pret>");
            service.adaugareCurs(argv[2], argv[3], argv[4], argv[5]);

        } else if (comanda == "stergere_curs") {
            if (argc != 3) throw std::invalid_argument("Utilizare: app1 stergere_curs <id>");
            service.stergereCurs(argv[2]);

        } else if (comanda == "modificare_curs") {
            if (argc != 5) throw std::invalid_argument("Utilizare: app1 modificare_curs <titlu|descriere|pret> <id> <valoare_noua>");
            service.modificareCurs(argv[2], argv[3], argv[4]);

        } else if (comanda == "adaugare_lectie") {
            if (argc != 6) throw std::invalid_argument("Utilizare: app1 adaugare_lectie <id_curs> <id_lectie> <titlu> <durata_minute>");
            service.adaugareLectie(argv[2], argv[3], argv[4], argv[5]);

        } else if (comanda == "stergere_lectie") {
            if (argc != 4) throw std::invalid_argument("Utilizare: app1 stergere_lectie <id_curs> <id_lectie>");
            service.stergereLectie(argv[2], argv[3]);

        } else {
            throw std::invalid_argument("Comanda necunoscuta: \"" + comanda + "\". Rulati app1 --help pentru ajutor.");
        }

    } catch (const std::invalid_argument& e) {
        std::cerr << "Eroare: " << e.what() << "\n";
        return 1;
    } catch (const std::runtime_error& e) {
        std::cerr << "Eroare I/O: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
