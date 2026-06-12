# Platforma Cursuri Online

Proiect C++23 — platformă de gestiune a cursurilor online cu două aplicații CLI și o interfață grafică ImGui.

---

## Structura proiectului

```text
ProiectPPC++/
├── core/                           # Echivalent "shared/" — cod comun
│   ├── models/                     # Clasele domeniului
│   │   ├── Curs.h / Curs.cpp
│   │   ├── Lectie.h / Lectie.cpp
│   │   ├── Inscriere.h / Inscriere.cpp
│   │   └── Utilizator.h / Utilizator.cpp
│   ├── services/                   # Logica de business
│   │   ├── CursService.h / .cpp
│   │   ├── InscriereService.h / .cpp
│   │   └── UtilizatorService.h / .cpp
│   └── infra/                      # Infrastructura
│       ├── Repository.h            # Template generic pentru citire/scriere fisiere
│       ├── constants.h             # Cai fisiere + template-urile nextId/findById + validare
│       └── colors.h                # Culori ANSI pentru terminal
│
├── src/                            # Punctele de intrare ale aplicatiilor
│   ├── app1.cpp                    # Aplicatia 1: gestiune cursuri si lectii
│   ├── app2.cpp                    # Aplicatia 2: utilizatori, inscrieri, progres
│   ├── app_gui.cpp                 # Aplicatia grafica
│   └── gui/                        # Module interfata grafica
│       ├── state.h                 # AppState — starea globala a GUI
│       ├── theme.h                 # Tema vizuala ImGui
│       ├── tab_cursuri.h           # Tab-ul Cursuri
│       ├── tab_inscrieri.h         # Tab-ul Inscrieri
│       ├── popups.h                # Popup-uri formular si confirmare
│       └── utils.h                 # Utilitare (formatare pret, randare rand curs)
│
├── data/                           # Echivalent "shared/files/" — fisiere de date
│   ├── cursuri.txt
│   ├── inscrieri.txt
│   └── utilizatori.txt
│
├── assets/
│   └── arial.ttf                   # Font pentru GUI
├── imgui/                          # Librarie ImGui 1.91.9b
├── Makefile
├── documentatie.tex                # Documentatia proiectului (sursa LaTeX)
└── README.md
```

---

## Compilare

Necesita: **g++ cu suport C++23**, **MinGW/MSYS2**, **SDL2**, **OpenGL**, **GLEW**, **ImGui**.

```bash
mingw32-make        # compileaza toate cele 3 executabile
mingw32-make clean  # sterge fisierele compilate
```

Executabilele se genereaza in `bin/`.

---

## Rulare

> **Important:** toate executabilele trebuie rulate din directorul radacina al proiectului, altfel fisierele `data/` si `assets/` nu sunt gasite.

```bash
bin/app1.exe <comanda> [argumente]
bin/app2.exe <comanda> [argumente]
bin/app_gui.exe
```

---

## app1 — Gestiune cursuri si lectii

```text
bin/app1.exe vizualizare_cursuri
bin/app1.exe adaugare_curs    <titlu> <descriere> <pret>
bin/app1.exe stergere_curs    <id>
bin/app1.exe modificare_curs  <titlu|descriere|pret> <id> <valoare_noua>
bin/app1.exe adaugare_lectie  <id_curs> <titlu> <durata_minute>
bin/app1.exe stergere_lectie  <id_curs> <id_lectie>
bin/app1.exe --help
```

**Exemple:**

```bash
bin/app1.exe adaugare_curs "Matematica" "Algebra liniara" 99.99
bin/app1.exe adaugare_lectie 1 "Vectori" 45
bin/app1.exe modificare_curs pret 1 149.99
bin/app1.exe stergere_lectie 1 1
bin/app1.exe stergere_curs 1
```

---

## app2 — Utilizatori, inscrieri si progres

```text
bin/app2.exe vizualizare_utilizatori
bin/app2.exe inregistrare_utilizator  <nume>
bin/app2.exe stergere_utilizator      <id>

bin/app2.exe inscriere                    <id_user> <id_curs>
bin/app2.exe dezinscriere                 <id_user> <id_curs>
bin/app2.exe vizualizare_cursuri_inscrise <id_user>
bin/app2.exe finalizare_lectie            <id_user> <id_curs> <id_lectie>
bin/app2.exe nefinalizare_lectie          <id_user> <id_curs> <id_lectie>
bin/app2.exe vizualizare_progres          <id_user> <id_curs>
bin/app2.exe --help
```

**Exemple:**

```bash
bin/app2.exe inregistrare_utilizator "Ion Popescu"
bin/app2.exe inscriere 1 1
bin/app2.exe finalizare_lectie 1 1 1
bin/app2.exe vizualizare_progres 1 1
bin/app2.exe stergere_utilizator 1
```

---

## app_gui — Interfata grafica

Interfata ImGui cu doua tab-uri:

- **Cursuri** — vizualizare, adaugare, editare si stergere cursuri si lectii
- **Inscrieri** — selectare utilizator, inscriere/dezinscriere la cursuri, marcare lectii ca finalizate, bara de progres

---

## Structura fisierelor de date

### cursuri.txt

```text
<id_curs> <titlu_curs> <descriere_curs> <pret_curs>
<numar_lectii_curs>
<id_lectie> <titlu_lectie> <durata_minute>
...
```

### inscrieri.txt

```text
<id_user> <id_curs> <numar_lectii_finalizate>
<id_lectie_finalizata1> <id_lectie_finalizata2> ...
```

### utilizatori.txt

```text
<id_user> <nume_utilizator>
```

Nu exista numar de inregistrari la inceputul fisierului — fiecare linie este un record independent.

---

## Arhitectura

| Componenta | Responsabilitate |
|---|---|
| `Repository<T>` | Citire/scriere generica din fisier, fara a cunoaste tipul T |
| `CursService` | Gestiune cursuri si lectii, cascade delete la stergere |
| `InscriereService` | Inscrieri, validare utilizator si curs, progres lectii |
| `UtilizatorService` | Inregistrare/stergere utilizatori, rebalansare ID-uri |
| `AppState` | Starea centrala a GUI, reload din fisiere la fiecare mutatie |

**Relatii intre modele:**

- `Curs` ↔ `Lectie` — compozitie (lectia nu exista fara curs)
- `Inscriere` ↔ `Curs` — asociere (inscrierea referentiaza cursul prin ID, nu il detine)
- `Inscriere` ↔ `Utilizator` — asociere prin ID

**Integritate si validare:**

- ID-uri auto-generate (numeric, urmatorul numar liber)
- Cascade delete: stergerea unui curs sterge si inscrierile lui; stergerea unui utilizator reechilibreaza ID-urile si actualizeaza inscrierile
- Validare la intrare: pret pozitiv, durata pozitiva, ID-uri existente, campuri ne-goale, fara caractere newline (care ar corupe formatul fisierelor)

---

## Tehnologii

- **C++23** — `std::ranges`, `std::erase_if`, `std::ranges::contains`
- **ImGui 1.91.9b** + **SDL2** + **OpenGL 3.3** + **GLEW** — interfata grafica
- **MinGW/MSYS2** — compilator si librarii pe Windows
