#pragma once
#include <algorithm>
#include "imgui.h"
#include "state.h"
#include "theme.h"
#include "utils.h"

// The "Inscrieri" tab: course list on the left, user selection and enrollment
// details on the right. The right side is laid out as a two-step flow —
// 1. pick a user, 2. pick a course — and only then shows the actions.
inline void drawTabInscrieri(AppState& st, float W, float H, float TOPBAR_H, float LEFT_W, float PAD) {
    const float contentH = H - TOPBAR_H - ImGui::GetFrameHeight() - 8.f;
    const float ITEM_H   = 62.f;

    // Left panel
    ImGui::PushStyleColor(ImGuiCol_ChildBg, C_PANEL);
    ImGui::BeginChild("##ins_list", {LEFT_W, contentH}, ImGuiChildFlags_None);

    ImGui::SetCursorPos({PAD, PAD});
    ImGui::PushStyleColor(ImGuiCol_Text, C_MUTED);
    ImGui::Text("CURSURI (%zu)", st.cursuri.size());
    ImGui::PopStyleColor();
    ImGui::Separator();

    if (st.cursuri.empty()) {
        ImGui::SetCursorPosY(contentH / 2.f - 10.f);
        ImGui::PushStyleColor(ImGuiCol_Text, C_MUTED);
        ImGui::SetCursorPosX((LEFT_W - ImGui::CalcTextSize("Niciun curs disponibil.").x) / 2.f);
        ImGui::Text("Niciun curs disponibil.");
        ImGui::PopStyleColor();
    }

    for (int i = 0; i < static_cast<int>(st.cursuri.size()); i++) {
        const auto& c = st.cursuri[i];
        bool sel = (i == st.selInscCurs);

        // Is the currently selected user enrolled in this course? Drives the
        // green tint and the "inscris" badge on the row.
        bool enrolled = !st.currentUser.empty() && std::ranges::any_of(st.inscrieri,
            [&](const Inscriere& ins) {
                return ins.getIdUser() == st.currentUser && ins.getIdCurs() == c.getId();
            });

        // The +2000 ID offset keeps these rows from colliding with the
        // Cursuri tab's rows (offset 0) and the lesson checkboxes (offset 3000).
        ImGui::PushID(i + 2000);
        if (drawCursItem(c, sel, enrolled, LEFT_W, ITEM_H, PAD))
            st.selInscCurs = sel ? -1 : i;  // click again to deselect
        ImGui::PopID();
        ImGui::Separator();
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::SameLine();

    // Right panel
    ImGui::PushStyleColor(ImGuiCol_ChildBg, C_BG);
    ImGui::BeginChild("##ins_detail", {W - LEFT_W - 2.f, contentH}, ImGuiChildFlags_None);

    // Step 1: Select user
    // The step number lights up in accent color while the step is pending.
    ImGui::Spacing();
    ImGui::SetCursorPosX(PAD);
    ImGui::PushStyleColor(ImGuiCol_Text, st.currentUser.empty() ? C_ACCENT : C_MUTED);
    ImGui::Text("1.");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, C_MUTED);
    ImGui::Text("Selecteaza utilizatorul:");
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::SetCursorPosX(PAD + 20.f);

    // The active user is resolved once and reused below — both for the
    // delete-confirmation message and for the "Utilizator activ" label.
    auto activeIt = findById(st.utilizatori, st.currentUser);

    if (ImGui::Button("  + Utilizator  "))
        st.openForm("Inregistreaza utilizator", "add_user", {"Nume"});
    ImGui::SameLine();
    ImGui::BeginDisabled(st.currentUser.empty());
    if (RedButton("  - Sterge utilizator  ") && activeIt != st.utilizatori.end())
        st.openConfirm("Stergi utilizatorul \"" + activeIt->getNume() + "\"?", "del_user");
    ImGui::EndDisabled();
    ImGui::Spacing();

    if (st.utilizatori.empty()) {
        ImGui::SetCursorPosX(PAD + 20.f);
        ImGui::PushStyleColor(ImGuiCol_Text, C_MUTED);
        ImGui::Text("Niciun utilizator inregistrat.");
        ImGui::PopStyleColor();
    } else {
        // One button per user, flowing left to right and wrapping onto a new
        // line before reaching the right edge. ImGui has no automatic button
        // wrapping, so the position is tracked by hand: curX accumulates the
        // widths, and when the next button would overflow availW the cursor
        // is reset to the left margin on a new line.
        float availW = ImGui::GetContentRegionAvail().x - 24.f;  // right margin
        float curX   = PAD + 20.f;
        ImGui::SetCursorPosX(curX);
        for (const auto& u : st.utilizatori) {
            std::string label  = u.getId() + "  " + u.getNume();
            float       btnW   = ImGui::CalcTextSize(label.c_str()).x + 16.f;
            bool        active = (st.currentUser == u.getId());

            if (curX + btnW > availW) {
                ImGui::NewLine();
                ImGui::SetCursorPosX(PAD + 20.f);
                curX = PAD + 20.f;
            }

            // The selected user's button stays highlighted in blue.
            if (active) ImGui::PushStyleColor(ImGuiCol_Button, C_SEL);
            else        ImGui::PushStyleColor(ImGuiCol_Button, C_PANEL);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C_HOVER);
            if (ImGui::Button(label.c_str())) {
                st.currentUser = u.getId();
                st.reloadInscrieri();
            }
            ImGui::PopStyleColor(2);
            curX += btnW + ImGui::GetStyle().ItemSpacing.x;
            ImGui::SameLine();
        }
        ImGui::NewLine();
        if (activeIt != st.utilizatori.end()) {
            ImGui::SetCursorPosX(PAD + 20.f);
            ImGui::PushStyleColor(ImGuiCol_Text, C_GREEN);
            ImGui::Text("Utilizator activ: %s", activeIt->getNume().c_str());
            ImGui::PopStyleColor();
        }
    }

    // Step 2: Select course hint
    ImGui::Spacing();
    ImGui::SetCursorPosX(PAD);
    ImGui::PushStyleColor(ImGuiCol_Text,
        (!st.currentUser.empty() && st.selInscCurs < 0) ? C_ACCENT : C_MUTED);
    ImGui::Text("2.");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, C_MUTED);
    ImGui::Text("Selecteaza un curs din lista din stanga.");
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Content area
    if (st.currentUser.empty() || st.selInscCurs < 0) {
        // Nothing to show yet — the step hints above tell the user what to do.
    } else {
        const Curs& c = st.cursuri[st.selInscCurs];

        // Look up this user's enrollment in the selected course (if any);
        // it drives everything below: the badge, the button, the progress.
        auto insIt = std::ranges::find_if(st.inscrieri, [&](const Inscriere& i) {
            return i.getIdUser() == st.currentUser && i.getIdCurs() == c.getId();
        });
        bool enrolled = insIt != st.inscrieri.end();

        // Course title + enrollment badge.
        ImGui::SetCursorPosX(PAD);
        ImGui::PushStyleColor(ImGuiCol_Text, C_ACCENT);
        ImGui::SetWindowFontScale(1.2f);
        ImGui::Text("%s", c.getTitlu().c_str());
        ImGui::SetWindowFontScale(1.f);
        ImGui::PopStyleColor();

        ImGui::SameLine();
        if (enrolled) {
            ImGui::PushStyleColor(ImGuiCol_Text, C_GREEN);
            ImGui::Text("  [Inscris]");
            ImGui::PopStyleColor();
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, C_MUTED);
            ImGui::Text("  [Neinscris]");
            ImGui::PopStyleColor();
        }

        ImGui::Spacing();

        // One context-aware action: enroll when not enrolled, unenroll (with
        // confirmation) when enrolled — never both at once.
        ImGui::SetCursorPosX(PAD);
        if (!enrolled) {
            if (ImGui::Button("  Inscrie-te la acest curs  ")) {
                try {
                    st.inscriereService.inscriere(st.currentUser, c.getId());
                    st.reloadInscrieri();
                    st.setStatus("Inscris la \"" + c.getTitlu() + "\".");
                } catch (const std::exception& e) { st.setStatus(e.what(), false); }
            }
        } else {
            if (RedButton("  Dezinscrie-te din acest curs  "))
                st.openConfirm("Te dezinscrii din \"" + c.getTitlu() + "\"?", "dezinscriere");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Progress + lessons (only if enrolled)
        if (enrolled) {
            size_t total      = c.getLectii().size();
            size_t finalizate = insIt->getLectiiFinalizate().size();

            ImGui::SetCursorPosX(PAD);
            ImGui::PushStyleColor(ImGuiCol_Text, C_MUTED);
            ImGui::Text("Progres: %zu / %zu lectii finalizate", finalizate, total);
            ImGui::PopStyleColor();

            // Green progress bar; width -1 stretches to the panel edge.
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, C_GREEN);
            ImGui::PushStyleColor(ImGuiCol_FrameBg,       C_INPUT_BG);
            float fraction = total > 0
                ? static_cast<float>(finalizate) / static_cast<float>(total)
                : 0.f;
            ImGui::ProgressBar(fraction, {-1.f, 18.f}, "");
            ImGui::PopStyleColor(2);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::SetCursorPosX(PAD);
            ImGui::PushStyleColor(ImGuiCol_Text, C_ACCENT);
            ImGui::Text("Lectii — bifeaza pentru a marca ca finalizata:");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            if (c.getLectii().empty()) {
                ImGui::SetCursorPosX(PAD);
                ImGui::PushStyleColor(ImGuiCol_Text, C_MUTED);
                ImGui::Text("Acest curs nu are lectii inca.");
                ImGui::PopStyleColor();
            }

            // One checkbox per lesson. Checking calls finalizareLectie,
            // unchecking calls nefinalizareLectie; either way the enrollments
            // are reloaded so the progress bar updates this same frame.
            for (int i = 0; i < static_cast<int>(c.getLectii().size()); i++) {
                const auto& l = c.getLectii()[i];
                bool done = insIt->esteLectieFinalizata(l.getId());

                ImGui::PushID(i + 3000);
                bool chk = done;
                ImGui::SetCursorPosX(PAD);
                ImGui::PushStyleColor(ImGuiCol_Text, done ? C_GREEN : C_TEXT);
                std::string lt = l.getTitlu() + "   (" + std::to_string(l.getDurata()) + " min)";
                if (ImGui::Checkbox(lt.c_str(), &chk)) {
                    try {
                        if (done)
                            st.inscriereService.nefinalizareLectie(st.currentUser, c.getId(), l.getId());
                        else
                            st.inscriereService.finalizareLectie(st.currentUser, c.getId(), l.getId());
                        st.reloadInscrieri();
                        st.setStatus("Lectie actualizata.");
                    } catch (const std::exception& e) { st.setStatus(e.what(), false); }
                }
                ImGui::PopStyleColor();
                ImGui::PopID();
            }
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();
}
