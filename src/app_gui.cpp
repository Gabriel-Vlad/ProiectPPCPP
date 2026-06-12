#include <SDL.h>
#include <GL/glew.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "gui/theme.h"
#include "gui/state.h"
#include "gui/tab_cursuri.h"
#include "gui/tab_inscrieri.h"
#include "gui/popups.h"

// GUI entry point. The stack is: SDL2 creates the window and delivers input
// events, OpenGL does the actual drawing, and ImGui sits on top translating
// our draw* calls into OpenGL commands every frame.
int main(int, char**) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return 1;

    // Request an OpenGL 3.3 "core profile" context — the minimum ImGui's
    // OpenGL3 backend needs, and available on virtually every GPU.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window* win = SDL_CreateWindow(
        "Platforma Cursuri Online",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 780,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (!win) { SDL_Quit(); return 1; }

    SDL_GLContext glCtx = SDL_GL_CreateContext(win);
    SDL_GL_MakeCurrent(win, glCtx);
    SDL_GL_SetSwapInterval(1);  // vsync: cap the frame rate to the monitor's

    // GLEW loads the OpenGL function pointers at runtime (on Windows the
    // OpenGL driver exposes them dynamically, they cannot be linked directly).
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return 1;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;  // don't create imgui.ini (we never restore layout)

    // Font trick for sharper text: rasterize Arial at 32px but display at
    // half scale. The glyphs are stored at double resolution, so they stay
    // crisp instead of looking blurry when ImGui scales them.
    if (!io.Fonts->AddFontFromFileTTF("assets/arial.ttf", 32.f)) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
            "Eroare", "Nu s-a putut incarca fontul: assets/arial.ttf\n"
                      "Rulati aplicatia din directorul radacina al proiectului.", win);
        ImGui::DestroyContext();
        SDL_GL_DeleteContext(glCtx);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    io.FontGlobalScale = 0.5f;

    ImGui_ImplSDL2_InitForOpenGL(win, glCtx);
    ImGui_ImplOpenGL3_Init("#version 330");
    applyTheme();

    // Load all three data files once at startup; afterwards the tabs reload
    // only what each mutation actually changed.
    AppState st;
    st.reloadCursuri();
    st.reloadInscrieri();
    st.reloadUtilizatori();

    const float TOPBAR_H = 48.f;   // height of the title/status bar
    const float LEFT_W   = 320.f;  // width of the course list panels
    const float PAD      = 12.f;   // default inner padding

    // Main loop
    // Classic immediate-mode loop: handle input events, rebuild the whole UI
    // from scratch, render, repeat — roughly 60 times per second (vsync).
    bool running = true;
    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            ImGui_ImplSDL2_ProcessEvent(&ev);  // forward mouse/keyboard to ImGui
            if (ev.type == SDL_QUIT) running = false;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Query the size every frame so the layout follows window resizing.
        int dispW, dispH;
        SDL_GetWindowSize(win, &dispW, &dispH);
        const float W = static_cast<float>(dispW);
        const float H = static_cast<float>(dispH);

        // Full-screen root window
        // One borderless ImGui window pinned to the OS window — it acts as the
        // canvas everything else is drawn into.
        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize({W, H});
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.f, 0.f});
        ImGui::Begin("##root", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove     | ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoScrollbar);
        ImGui::PopStyleVar();

        // Top bar
        ImGui::SetCursorPos({0.f, 0.f});
        ImGui::PushStyleColor(ImGuiCol_ChildBg, C_PANEL);
        ImGui::BeginChild("##topbar", {W, TOPBAR_H}, ImGuiChildFlags_None,
                          ImGuiWindowFlags_NoScrollbar);

        ImGui::SetCursorPos({PAD, (TOPBAR_H - ImGui::GetTextLineHeight()) / 2.f});
        ImGui::PushStyleColor(ImGuiCol_Text, C_ACCENT);
        ImGui::Text("Platforma Cursuri Online");
        ImGui::PopStyleColor();

        // Status message, right-aligned. Timeline: fully visible for 2s,
        // fades out during the 3rd second (alpha 1 -> 0), then disappears.
        if (!st.statusMsg.empty()) {
            using namespace std::chrono;
            float elapsed = duration<float>(steady_clock::now() - st.statusTime).count();
            float alpha   = 1.f;
            if (elapsed > 3.f) {
                st.statusMsg.clear();
            } else if (elapsed > 2.f) {
                alpha = 1.f - (elapsed - 2.f);  // linear fade over the last second
            }
            if (!st.statusMsg.empty()) {
                ImVec4 col = st.statusOk ? C_GREEN : C_RED;
                col.w = alpha;  // .w is the alpha channel
                ImGui::PushStyleColor(ImGuiCol_Text, col);
                ImVec2 sz = ImGui::CalcTextSize(st.statusMsg.c_str());
                ImGui::SetCursorPos({W - sz.x - PAD, (TOPBAR_H - sz.y) / 2.f});
                ImGui::Text("%s", st.statusMsg.c_str());
                ImGui::PopStyleColor();
            }
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();

        // Tab content
        ImGui::SetCursorPos({0.f, TOPBAR_H});
        ImGui::PushStyleColor(ImGuiCol_ChildBg, C_BG);
        ImGui::BeginChild("##content", {W, H - TOPBAR_H}, ImGuiChildFlags_None,
                          ImGuiWindowFlags_NoScrollbar);
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Tab, C_TABBAR);
        if (ImGui::BeginTabBar("##tabs")) {
            if (ImGui::BeginTabItem("  Cursuri  ")) {
                drawTabCursuri(st, W, H, TOPBAR_H, LEFT_W, PAD);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("  Inscrieri  ")) {
                drawTabInscrieri(st, W, H, TOPBAR_H, LEFT_W, PAD);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::PopStyleColor();

        ImGui::EndChild();
        ImGui::End();

        // Popups
        // Drawn after the root window so they appear above everything.
        drawFormPopup(st, W, H);
        drawConfirmPopup(st, W, H);

        // Render
        // ImGui::Render assembles the draw data; OpenGL clears the backbuffer,
        // draws it, and SwapWindow flips it onto the screen.
        ImGui::Render();
        glViewport(0, 0, dispW, dispH);
        glClearColor(C_BG.x, C_BG.y, C_BG.z, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(win);
    }

    // Tear everything down in reverse order of creation.
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(glCtx);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
