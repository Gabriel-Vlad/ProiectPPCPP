#pragma once
#include "imgui.h"
#include "state.h"
#include "theme.h"
#include "utils.h"

// The "Cursuri" tab: course list on the left, details and action buttons on
// the right. Called once per frame; W/H are the current window dimensions so
// the layout follows window resizing.
inline void drawTabCursuri(AppState& st, float W, float H, float TOPBAR_H, float LEFT_W, float PAD) {
    const float contentH = H - TOPBAR_H - ImGui::GetFrameHeight() - 8.f;  // space below top bar + tab bar
    const float ITEM_H   = 62.f;                                          // height of one course row

    // ── Left panel — course list ──────────────────────────────────────────────
    ImGui::PushStyleColor(ImGuiCol_ChildBg, C_PANEL);
    ImGui::BeginChild("##curs_list", {LEFT_W, contentH}, ImGuiChildFlags_None);

    // Panel header with the course count.
    ImGui::SetCursorPos({PAD, PAD});
    ImGui::PushStyleColor(ImGuiCol_Text, C_MUTED);
    ImGui::Text("CURSURI (%zu)", st.cursuri.size());
    ImGui::PopStyleColor();
    ImGui::Separator();

    // Centered placeholder when there is nothing to list.
    if (st.cursuri.empty()) {
        ImGui::SetCursorPosY(contentH / 2.f - 10.f);
        ImGui::PushStyleColor(ImGuiCol_Text, C_MUTED);
        ImGui::SetCursorPosX((LEFT_W - ImGui::CalcTextSize("Niciun curs adaugat.").x) / 2.f);
        ImGui::Text("Niciun curs adaugat.");
        ImGui::PopStyleColor();
    }

    for (int i = 0; i < static_cast<int>(st.cursuri.size()); i++) {
        bool sel = (i == st.selCurs);

        // PushID makes the row's widget IDs unique — without it, every row's
        // "##citem" Selectable would be the same widget for ImGui.
        ImGui::PushID(i);
        if (drawCursItem(st.cursuri[i], sel, false, LEFT_W, ITEM_H, PAD)) {
            // Clicking a selected row deselects it; the lesson selection is
            // reset because it belonged to the previously selected course.
            st.selCurs   = sel ? -1 : i;
            st.selLectie = -1;
        }
        ImGui::PopID();
        ImGui::Separator();
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::SameLine();

    // Thin vertical line separating the two panels.
    ImVec2 divMin = ImGui::GetItemRectMin();
    ImGui::GetWindowDrawList()->AddRectFilled(
        {divMin.x + LEFT_W, divMin.y},
        {divMin.x + LEFT_W + 1.f, divMin.y + contentH},
        IM_COL32(80, 80, 110, 255));

    // ── Right panel — details + buttons ──────────────────────────────────────
    ImGui::PushStyleColor(ImGuiCol_ChildBg, C_BG);
    ImGui::BeginChild("##curs_detail", {W - LEFT_W - 2.f, contentH}, ImGuiChildFlags_None);

    // ── Action buttons ────────────────────────────────────────────────────────
    ImGui::Spacing();
    ImGui::SetCursorPosX(PAD);

    if (ImGui::Button("  + Adauga curs  "))
        st.openForm("Adauga curs", "add_curs", {"Titlu", "Descriere", "Pret (RON)"});

    // BeginDisabled grays out and blocks everything until EndDisabled —
    // course buttons make no sense while no course is selected.
    ImGui::SameLine();
    ImGui::BeginDisabled(st.selCurs < 0);
    if (RedButton("  - Sterge curs  ") && st.selCurs >= 0)
        st.openConfirm("Stergi cursul \"" + st.cursuri[st.selCurs].getTitlu() + "\"?", "del_curs");
    ImGui::SameLine();
    if (ImGui::Button("  Modifica curs  ") && st.selCurs >= 0) {
        // Pre-fill the form with the current values so the user edits
        // instead of retyping everything.
        const auto& c = st.cursuri[st.selCurs];
        st.openForm("Modifica curs", "edit_curs",
                    {"Titlu nou", "Descriere noua", "Pret nou (RON)"},
                    {c.getTitlu(), c.getDescriere(), fmtPret(c.getPret())});
    }
    ImGui::EndDisabled();

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, C_INPUT_BD);
    ImGui::Text("|");
    ImGui::PopStyleColor();
    ImGui::SameLine();

    // Lesson buttons: adding needs a course; deleting also needs a lesson.
    ImGui::BeginDisabled(st.selCurs < 0);
    if (ImGui::Button("  + Adauga lectie  ") && st.selCurs >= 0)
        st.openForm("Adauga lectie", "add_lectie", {"Titlu", "Durata (minute)"});
    ImGui::SameLine();
    ImGui::BeginDisabled(st.selLectie < 0);
    if (RedButton("  - Sterge lectie  ") && st.selCurs >= 0 && st.selLectie >= 0) {
        const auto& l = st.cursuri[st.selCurs].getLectii()[st.selLectie];
        st.openConfirm("Stergi lectia \"" + l.getTitlu() + "\"?", "del_lectie");
    }
    ImGui::EndDisabled();
    ImGui::EndDisabled();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ── Course details ────────────────────────────────────────────────────────
    if (st.selCurs < 0) {
        // Nothing selected — show a centered hint instead of empty space.
        float midY = contentH / 2.f;
        ImGui::SetCursorPosY(midY - 10.f);
        ImGui::PushStyleColor(ImGuiCol_Text, C_MUTED);
        ImGui::SetCursorPosX(PAD);
        ImGui::Text("Selecteaza un curs din lista.");
        ImGui::PopStyleColor();
    } else {
        const Curs& c = st.cursuri[st.selCurs];

        // Title, enlarged via the window font scale (then reset to 1).
        ImGui::SetCursorPosX(PAD);
        ImGui::PushStyleColor(ImGuiCol_Text, C_ACCENT);
        ImGui::SetWindowFontScale(1.2f);
        ImGui::Text("%s", c.getTitlu().c_str());
        ImGui::SetWindowFontScale(1.f);
        ImGui::PopStyleColor();

        ImGui::SetCursorPosX(PAD);
        ImGui::PushStyleColor(ImGuiCol_Text, C_GREEN);
        ImGui::Text("%s RON", fmtPret(c.getPret()).c_str());
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::SetCursorPosX(PAD);
        ImGui::PushStyleColor(ImGuiCol_Text, C_TEXT);
        ImGui::TextWrapped("%s", c.getDescriere().c_str());  // wraps long descriptions
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::SetCursorPosX(PAD);
        ImGui::PushStyleColor(ImGuiCol_Text, C_ACCENT);
        ImGui::Text("Lectii (%zu)", c.getLectii().size());
        ImGui::PopStyleColor();
        ImGui::Spacing();

        if (c.getLectii().empty()) {
            ImGui::SetCursorPosX(PAD);
            ImGui::PushStyleColor(ImGuiCol_Text, C_MUTED);
            ImGui::Text("Nicio lectie adaugata.");
            ImGui::PopStyleColor();
        }

        // Lesson list: each row is a selectable "id  title  (duration)" line.
        // The +1000 ID offset keeps lesson IDs from colliding with course rows.
        for (int i = 0; i < static_cast<int>(c.getLectii().size()); i++) {
            const auto& l = c.getLectii()[i];
            bool lsel = (i == st.selLectie);
            std::string lt = "  " + l.getId() + "   " + l.getTitlu()
                           + "   (" + std::to_string(l.getDurata()) + " min)";
            ImGui::PushID(i + 1000);
            ImGui::PushStyleColor(ImGuiCol_Header,        C_SEL);
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, C_HOVER);
            ImGui::PushStyleColor(ImGuiCol_Text, lsel ? C_TEXT : C_MUTED);
            if (ImGui::Selectable(lt.c_str(), lsel, ImGuiSelectableFlags_None, {0, 26.f}))
                st.selLectie = lsel ? -1 : i;  // click again to deselect
            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();
}
