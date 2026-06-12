#pragma once
#include "imgui.h"

// The GUI color palette. ImVec4 stores red, green, blue, alpha as floats in
// the 0..1 range, so each 0-255 channel value is divided by 255.
inline ImVec4 C_BG       { 28/255.f,  28/255.f,  38/255.f,  1.f };  // window background
inline ImVec4 C_PANEL     { 40/255.f,  40/255.f,  55/255.f,  1.f };  // side panels, top bar
inline ImVec4 C_TABBAR    { 33/255.f,  33/255.f,  46/255.f,  1.f };  // inactive tabs
inline ImVec4 C_SEL       { 60/255.f,  100/255.f, 180/255.f, 1.f };  // selected items
inline ImVec4 C_HOVER     { 50/255.f,  50/255.f,  70/255.f,  1.f };  // hovered items
inline ImVec4 C_TEXT      { 220/255.f, 220/255.f, 230/255.f, 1.f };  // normal text
inline ImVec4 C_MUTED     { 130/255.f, 130/255.f, 155/255.f, 1.f };  // secondary text
inline ImVec4 C_ACCENT    { 100/255.f, 160/255.f, 255/255.f, 1.f };  // highlights, titles
inline ImVec4 C_GREEN     { 80/255.f,  200/255.f, 120/255.f, 1.f };  // success, progress
inline ImVec4 C_RED       { 220/255.f, 80/255.f,  80/255.f,  1.f };  // errors, destructive
inline ImVec4 C_BTN       { 60/255.f,  100/255.f, 180/255.f, 1.f };  // normal buttons
inline ImVec4 C_BTN_HOV   { 80/255.f,  120/255.f, 210/255.f, 1.f };
inline ImVec4 C_BTN_ACT   { 50/255.f,  80/255.f,  150/255.f, 1.f };  // button while pressed
inline ImVec4 C_RED_HOV   { 200/255.f, 60/255.f,  60/255.f,  1.f };
inline ImVec4 C_RED_ACT   { 160/255.f, 40/255.f,  40/255.f,  1.f };
inline ImVec4 C_INPUT_BG  { 35/255.f,  35/255.f,  50/255.f,  1.f };  // text input background
inline ImVec4 C_INPUT_BD  { 80/255.f,  80/255.f,  110/255.f, 1.f };  // borders, separators

// Applies the dark theme once at startup: rounded corners, spacing, and a
// remapping of ImGui's built-in color slots to the palette above.
inline void applyTheme() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding    = 0.f;
    s.FrameRounding     = 4.f;
    s.ScrollbarRounding = 4.f;
    s.GrabRounding      = 4.f;
    s.TabRounding       = 4.f;
    s.FramePadding      = { 8.f, 5.f };
    s.ItemSpacing       = { 10.f, 8.f };
    s.ScrollbarSize     = 14.f;
    s.WindowBorderSize  = 0.f;
    s.FrameBorderSize   = 1.f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg]             = C_BG;
    c[ImGuiCol_ChildBg]              = C_PANEL;
    c[ImGuiCol_PopupBg]              = C_PANEL;
    c[ImGuiCol_Border]               = C_INPUT_BD;
    c[ImGuiCol_FrameBg]              = C_INPUT_BG;
    c[ImGuiCol_FrameBgHovered]       = C_HOVER;
    c[ImGuiCol_FrameBgActive]        = C_SEL;
    c[ImGuiCol_TitleBg]              = C_PANEL;
    c[ImGuiCol_TitleBgActive]        = C_PANEL;
    c[ImGuiCol_MenuBarBg]            = C_PANEL;
    c[ImGuiCol_ScrollbarBg]          = C_BG;
    c[ImGuiCol_ScrollbarGrab]        = C_INPUT_BD;
    c[ImGuiCol_ScrollbarGrabHovered] = C_MUTED;
    c[ImGuiCol_ScrollbarGrabActive]  = C_ACCENT;
    c[ImGuiCol_CheckMark]            = C_GREEN;
    c[ImGuiCol_SliderGrab]           = C_ACCENT;
    c[ImGuiCol_SliderGrabActive]     = C_BTN_HOV;
    c[ImGuiCol_Button]               = C_BTN;
    c[ImGuiCol_ButtonHovered]        = C_BTN_HOV;
    c[ImGuiCol_ButtonActive]         = C_BTN_ACT;
    c[ImGuiCol_Header]               = C_SEL;
    c[ImGuiCol_HeaderHovered]        = C_HOVER;
    c[ImGuiCol_HeaderActive]         = C_SEL;
    c[ImGuiCol_Tab]                  = C_TABBAR;
    c[ImGuiCol_TabHovered]           = C_HOVER;
    c[ImGuiCol_TabSelected]          = C_SEL;
    c[ImGuiCol_TabSelectedOverline]  = C_ACCENT;
    c[ImGuiCol_Separator]            = C_INPUT_BD;
    c[ImGuiCol_Text]                 = C_TEXT;
    c[ImGuiCol_TextDisabled]         = C_MUTED;
    c[ImGuiCol_PlotHistogram]        = C_GREEN;
    c[ImGuiCol_PlotHistogramHovered] = C_BTN_HOV;
    c[ImGuiCol_ModalWindowDimBg]     = { 0.f, 0.f, 0.f, 0.63f };  // dims everything behind a modal
}

// A button in red, for destructive actions (delete, unenroll).
// ImGui styling works as a stack: push the override colors, draw the widget,
// then pop them so the rest of the frame is unaffected. Returns true on click.
inline bool RedButton(const char* label) {
    ImGui::PushStyleColor(ImGuiCol_Button,        C_RED);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C_RED_HOV);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  C_RED_ACT);
    bool pressed = ImGui::Button(label);
    ImGui::PopStyleColor(3);
    return pressed;
}
