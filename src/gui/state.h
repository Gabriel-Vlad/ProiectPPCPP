#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "../../core/models/Curs.h"
#include "../../core/models/Inscriere.h"
#include "../../core/services/CursService.h"
#include "../../core/services/InscriereService.h"
#include "../../core/services/UtilizatorService.h"

// The entire state of the GUI in one place. A single AppState instance is
// created in main() and passed by reference to every draw function — ImGui is
// "immediate mode" (the whole UI is rebuilt every frame), so anything that
// must survive between frames has to live here.
struct AppState {
    // The same services the CLI apps use — the GUI is just another client.
    CursService       cursService;
    InscriereService  inscriereService;
    UtilizatorService utilizatorService;

    // In-memory copies of the data files, refreshed via the reload* methods
    // after every mutation. The draw functions read only these vectors.
    std::vector<Curs>       cursuri;
    std::vector<Inscriere>  inscrieri;
    std::vector<Utilizator> utilizatori;

    std::string currentUser;   // id of the user selected in the Inscrieri tab ("" = none)

    // Status message shown in the top bar; statusTime records when it was set
    // so the render loop can fade it out and clear it after a few seconds.
    std::string statusMsg;
    bool        statusOk = true;   // true = green (success), false = red (error)
    std::chrono::steady_clock::time_point statusTime {};

    // Selection state: indexes into the vectors above, -1 = nothing selected.
    int selCurs     = -1;  // selected course in the Cursuri tab
    int selInscCurs = -1;  // selected course in the Inscrieri tab
    int selLectie   = -1;  // selected lesson inside the selected course

    // Popup state. ImGui popups must be opened in the same frame they are
    // requested, so the draw code sets showForm/showConfirm and the popup
    // functions consume the flag on the next drawFormPopup/drawConfirmPopup call.
    bool        showForm      = false;
    bool        showConfirm   = false;
    std::string formTitle;       // heading shown at the top of the form popup
    std::string pendingAction;   // which action the form confirms ("add_curs", ...)
    std::string confirmMsg;      // question shown in the confirm popup
    std::string confirmAction;   // which action the confirm executes ("del_curs", ...)
    std::vector<std::string> formLabels;  // one label per input field
    char formBuf[4][256]  = {};  // raw text buffers for up to 4 form inputs
                                 // (ImGui::InputText writes into char arrays)

    // Re-read the data files into the in-memory vectors. The catch blocks
    // keep the GUI alive even if a file is temporarily unreadable.
    void reloadCursuri() {
        try { cursuri = cursService.getCursuri(); }
        catch (...) { cursuri.clear(); }
    }

    void reloadInscrieri() {
        try { inscrieri = inscriereService.getInscrieri(); }
        catch (...) { inscrieri.clear(); }
    }

    void reloadUtilizatori() {
        try { utilizatori = utilizatorService.getUtilizatori(); }
        catch (...) { utilizatori.clear(); }
    }

    // Shows a message in the top bar and starts its fade-out timer.
    void setStatus(const std::string& msg, bool ok = true) {
        statusMsg  = msg;
        statusOk   = ok;
        statusTime = std::chrono::steady_clock::now();
    }

    // Prepares and opens the form popup: sets the title, the action to run on
    // confirm, the field labels, and optionally pre-fills the inputs
    // (used by "Modifica curs" to show the current values).
    void openForm(const std::string& title, const std::string& action,
                  const std::vector<std::string>& labels,
                  const std::vector<std::string>& defaults = {}) {
        formTitle     = title;
        pendingAction = action;
        formLabels    = labels;
        showForm      = true;
        for (int i = 0; i < 4; i++) formBuf[i][0] = '\0';
        for (int i = 0; i < static_cast<int>(defaults.size()) && i < 4; i++)
            snprintf(formBuf[i], 256, "%s", defaults[i].c_str());
    }

    // Prepares and opens the yes/no confirmation popup.
    void openConfirm(const std::string& msg, const std::string& action) {
        confirmMsg    = msg;
        confirmAction = action;
        showConfirm   = true;
    }
};
