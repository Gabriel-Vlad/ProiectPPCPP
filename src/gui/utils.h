#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include "imgui.h"
#include "theme.h"
#include "../../core/models/Curs.h"

// Formats a price with exactly two decimals (99.9 -> "99.90").
inline std::string fmtPret(double p) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << p;
    return ss.str();
}

// Draws one course row in a left-panel list and returns true if it was clicked.
// Shared by both tabs so the rows look identical everywhere.
//
// The row is built in two layers:
//   1. An invisible Selectable ("##citem" renders no text) provides the
//      clickable area, the hover highlight and the selection background.
//   2. Text and decorations are then drawn manually on top of it through the
//      window's draw list, positioned relative to the Selectable's rectangle
//      (GetItemRectMin = the top-left corner of the last widget).
// This is the usual ImGui pattern for list rows with custom layouts.
//
// The caller is responsible for PushID (so multiple rows don't collide on the
// same "##citem" identifier) and for the separator between rows.
inline bool drawCursItem(const Curs& c, bool sel, bool enrolled,
                         float LEFT_W, float ITEM_H, float PAD) {
    // Enrolled courses get a subtle green background instead of the default.
    if (enrolled && !sel)
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.f, 0.35f, 0.15f, 0.35f));
    else
        ImGui::PushStyleColor(ImGuiCol_Header, C_SEL);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, C_HOVER);
    bool clicked = ImGui::Selectable("##citem", sel, ImGuiSelectableFlags_None,
                                     {LEFT_W - 16.f, ITEM_H});
    ImGui::PopStyleColor(2);

    ImVec2 sPos = ImGui::GetItemRectMin();

    // Thin vertical accent bar on the left edge of the selected row.
    if (sel)
        ImGui::GetWindowDrawList()->AddRectFilled(
            {sPos.x, sPos.y}, {sPos.x + 3.f, sPos.y + ITEM_H},
            IM_COL32(100, 160, 255, 255));

    // Course title, slightly larger than the default font size.
    ImGui::GetWindowDrawList()->AddText(
        ImGui::GetFont(), 18.f, {sPos.x + PAD + 4.f, sPos.y + 10.f},
        sel ? IM_COL32(255,255,255,255) : IM_COL32(220,220,230,255),
        c.getTitlu().c_str());

    // Second line: lesson count and price, in muted gray.
    std::string meta = std::to_string(c.getLectii().size()) + " lectii  |  "
                     + fmtPret(c.getPret()) + " RON";
    ImGui::GetWindowDrawList()->AddText(
        nullptr, 0.f, {sPos.x + PAD + 4.f, sPos.y + 34.f},
        IM_COL32(130,130,155,255), meta.c_str());

    // Small green "inscris" badge on the right, only in the Inscrieri tab.
    if (enrolled)
        ImGui::GetWindowDrawList()->AddText(
            nullptr, 0.f, {sPos.x + LEFT_W - 60.f, sPos.y + 22.f},
            IM_COL32(80,200,120,255), "inscris");

    return clicked;
}
