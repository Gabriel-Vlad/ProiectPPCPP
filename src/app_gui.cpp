#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "../common/models/Curs.h"
#include "../common/models/Inscriere.h"
#include "../common/services/CursService.h"
#include "../common/services/InscriereService.h"
#include "../common/Repository.h"
#include "../common/constants.h"

// ─── Constants ────────────────────────────────────────────────────────────────

static constexpr float        W          = 1600.f;
static constexpr float        H          = 900.f;
static constexpr float        PAD        = 16.f;
static constexpr float        TOPBAR_H   = 60.f;
static constexpr float        TABBAR_H   = 46.f;
static constexpr float        LEFT_W     = 420.f;
static constexpr unsigned int FN_LG      = 20;
static constexpr unsigned int FN_MD      = 17;
static constexpr unsigned int FN_SM      = 14;

static const sf::Color C_BG       { 28,  28,  38 };
static const sf::Color C_PANEL    { 40,  40,  55 };
static const sf::Color C_SEL      { 60,  100, 180 };
static const sf::Color C_HOVER    { 50,  50,  70 };
static const sf::Color C_TEXT     { 220, 220, 230 };
static const sf::Color C_MUTED    { 130, 130, 155 };
static const sf::Color C_ACCENT   { 100, 160, 255 };
static const sf::Color C_GREEN    { 80,  200, 120 };
static const sf::Color C_RED      { 220, 80,  80  };
static const sf::Color C_INPUT_BG { 35,  35,  50  };
static const sf::Color C_INPUT_BD { 80,  80,  110 };
static const sf::Color C_BTN      { 60,  100, 180 };
static const sf::Color C_BTN_HOV  { 80,  120, 210 };

// ─── Utility ──────────────────────────────────────────────────────────────────

static std::string fmtPret(double p) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << p;
    return ss.str();
}

static std::string wrapText(const std::string& text, const sf::Font& font,
                             unsigned int size, float maxW) {
    std::string result, word, line;
    auto lineW = [&](const std::string& s) {
        sf::Text t(font, s, size);
        return t.getLocalBounds().size.x;
    };
    for (char ch : text + ' ') {
        if (ch == ' ' || ch == '\n') {
            std::string test = line.empty() ? word : line + ' ' + word;
            if (lineW(test) > maxW && !line.empty()) { result += line + '\n'; line = word; }
            else line = test;
            word.clear();
        } else word += ch;
    }
    if (!line.empty()) result += line;
    return result;
}

// ─── UI Primitives ────────────────────────────────────────────────────────────

struct Button {
    sf::FloatRect rect;
    std::string   label;
    sf::Color     color    = C_BTN;
    sf::Color     hovColor = C_BTN_HOV;

    bool contains(sf::Vector2f p) const { return rect.contains(p); }

    void draw(sf::RenderWindow& w, const sf::Font& font, sf::Vector2f mouse) const {
        sf::RectangleShape bg({ rect.size.x, rect.size.y });
        bg.setPosition(rect.position);
        bg.setFillColor(rect.contains(mouse) ? hovColor : color);
        bg.setOutlineColor(C_INPUT_BD);
        bg.setOutlineThickness(1.f);
        w.draw(bg);

        sf::Text t(font, label, FN_SM);
        t.setFillColor(sf::Color::White);
        auto tb = t.getLocalBounds();
        t.setPosition({
            rect.position.x + (rect.size.x - tb.size.x) / 2.f,
            rect.position.y + (rect.size.y - tb.size.y) / 2.f - 2.f
        });
        w.draw(t);
    }
};

struct TextInput {
    sf::FloatRect rect;
    std::string   value;
    std::string   placeholder;
    bool          focused = false;

    bool contains(sf::Vector2f p) const { return rect.contains(p); }

    void handleEvent(const sf::Event& ev) {
        if (!focused) return;
        if (const auto* t = ev.getIf<sf::Event::TextEntered>()) {
            if (t->unicode == 8 && !value.empty()) value.pop_back();
            else if (t->unicode >= 32 && t->unicode < 127)
                value += static_cast<char>(t->unicode);
        }
    }

    void draw(sf::RenderWindow& w, const sf::Font& font) const {
        sf::RectangleShape bg({ rect.size.x, rect.size.y });
        bg.setPosition(rect.position);
        bg.setFillColor(C_INPUT_BG);
        bg.setOutlineColor(focused ? C_ACCENT : C_INPUT_BD);
        bg.setOutlineThickness(1.f);
        w.draw(bg);

        std::string display = value.empty() ? placeholder : value;
        sf::Text t(font, display, FN_SM);
        t.setFillColor(value.empty() ? C_MUTED : C_TEXT);
        t.setPosition({ rect.position.x + 8.f, rect.position.y + (rect.size.y - 14.f) / 2.f });
        w.draw(t);

        // Blinking cursor
        if (focused && !value.empty()) {
            sf::Text cur(font, display + "|", FN_SM);
            sf::RectangleShape caret({ 1.f, 16.f });
            float cx = rect.position.x + 8.f + cur.getLocalBounds().size.x;
            caret.setPosition({ cx, rect.position.y + 8.f });
            caret.setFillColor(C_TEXT);
            w.draw(caret);
        }
    }
};

// ─── Popup: Confirmation ──────────────────────────────────────────────────────

struct ConfirmPopup {
    bool        visible = false;
    std::string message;
    bool        result  = false;
    bool        done    = false;

    Button btnDa  { {{460.f, 370.f}, {140.f, 36.f}}, "Da",  C_RED,   {200,60,60} };
    Button btnNu  { {{620.f, 370.f}, {140.f, 36.f}}, "Nu",  C_PANEL, C_HOVER     };

    void show(const std::string& msg) {
        message = msg; visible = true; result = false; done = false;
    }

    void handleClick(sf::Vector2f p) {
        if (!visible) return;
        if (btnDa.contains(p)) { result = true;  done = true; visible = false; }
        if (btnNu.contains(p)) { result = false; done = true; visible = false; }
    }

    void draw(sf::RenderWindow& w, const sf::Font& font, sf::Vector2f mouse) const {
        if (!visible) return;
        // Overlay
        sf::RectangleShape overlay({W, H});
        overlay.setFillColor({0, 0, 0, 160});
        w.draw(overlay);
        // Box
        sf::RectangleShape box({360.f, 120.f});
        box.setPosition({460.f, 310.f});
        box.setFillColor(C_PANEL);
        box.setOutlineColor(C_INPUT_BD);
        box.setOutlineThickness(1.f);
        w.draw(box);
        // Message
        sf::Text t(font, message, FN_MD);
        t.setFillColor(C_TEXT);
        auto tb = t.getLocalBounds();
        t.setPosition({ 460.f + (360.f - tb.size.x) / 2.f, 325.f });
        w.draw(t);
        btnDa.draw(w, font, mouse);
        btnNu.draw(w, font, mouse);
    }
};

// ─── Popup: Form (generic 1-4 fields) ────────────────────────────────────────

struct FormPopup {
    bool                     visible = false;
    std::string              title;
    std::vector<std::string> labels;
    std::vector<TextInput>   inputs;
    bool                     confirmed = false;
    bool                     done      = false;
    Button btnOk     { {{460.f, 0.f}, {140.f, 36.f}}, "Confirma", C_BTN, C_BTN_HOV };
    Button btnCancel { {{620.f, 0.f}, {140.f, 36.f}}, "Anuleaza", C_PANEL, C_HOVER  };

    void show(const std::string& t, const std::vector<std::string>& lbls,
              const std::vector<std::string>& defaults = {}) {
        title = t; labels = lbls; confirmed = false; done = false; visible = true;
        inputs.clear();
        float startY = 330.f;
        for (int i = 0; i < static_cast<int>(lbls.size()); i++) {
            TextInput inp;
            inp.rect        = sf::FloatRect{ {460.f, startY + i * 50.f}, {360.f, 32.f} };
            inp.placeholder = lbls[i];
            inp.value       = (i < static_cast<int>(defaults.size())) ? defaults[i] : "";
            inp.focused     = (i == 0);
            inputs.push_back(inp);
        }
        // Position buttons below inputs
        float btnY = startY + labels.size() * 50.f + 10.f;
        btnOk.rect     = sf::FloatRect{ {460.f, btnY}, {140.f, 36.f} };
        btnCancel.rect = sf::FloatRect{ {620.f, btnY}, {140.f, 36.f} };
    }

    void handleEvent(const sf::Event& ev, sf::Vector2f clickPos, bool clicked) {
        if (!visible) return;
        for (auto& inp : inputs) {
            inp.handleEvent(ev);
            if (clicked) inp.focused = inp.contains(clickPos);
        }
        if (clicked) {
            if (btnOk.contains(clickPos))     { confirmed = true;  done = true; visible = false; }
            if (btnCancel.contains(clickPos)) { confirmed = false; done = true; visible = false; }
        }
    }

    std::vector<std::string> getValues() const {
        std::vector<std::string> v;
        for (const auto& inp : inputs) v.push_back(inp.value);
        return v;
    }

    void draw(sf::RenderWindow& w, const sf::Font& font, sf::Vector2f mouse) const {
        if (!visible) return;
        sf::RectangleShape overlay({W, H});
        overlay.setFillColor({0, 0, 0, 160});
        w.draw(overlay);

        float boxH = 80.f + labels.size() * 50.f + 60.f;
        sf::RectangleShape box({400.f, boxH});
        box.setPosition({440.f, 300.f});
        box.setFillColor(C_PANEL);
        box.setOutlineColor(C_INPUT_BD);
        box.setOutlineThickness(1.f);
        w.draw(box);

        sf::Text ttl(font, title, FN_LG);
        ttl.setFillColor(C_ACCENT);
        ttl.setStyle(sf::Text::Bold);
        ttl.setPosition({460.f, 310.f});
        w.draw(ttl);

        for (int i = 0; i < static_cast<int>(labels.size()); i++) {
            sf::Text lbl(font, labels[i], FN_SM);
            lbl.setFillColor(C_MUTED);
            lbl.setPosition({460.f, inputs[i].rect.position.y - 18.f});
            w.draw(lbl);
            inputs[i].draw(w, font);
        }
        btnOk.draw(w, font, mouse);
        btnCancel.draw(w, font, mouse);
    }
};

// ─── Helpers ──────────────────────────────────────────────────────────────────

static void drawText(sf::RenderWindow& w, const sf::Font& f, const std::string& s,
                     unsigned int sz, sf::Vector2f pos, sf::Color col = C_TEXT,
                     bool bold = false) {
    sf::Text t(f, s, sz);
    t.setFillColor(col);
    if (bold) t.setStyle(sf::Text::Bold);
    t.setPosition(pos);
    w.draw(t);
}

static void drawRect(sf::RenderWindow& w, sf::Vector2f pos, sf::Vector2f sz, sf::Color fill,
                     sf::Color outline = sf::Color::Transparent, float thickness = 0.f) {
    sf::RectangleShape s(sz);
    s.setPosition(pos);
    s.setFillColor(fill);
    if (thickness > 0.f) { s.setOutlineColor(outline); s.setOutlineThickness(thickness); }
    w.draw(s);
}

// ─── Main ─────────────────────────────────────────────────────────────────────

int main() {
    sf::RenderWindow window(
        sf::VideoMode({static_cast<unsigned int>(W), static_cast<unsigned int>(H)}),
        "Platforma Cursuri Online",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("assets/arial.ttf")) return 1;

    // ── State ────────────────────────────────────────────────────────────────

    CursService      cursService;
    InscriereService inscriereService;

    std::vector<Curs>      cursuri;
    std::vector<Inscriere> inscrieri;
    std::string            currentUser;
    std::string            statusMsg;
    sf::Color              statusColor = C_GREEN;

    auto reloadCursuri = [&]() {
        try { cursuri = Repository<Curs>(CURSURI_FILE).loadAll(); }
        catch (...) { cursuri.clear(); }
    };
    auto reloadInscrieri = [&]() {
        try { inscrieri = Repository<Inscriere>(INSCRIERI_FILE).loadAll(); }
        catch (...) { inscrieri.clear(); }
    };

    reloadCursuri();
    reloadInscrieri();

    int   activeTab      = 0;  // 0 = Cursuri, 1 = Inscrieri
    int   selCurs        = -1;
    int   selInscCurs    = -1;
    float leftScroll     = 0.f;
    float rightScroll    = 0.f;
    float leftScrollIns  = 0.f;
    float rightScrollIns = 0.f;

    auto setStatus = [&](const std::string& msg, bool ok = true) {
        statusMsg   = msg;
        statusColor = ok ? C_GREEN : C_RED;
    };

    // ── Top bar ──────────────────────────────────────────────────────────────
    TextInput userInput;
    userInput.rect        = sf::FloatRect{ {PAD, (TOPBAR_H - 32.f) / 2.f}, {280.f, 32.f} };
    userInput.placeholder = "ID utilizator...";

    Button btnLoad { {{300.f, (TOPBAR_H - 32.f) / 2.f}, {100.f, 32.f}}, "Incarca" };

    // ── Tab buttons ──────────────────────────────────────────────────────────
    Button tabCursuri   { {{0.f,   TOPBAR_H}, {160.f, TABBAR_H}}, "Cursuri"   };
    Button tabInscrieri { {{160.f, TOPBAR_H}, {160.f, TABBAR_H}}, "Inscrieri" };

    // ── Cursuri tab buttons ───────────────────────────────────────────────────
    float btnY = TOPBAR_H + TABBAR_H + PAD;
    Button btnAddCurs    { {{LEFT_W + PAD,         btnY},        {130.f, 30.f}}, "Adauga curs"   };
    Button btnDelCurs    { {{LEFT_W + PAD + 140.f, btnY},        {130.f, 30.f}}, "Sterge curs",  {150,50,50}, {180,60,60} };
    Button btnEditCurs   { {{LEFT_W + PAD + 280.f, btnY},        {130.f, 30.f}}, "Modifica curs" };
    Button btnAddLectie  { {{LEFT_W + PAD,         btnY + 40.f}, {130.f, 30.f}}, "Adauga lectie" };
    Button btnDelLectie  { {{LEFT_W + PAD + 140.f, btnY + 40.f}, {130.f, 30.f}}, "Sterge lectie", {150,50,50}, {180,60,60} };

    // ── Inscrieri tab buttons ─────────────────────────────────────────────────
    Button btnInscriere   { {{LEFT_W + PAD,         btnY}, {130.f, 30.f}}, "Inscrie-te"     };
    Button btnDezinscriere{ {{LEFT_W + PAD + 140.f, btnY}, {130.f, 30.f}}, "Dezinscrie-te", {150,50,50}, {180,60,60} };

    // ── Popups ───────────────────────────────────────────────────────────────
    ConfirmPopup confirmPopup;
    FormPopup    formPopup;
    std::string  pendingAction;
    int          selLectieIdx = -1;

    // ── Main loop ─────────────────────────────────────────────────────────────
    while (window.isOpen()) {
        sf::Vector2i mouseI = sf::Mouse::getPosition(window);
        sf::Vector2f mouse  = { static_cast<float>(mouseI.x), static_cast<float>(mouseI.y) };
        sf::Vector2f clickPos;

        while (const std::optional ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) window.close();

            // Text input for user field
            userInput.handleEvent(*ev);

            // Form popup text input
            if (formPopup.visible)
                formPopup.handleEvent(*ev, clickPos, false);

            if (const auto* k = ev->getIf<sf::Event::KeyPressed>()) {
                // Press Enter in user input to load
                if (k->code == sf::Keyboard::Key::Enter && userInput.focused) {
                    currentUser = userInput.value;
                    reloadInscrieri();
                    setStatus("Utilizator incarcat: " + currentUser);
                }
            }

            if (const auto* mb = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (mb->button == sf::Mouse::Button::Left) {
                    clickPos = { static_cast<float>(mb->position.x),
                                 static_cast<float>(mb->position.y) };

                    // Confirmation popup
                    if (confirmPopup.visible) {
                        confirmPopup.handleClick(clickPos);
                        // Handle confirmed action
                        if (confirmPopup.done && confirmPopup.result) {
                            try {
                                if (pendingAction == "del_curs" && selCurs >= 0) {
                                    cursService.stergereCurs(cursuri[selCurs].getId());
                                    reloadCursuri(); reloadInscrieri();
                                    selCurs = -1;
                                    setStatus("Cursul a fost sters.");
                                } else if (pendingAction == "del_lectie" && selCurs >= 0 && selLectieIdx >= 0) {
                                    auto& c = cursuri[selCurs];
                                    cursService.stergereLectie(c.getId(), c.getLectii()[selLectieIdx].getId());
                                    reloadCursuri();
                                    selLectieIdx = -1;
                                    setStatus("Lectia a fost stearsa.");
                                } else if (pendingAction == "dezinscriere" && selInscCurs >= 0) {
                                    inscriereService.dezinscriere(currentUser, cursuri[selInscCurs].getId());
                                    reloadInscrieri();
                                    selInscCurs = -1;
                                    setStatus("Dezinscriere efectuata.");
                                }
                            } catch (const std::exception& e) {
                                setStatus(e.what(), false);
                            }
                        }
                        continue;
                    }

                    // Form popup
                    if (formPopup.visible) {
                        formPopup.handleEvent(*ev, clickPos, true);
                        if (formPopup.done && formPopup.confirmed) {
                            auto vals = formPopup.getValues();
                            try {
                                if (pendingAction == "add_curs") {
                                    cursService.adaugareCurs(vals[0], vals[1], vals[2], vals[3]);
                                    reloadCursuri();
                                    setStatus("Curs adaugat.");
                                } else if (pendingAction == "edit_curs" && selCurs >= 0) {
                                    auto id = cursuri[selCurs].getId();
                                    if (!vals[0].empty()) cursService.modificareCurs("titlu",    id, vals[0]);
                                    if (!vals[1].empty()) cursService.modificareCurs("descriere", id, vals[1]);
                                    if (!vals[2].empty()) cursService.modificareCurs("pret",      id, vals[2]);
                                    reloadCursuri();
                                    setStatus("Curs modificat.");
                                } else if (pendingAction == "add_lectie" && selCurs >= 0) {
                                    cursService.adaugareLectie(cursuri[selCurs].getId(),
                                                               vals[0], vals[1], vals[2]);
                                    reloadCursuri();
                                    setStatus("Lectie adaugata.");
                                }
                            } catch (const std::exception& e) {
                                setStatus(e.what(), false);
                            }
                        }
                        continue;
                    }

                    // Focus user input
                    userInput.focused = userInput.contains(clickPos);

                    // Load button
                    if (btnLoad.contains(clickPos)) {
                        currentUser = userInput.value;
                        reloadInscrieri();
                        setStatus("Utilizator incarcat: " + currentUser);
                    }

                    // Tab switching
                    if (tabCursuri.contains(clickPos))   { activeTab = 0; selCurs = -1; leftScroll = rightScroll = 0.f; }
                    if (tabInscrieri.contains(clickPos))  { activeTab = 1; selInscCurs = -1; leftScrollIns = rightScrollIns = 0.f; }

                    float contentTop = TOPBAR_H + TABBAR_H;

                    if (activeTab == 0) {
                        // Course list click
                        if (clickPos.x < LEFT_W && clickPos.y > contentTop) {
                            float itemH = 62.f;
                            int idx = static_cast<int>((clickPos.y - contentTop + leftScroll) / itemH);
                            if (idx >= 0 && idx < static_cast<int>(cursuri.size())) {
                                selCurs      = (selCurs == idx) ? -1 : idx;
                                selLectieIdx = -1;
                                rightScroll  = 0.f;
                            }
                        }
                        // Lesson list click (in right panel)
                        if (selCurs >= 0 && clickPos.x > LEFT_W) {
                            float lessonTop = contentTop + 110.f;
                            float itemH     = 28.f;
                            int idx = static_cast<int>((clickPos.y - lessonTop + rightScroll) / itemH);
                            const auto& lecs = cursuri[selCurs].getLectii();
                            if (idx >= 0 && idx < static_cast<int>(lecs.size()))
                                selLectieIdx = (selLectieIdx == idx) ? -1 : idx;
                        }

                        // Cursuri tab buttons
                        if (btnAddCurs.contains(clickPos)) {
                            pendingAction = "add_curs";
                            formPopup.show("Adauga curs", {"ID curs", "Titlu", "Descriere", "Pret (RON)"});
                        }
                        if (btnDelCurs.contains(clickPos) && selCurs >= 0) {
                            pendingAction = "del_curs";
                            confirmPopup.show("Stergi cursul \"" + cursuri[selCurs].getTitlu() + "\"?");
                        }
                        if (btnEditCurs.contains(clickPos) && selCurs >= 0) {
                            pendingAction = "edit_curs";
                            const auto& c = cursuri[selCurs];
                            formPopup.show("Modifica curs",
                                {"Titlu nou", "Descriere noua", "Pret nou (RON)"},
                                {c.getTitlu(), c.getDescriere(), fmtPret(c.getPret())});
                        }
                        if (btnAddLectie.contains(clickPos) && selCurs >= 0) {
                            pendingAction = "add_lectie";
                            formPopup.show("Adauga lectie", {"ID lectie", "Titlu", "Durata (minute)"});
                        }
                        if (btnDelLectie.contains(clickPos) && selCurs >= 0 && selLectieIdx >= 0) {
                            pendingAction = "del_lectie";
                            const auto& l = cursuri[selCurs].getLectii()[selLectieIdx];
                            confirmPopup.show("Stergi lectia \"" + l.getTitlu() + "\"?");
                        }
                    }

                    if (activeTab == 1) {
                        // Course list click (inscrieri tab)
                        if (clickPos.x < LEFT_W && clickPos.y > contentTop) {
                            float itemH = 62.f;
                            int idx = static_cast<int>((clickPos.y - contentTop + leftScrollIns) / itemH);
                            if (idx >= 0 && idx < static_cast<int>(cursuri.size())) {
                                selInscCurs     = (selInscCurs == idx) ? -1 : idx;
                                rightScrollIns  = 0.f;
                            }
                        }

                        // Lesson checkbox click
                        if (selInscCurs >= 0 && clickPos.x > LEFT_W && !currentUser.empty()) {
                            const auto& c = cursuri[selInscCurs];
                            float lessonTop = contentTop + 110.f;
                            float itemH     = 28.f;
                            int idx = static_cast<int>((clickPos.y - lessonTop + rightScrollIns) / itemH);
                            const auto& lecs = c.getLectii();
                            if (idx >= 0 && idx < static_cast<int>(lecs.size())) {
                                // Find enrollment
                                auto it = std::ranges::find_if(inscrieri, [&](const Inscriere& i) {
                                    return i.getIdUser() == currentUser && i.getIdCurs() == c.getId();
                                });
                                if (it != inscrieri.end()) {
                                    try {
                                        if (it->esteLectieFinalizata(lecs[idx].getId()))
                                            inscriereService.nefinalizareLectie(currentUser, c.getId(), lecs[idx].getId());
                                        else
                                            inscriereService.finalizareLectie(currentUser, c.getId(), lecs[idx].getId());
                                        reloadInscrieri();
                                        setStatus("Lectie actualizata.");
                                    } catch (const std::exception& e) {
                                        setStatus(e.what(), false);
                                    }
                                }
                            }
                        }

                        // Inscrieri tab buttons
                        if (btnInscriere.contains(clickPos) && selInscCurs >= 0 && !currentUser.empty()) {
                            try {
                                inscriereService.inscriere(currentUser, cursuri[selInscCurs].getId());
                                reloadInscrieri();
                                setStatus("Inscriere efectuata.");
                            } catch (const std::exception& e) {
                                setStatus(e.what(), false);
                            }
                        }
                        if (btnDezinscriere.contains(clickPos) && selInscCurs >= 0 && !currentUser.empty()) {
                            pendingAction = "dezinscriere";
                            confirmPopup.show("Te dezinscrii din \"" + cursuri[selInscCurs].getTitlu() + "\"?");
                        }
                    }
                }
            }

            if (const auto* sc = ev->getIf<sf::Event::MouseWheelScrolled>()) {
                if (activeTab == 0) {
                    if (mouse.x < LEFT_W) leftScroll  = std::max(0.f, leftScroll  - sc->delta * 20.f);
                    else                  rightScroll = std::max(0.f, rightScroll - sc->delta * 20.f);
                } else {
                    if (mouse.x < LEFT_W) leftScrollIns  = std::max(0.f, leftScrollIns  - sc->delta * 20.f);
                    else                  rightScrollIns = std::max(0.f, rightScrollIns - sc->delta * 20.f);
                }
            }
        }

        // ── Draw ─────────────────────────────────────────────────────────────
        window.clear(C_BG);

        float contentTop = TOPBAR_H + TABBAR_H;

        // ── Top bar ──────────────────────────────────────────────────────────
        drawRect(window, {0.f, 0.f}, {W, TOPBAR_H}, C_PANEL);
        drawText(window, font, "Platforma Cursuri Online", FN_MD,
                 {PAD, (TOPBAR_H - 16.f) / 2.f}, C_ACCENT, true);

        userInput.draw(window, font);
        btnLoad.draw(window, font, mouse);

        // Status message (top right)
        if (!statusMsg.empty()) {
            sf::Text st(font, statusMsg, FN_SM);
            st.setFillColor(statusColor);
            st.setPosition({ W - st.getLocalBounds().size.x - PAD, (TOPBAR_H - 14.f) / 2.f });
            window.draw(st);
        }

        // ── Tab bar ──────────────────────────────────────────────────────────
        drawRect(window, {0.f, TOPBAR_H}, {W, TABBAR_H}, {33, 33, 46});
        for (int i = 0; i < 2; i++) {
            Button& tab = (i == 0) ? tabCursuri : tabInscrieri;
            sf::Color bg = (i == activeTab) ? C_SEL : (tab.contains(mouse) ? C_HOVER : sf::Color::Transparent);
            drawRect(window, tab.rect.position, tab.rect.size, bg);
            drawText(window, font, tab.label, FN_MD,
                     {tab.rect.position.x + 20.f, tab.rect.position.y + 10.f},
                     i == activeTab ? sf::Color::White : C_MUTED);
        }
        // Tab underline
        drawRect(window, {static_cast<float>(activeTab) * 160.f, TOPBAR_H + TABBAR_H - 2.f}, {160.f, 2.f}, C_ACCENT);

        // ── Left panel ───────────────────────────────────────────────────────
        drawRect(window, {0.f, contentTop}, {LEFT_W, H - contentTop}, C_PANEL);

        float& lScroll   = (activeTab == 0) ? leftScroll : leftScrollIns;
        int&   selList   = (activeTab == 0) ? selCurs    : selInscCurs;

        // Set clip view for left panel list
        sf::View lv(sf::FloatRect({0.f, contentTop}, {LEFT_W, H - contentTop}));
        lv.setViewport(sf::FloatRect({0.f, contentTop / H}, {LEFT_W / W, (H - contentTop) / H}));
        window.setView(lv);

        float itemH = 62.f;
        for (int i = 0; i < static_cast<int>(cursuri.size()); i++) {
            float yPos = contentTop + i * itemH - lScroll;
            sf::Color bg = (i == selList) ? C_SEL : (sf::FloatRect{{0.f, yPos}, {LEFT_W, itemH}}.contains(mouse) ? C_HOVER : sf::Color::Transparent);
            drawRect(window, {0.f, yPos}, {LEFT_W, itemH - 1.f}, bg);

            drawText(window, font, cursuri[i].getTitlu(), FN_MD,
                     {PAD, yPos + 8.f}, i == selList ? sf::Color::White : C_TEXT, i == selList);
            std::string meta = std::to_string(cursuri[i].getLectii().size()) + " lectii  |  "
                             + fmtPret(cursuri[i].getPret()) + " RON";
            drawText(window, font, meta, FN_SM, {PAD, yPos + 32.f}, C_MUTED);

            // Enrollment indicator in Inscrieri tab
            if (activeTab == 1 && !currentUser.empty()) {
                bool enrolled = std::ranges::any_of(inscrieri, [&](const Inscriere& ins) {
                    return ins.getIdUser() == currentUser && ins.getIdCurs() == cursuri[i].getId();
                });
                if (enrolled) {
                    drawText(window, font, "✓", FN_MD, {LEFT_W - 24.f, yPos + 18.f}, C_GREEN);
                }
            }
        }
        window.setView(window.getDefaultView());

        // Divider
        drawRect(window, {LEFT_W, contentTop}, {1.f, H - contentTop}, C_MUTED);

        // ── Right panel ───────────────────────────────────────────────────────
        float rightX = LEFT_W + PAD + 1.f;
        float rightW = W - LEFT_W - PAD * 2.f;

        if (activeTab == 0) {
            // ── Cursuri tab buttons ───────────────────────────────────────────
            btnAddCurs.draw(window, font, mouse);
            btnDelCurs.draw(window, font, mouse);
            btnEditCurs.draw(window, font, mouse);
            if (selCurs >= 0) {
                btnAddLectie.draw(window, font, mouse);
                btnDelLectie.draw(window, font, mouse);
            }

            if (selCurs < 0) {
                drawText(window, font, "Selecteaza un curs din lista.", FN_MD,
                         {rightX, H / 2.f}, C_MUTED);
            } else {
                const Curs& c = cursuri[selCurs];

                sf::View rv(sf::FloatRect({LEFT_W + 1.f, contentTop + 90.f},
                                          {W - LEFT_W - 1.f, H - contentTop - 90.f}));
                rv.setViewport(sf::FloatRect(
                    {(LEFT_W + 1.f) / W, (contentTop + 90.f) / H},
                    {(W - LEFT_W - 1.f) / W, (H - contentTop - 90.f) / H}
                ));
                window.setView(rv);

                float y = contentTop + 90.f - rightScroll;

                drawText(window, font, c.getTitlu(), FN_LG, {rightX, y}, C_ACCENT, true);
                y += 28.f;
                drawText(window, font, "Pret: " + fmtPret(c.getPret()) + " RON", FN_SM, {rightX, y}, C_MUTED);
                y += 22.f;

                std::string wrapped = wrapText(c.getDescriere(), font, FN_SM, rightW - PAD);
                drawText(window, font, wrapped, FN_SM, {rightX, y}, C_TEXT);
                sf::Text tmp(font, wrapped, FN_SM);
                y += tmp.getLocalBounds().size.y + 16.f;

                drawRect(window, {rightX, y}, {rightW, 1.f}, C_MUTED);
                y += 12.f;

                drawText(window, font, "Lectii (" + std::to_string(c.getLectii().size()) + ")",
                         FN_MD, {rightX, y}, C_ACCENT, true);
                y += 26.f;

                for (int i = 0; i < static_cast<int>(c.getLectii().size()); i++) {
                    const auto& l = c.getLectii()[i];
                    sf::Color bg = (i == selLectieIdx) ? C_SEL : sf::Color::Transparent;
                    drawRect(window, {rightX - 4.f, y - 2.f}, {rightW, 26.f}, bg);
                    std::string lt = l.getId() + "  —  " + l.getTitlu()
                                   + "  (" + std::to_string(l.getDurata()) + " min)";
                    drawText(window, font, lt, FN_SM, {rightX, y},
                             i == selLectieIdx ? sf::Color::White : C_TEXT);
                    y += 28.f;
                }

                window.setView(window.getDefaultView());
            }
        }

        if (activeTab == 1) {
            // ── Inscrieri tab buttons ─────────────────────────────────────────
            btnInscriere.draw(window, font, mouse);
            btnDezinscriere.draw(window, font, mouse);

            if (currentUser.empty()) {
                drawText(window, font, "Introdu un ID utilizator si apasa Incarca.", FN_MD,
                         {rightX, H / 2.f}, C_MUTED);
            } else if (selInscCurs < 0) {
                drawText(window, font, "Selecteaza un curs din lista.", FN_MD,
                         {rightX, H / 2.f}, C_MUTED);
            } else {
                const Curs& c = cursuri[selInscCurs];

                // Find enrollment for this user+course
                auto insIt = std::ranges::find_if(inscrieri, [&](const Inscriere& i) {
                    return i.getIdUser() == currentUser && i.getIdCurs() == c.getId();
                });
                bool enrolled = insIt != inscrieri.end();

                sf::View rv(sf::FloatRect({LEFT_W + 1.f, contentTop + 90.f},
                                          {W - LEFT_W - 1.f, H - contentTop - 90.f}));
                rv.setViewport(sf::FloatRect(
                    {(LEFT_W + 1.f) / W, (contentTop + 90.f) / H},
                    {(W - LEFT_W - 1.f) / W, (H - contentTop - 90.f) / H}
                ));
                window.setView(rv);

                float y = contentTop + 90.f - rightScrollIns;

                drawText(window, font, c.getTitlu(), FN_LG, {rightX, y}, C_ACCENT, true);
                y += 28.f;

                if (!enrolled) {
                    drawText(window, font, "Nu esti inscris la acest curs.", FN_MD,
                             {rightX, y + 20.f}, C_MUTED);
                } else {
                    size_t total      = c.getLectii().size();
                    size_t finalizate = insIt->getLectiiFinalizate().size();

                    // Progress bar
                    drawText(window, font, "Progres: " + std::to_string(finalizate)
                             + " / " + std::to_string(total) + " lectii", FN_SM, {rightX, y}, C_MUTED);
                    y += 20.f;

                    float barW = rightW - PAD;
                    drawRect(window, {rightX, y}, {barW, 10.f}, C_INPUT_BD);
                    if (total > 0) {
                        float fill = barW * static_cast<float>(finalizate) / static_cast<float>(total);
                        drawRect(window, {rightX, y}, {fill, 10.f}, C_GREEN);
                    }
                    y += 22.f;

                    drawRect(window, {rightX, y}, {rightW, 1.f}, C_MUTED);
                    y += 12.f;

                    drawText(window, font, "Lectii (click pentru a marca/demarca):",
                             FN_SM, {rightX, y}, C_MUTED);
                    y += 24.f;

                    for (int i = 0; i < static_cast<int>(c.getLectii().size()); i++) {
                        const auto& l = c.getLectii()[i];
                        bool done = insIt->esteLectieFinalizata(l.getId());

                        // Checkbox
                        drawRect(window, {rightX, y + 2.f}, {14.f, 14.f}, C_INPUT_BG, C_INPUT_BD, 1.f);
                        if (done) drawText(window, font, "✓", FN_SM, {rightX + 1.f, y - 1.f}, C_GREEN);

                        std::string lt = l.getTitlu() + "  (" + std::to_string(l.getDurata()) + " min)";
                        drawText(window, font, lt, FN_SM, {rightX + 22.f, y},
                                 done ? C_GREEN : C_TEXT);
                        y += 28.f;
                    }
                }

                window.setView(window.getDefaultView());
            }
        }

        // ── Popups (drawn last, on top) ───────────────────────────────────────
        confirmPopup.draw(window, font, mouse);
        formPopup.draw(window, font, mouse);

        window.display();
    }

    return 0;
}
