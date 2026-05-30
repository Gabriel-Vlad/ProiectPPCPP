#include <iostream>
#include <string>
#include <stdexcept>
#include "../common/services/InscriereService.h"

void printHelp() {
    std::cout <<
        "Utilizare: app2 <comanda> [argumente]\n"
        "\n"
        "Comenzi disponibile:\n"
        "  vizualizare_cursuri_inscrise <id_user>\n"
        "      Afiseaza cursurile la care utilizatorul este inscris.\n"
        "\n"
        "  inscriere <id_user> <id_curs>\n"
        "      Inscrie utilizatorul la un curs.\n"
        "\n"
        "  dezinscriere <id_user> <id_curs>\n"
        "      Dezinscrie utilizatorul dintr-un curs.\n"
        "\n"
        "  finalizare_lectie <id_user> <id_curs> <id_lectie>\n"
        "      Marcheaza o lectie ca finalizata.\n"
        "\n"
        "  nefinalizare_lectie <id_user> <id_curs> <id_lectie>\n"
        "      Marcheaza o lectie ca nefinalizata.\n"
        "\n"
        "  vizualizare_progres <id_user> <id_curs>\n"
        "      Afiseaza cate lectii a terminat utilizatorul din totalul cursului.\n"
        "\n"
        "  --help\n"
        "      Afiseaza acest mesaj.\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Utilizare: app2 <comanda> [argumente]\nRulati app2 --help pentru lista de comenzi.\n";
        return 1;
    }

    if (std::string(argv[1]) == "--help") {
        printHelp();
        return 0;
    }

    std::string comanda = argv[1];
    InscriereService service;

    try {
        if (comanda == "vizualizare_cursuri_inscrise") {
            if (argc != 3) throw std::invalid_argument("Utilizare: app2 vizualizare_cursuri_inscrise <id_user>");
            service.vizualizareCursuriInscrise(argv[2]);

        } else if (comanda == "inscriere") {
            if (argc != 4) throw std::invalid_argument("Utilizare: app2 inscriere <id_user> <id_curs>");
            service.inscriere(argv[2], argv[3]);

        } else if (comanda == "dezinscriere") {
            if (argc != 4) throw std::invalid_argument("Utilizare: app2 dezinscriere <id_user> <id_curs>");
            service.dezinscriere(argv[2], argv[3]);

        } else if (comanda == "finalizare_lectie") {
            if (argc != 5) throw std::invalid_argument("Utilizare: app2 finalizare_lectie <id_user> <id_curs> <id_lectie>");
            service.finalizareLectie(argv[2], argv[3], argv[4]);

        } else if (comanda == "nefinalizare_lectie") {
            if (argc != 5) throw std::invalid_argument("Utilizare: app2 nefinalizare_lectie <id_user> <id_curs> <id_lectie>");
            service.nefinalizareLectie(argv[2], argv[3], argv[4]);

        } else if (comanda == "vizualizare_progres") {
            if (argc != 4) throw std::invalid_argument("Utilizare: app2 vizualizare_progres <id_user> <id_curs>");
            service.vizualizareProgres(argv[2], argv[3]);

        } else {
            throw std::invalid_argument("Comanda necunoscuta: \"" + comanda + "\". Rulati app2 --help pentru ajutor.");
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
