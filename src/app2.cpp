#include <iostream>
#include <string>
#include <stdexcept>
#include "../core/services/InscriereService.h"
#include "../core/services/UtilizatorService.h"
#include "../core/infra/colors.h"

// Prints the command reference shown by "app2 --help".
void printHelp() {
    std::cout
        << Color::CYAN << Color::BOLD << "Platforma Cursuri Online" << Color::RESET << " -- Utilizatori, Inscrieri si Progres\n"
        << "\n"
        << Color::YELLOW << "UTILIZARE:" << Color::RESET << "\n"
        << "  app2 <comanda> [argumente]\n"
        << "\n"
        << Color::YELLOW << "COMENZI UTILIZATORI:" << Color::RESET << "\n"
        << "  vizualizare_utilizatori                                          Afiseaza toti utilizatorii inregistrati\n"
        << "  inregistrare_utilizator  <nume>                                  Inregistreaza un utilizator nou (ID auto)\n"
        << "  stergere_utilizator      <id>                                   Sterge un utilizator\n"
        << "\n"
        << Color::YELLOW << "COMENZI INSCRIERI:" << Color::RESET << "\n"
        << "  vizualizare_cursuri_inscrise  <id_user>                         Afiseaza cursurile la care esti inscris\n"
        << "  inscriere                     <id_user> <id_curs>               Inscrie utilizatorul la un curs\n"
        << "  dezinscriere                  <id_user> <id_curs>               Dezinscrie utilizatorul dintr-un curs\n"
        << "  finalizare_lectie             <id_user> <id_curs> <id_lectie>   Marcheaza o lectie ca finalizata\n"
        << "  nefinalizare_lectie           <id_user> <id_curs> <id_lectie>   Marcheaza o lectie ca nefinalizata\n"
        << "  vizualizare_progres           <id_user> <id_curs>               Afiseaza progresul la un curs\n"
        << "  --help                                                          Afiseaza acest mesaj\n";
}

// app2 — user and enrollment CLI. Same thin-dispatcher structure as app1,
// but it needs two services: UtilizatorService for the user commands and
// InscriereService for the enrollment/progress commands.
int main(int argc, char* argv[]) {
    // argv[0] is the program name, so argc < 2 means no command was given.
    if (argc < 2) {
        std::cerr << Color::RED << "Eroare:" << Color::RESET
                  << " Utilizare: app2 <comanda> [argumente]\n"
                  << "Rulati " << Color::CYAN << "app2 --help" << Color::RESET << " pentru lista de comenzi.\n";
        return 1;
    }

    // --help is handled before constructing the services, so it works even
    // when the data files are missing.
    if (std::string(argv[1]) == "--help") {
        printHelp();
        return 0;
    }

    std::string comanda = argv[1];
    InscriereService  inscriereService;
    UtilizatorService utilizatorService;

    try {
        // ── User commands ─────────────────────────────────────────────────────
        if (comanda == "vizualizare_utilizatori") {
            if (argc != 2) throw std::invalid_argument("Utilizare: app2 vizualizare_utilizatori");
            utilizatorService.vizualizareUtilizatori();

        } else if (comanda == "inregistrare_utilizator") {
            if (argc != 3) throw std::invalid_argument("Utilizare: app2 inregistrare_utilizator <nume>");
            utilizatorService.inregistrareUtilizator(argv[2]);

        } else if (comanda == "stergere_utilizator") {
            if (argc != 3) throw std::invalid_argument("Utilizare: app2 stergere_utilizator <id>");
            utilizatorService.stergereUtilizator(argv[2]);

        // ── Enrollment commands ───────────────────────────────────────────────
        } else if (comanda == "vizualizare_cursuri_inscrise") {
            if (argc != 3) throw std::invalid_argument("Utilizare: app2 vizualizare_cursuri_inscrise <id_user>");
            inscriereService.vizualizareCursuriInscrise(argv[2]);

        } else if (comanda == "inscriere") {
            if (argc != 4) throw std::invalid_argument("Utilizare: app2 inscriere <id_user> <id_curs>");
            inscriereService.inscriere(argv[2], argv[3]);

        } else if (comanda == "dezinscriere") {
            if (argc != 4) throw std::invalid_argument("Utilizare: app2 dezinscriere <id_user> <id_curs>");
            inscriereService.dezinscriere(argv[2], argv[3]);

        } else if (comanda == "finalizare_lectie") {
            if (argc != 5) throw std::invalid_argument("Utilizare: app2 finalizare_lectie <id_user> <id_curs> <id_lectie>");
            inscriereService.finalizareLectie(argv[2], argv[3], argv[4]);

        } else if (comanda == "nefinalizare_lectie") {
            if (argc != 5) throw std::invalid_argument("Utilizare: app2 nefinalizare_lectie <id_user> <id_curs> <id_lectie>");
            inscriereService.nefinalizareLectie(argv[2], argv[3], argv[4]);

        } else if (comanda == "vizualizare_progres") {
            if (argc != 4) throw std::invalid_argument("Utilizare: app2 vizualizare_progres <id_user> <id_curs>");
            inscriereService.vizualizareProgres(argv[2], argv[3]);

        } else {
            throw std::invalid_argument("Comanda necunoscuta: \"" + comanda + "\". Rulati app2 --help pentru ajutor.");
        }

    // invalid_argument = the user's input was wrong; runtime_error = a data
    // file could not be opened. Both exit with code 1 so scripts can detect failure.
    } catch (const std::invalid_argument& e) {
        std::cerr << Color::RED << "Eroare:" << Color::RESET << " " << e.what() << "\n";
        return 1;
    } catch (const std::runtime_error& e) {
        std::cerr << Color::RED << "Eroare I/O:" << Color::RESET << " " << e.what() << "\n";
        return 1;
    }

    return 0;
}
