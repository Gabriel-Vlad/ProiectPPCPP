#pragma once
#include "imgui.h"
#include "state.h"
#include "theme.h"

// The generic input-form popup, reused by every "add"/"edit" action.
// Which fields it shows and what happens on confirm are driven entirely by
// AppState (formLabels, formBuf, pendingAction) — the popup itself is dumb.
inline void drawFormPopup(AppState& st, float W, float H) {
    // ImGui quirk: OpenPopup must be called in the same frame the popup is
    // drawn, so the tabs only set st.showForm and this function converts the
    // flag into the actual OpenPopup call, then clears it.
    if (st.showForm) {
        ImGui::OpenPopup("##form");
        st.showForm = false;
    }

    // Center the popup; pivot {0.5, 0.5} means "position refers to my center".
    ImGui::SetNextWindowPos({W / 2.f, H / 2.f}, ImGuiCond_Always, {0.5f, 0.5f});
    ImGui::SetNextWindowSize({500.f, 0.f});  // height 0 = auto-fit to content
    if (!ImGui::BeginPopupModal("##form", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
        return;  // popup is not open this frame — nothing to draw

    ImGui::PushStyleColor(ImGuiCol_Text, C_ACCENT);
    ImGui::Text("%s", st.formTitle.c_str());
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();

    // One label + text input per form field. Each input gets a unique
    // invisible ID ("##fi0", "##fi1"...) and writes into its own char buffer.
    for (int i = 0; i < static_cast<int>(st.formLabels.size()); i++) {
        ImGui::Text("%s", st.formLabels[i].c_str());
        ImGui::SetNextItemWidth(-1.f);  // stretch to the full popup width
        std::string id = "##fi" + std::to_string(i);
        ImGui::InputText(id.c_str(), st.formBuf[i], 256);
        ImGui::Spacing();
    }

    ImGui::Spacing();
    if (ImGui::Button("Confirma", {140.f, 0.f})) {
        // pendingAction tells us which service call this form belongs to.
        // Errors are caught and surfaced through the status bar instead of
        // crashing the GUI.
        try {
            if (st.pendingAction == "add_user") {
                st.utilizatorService.inregistrareUtilizator(st.formBuf[0]);
                st.reloadUtilizatori(); st.setStatus("Utilizator inregistrat.");
            } else if (st.pendingAction == "add_curs") {
                st.cursService.adaugareCurs(st.formBuf[0], st.formBuf[1], st.formBuf[2]);
                st.reloadCursuri(); st.setStatus("Curs adaugat.");
            } else if (st.pendingAction == "edit_curs" && st.selCurs >= 0) {
                // Only fields the user actually filled in are updated —
                // an empty buffer means "leave this field unchanged".
                auto id = st.cursuri[st.selCurs].getId();
                if (st.formBuf[0][0]) st.cursService.modificareCurs("titlu",     id, st.formBuf[0]);
                if (st.formBuf[1][0]) st.cursService.modificareCurs("descriere", id, st.formBuf[1]);
                if (st.formBuf[2][0]) st.cursService.modificareCurs("pret",      id, st.formBuf[2]);
                st.reloadCursuri(); st.setStatus("Curs modificat.");
            } else if (st.pendingAction == "add_lectie" && st.selCurs >= 0) {
                st.cursService.adaugareLectie(st.cursuri[st.selCurs].getId(),
                                              st.formBuf[0], st.formBuf[1]);
                st.reloadCursuri(); st.setStatus("Lectie adaugata.");
            }
        } catch (const std::exception& e) { st.setStatus(e.what(), false); }
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Anuleaza", {140.f, 0.f}))
        ImGui::CloseCurrentPopup();

    ImGui::EndPopup();
}

// The yes/no confirmation popup shown before every destructive action.
// confirmAction decides what runs when the user clicks "Da".
inline void drawConfirmPopup(AppState& st, float W, float H) {
    if (st.showConfirm) {
        ImGui::OpenPopup("##confirm");
        st.showConfirm = false;
    }

    ImGui::SetNextWindowPos({W / 2.f, H / 2.f}, ImGuiCond_Always, {0.5f, 0.5f});
    ImGui::SetNextWindowSize({420.f, 0.f});
    if (!ImGui::BeginPopupModal("##confirm", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
        return;

    ImGui::Spacing();
    // Center the question horizontally inside the fixed 420px popup.
    ImGui::SetCursorPosX((420.f - ImGui::CalcTextSize(st.confirmMsg.c_str()).x) / 2.f);
    ImGui::Text("%s", st.confirmMsg.c_str());
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::SetCursorPosX((420.f - 300.f) / 2.f);
    if (RedButton("  Da  ")) {
        try {
            if (st.confirmAction == "del_user" && !st.currentUser.empty()) {
                st.utilizatorService.stergereUtilizator(st.currentUser);
                // The deleted user can no longer be the active one, and the
                // remaining IDs may have shifted — reload both lists.
                st.currentUser.clear();
                st.reloadUtilizatori(); st.reloadInscrieri();
                st.setStatus("Utilizator sters.");
            } else if (st.confirmAction == "del_curs" && st.selCurs >= 0) {
                st.cursService.stergereCurs(st.cursuri[st.selCurs].getId());
                // Enrollments are cascade-deleted by the service, so the
                // enrollment list must be reloaded too.
                st.reloadCursuri(); st.reloadInscrieri();
                st.selCurs = -1; st.setStatus("Cursul a fost sters.");
            } else if (st.confirmAction == "del_lectie" && st.selCurs >= 0 && st.selLectie >= 0) {
                auto& c = st.cursuri[st.selCurs];
                st.cursService.stergereLectie(c.getId(), c.getLectii()[st.selLectie].getId());
                st.reloadCursuri(); st.selLectie = -1; st.setStatus("Lectia a fost stearsa.");
            } else if (st.confirmAction == "dezinscriere" && st.selInscCurs >= 0) {
                st.inscriereService.dezinscriere(st.currentUser, st.cursuri[st.selInscCurs].getId());
                st.reloadInscrieri(); st.selInscCurs = -1; st.setStatus("Dezinscriere efectuata.");
            }
        } catch (const std::exception& e) { st.setStatus(e.what(), false); }
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("  Nu  "))
        ImGui::CloseCurrentPopup();

    ImGui::EndPopup();
}
