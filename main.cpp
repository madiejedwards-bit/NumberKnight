// Dear ImGui: Number Knight dashboard (Windows API + DirectX 11)
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "Questions.h"
#include "GridProgression.h"
#include <d3d11.h>
#include <tchar.h>
#include <algorithm>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#pragma warning(push,0)
#pragma warning(disable :6262)
#include "stb_image.h"
#pragma warning(pop)
// Data
static ID3D11Device*            g_pd3dDevice = nullptr;
static ID3D11DeviceContext*     g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;
static ID3D11ShaderResourceView* g_AvatarTexture = nullptr;

// declarations of helper functions
static void ApplyDashboardStyle(float main_scale);
HWND CreateApplicationWindow(float main_scale);
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void Cleanup(HWND hwnd);
bool InitializeImGui(HWND hwnd, float main_scale);
void RenderAdventureBoard(GridProgression& grid, Questions& questions, State& state);
void RenderChallengePanel(GridProgression& grid, Questions& questions, State& state);
void RenderVictoryPopUp(GridProgression& grid, Questions& questions, State& state);
void RenderStatsHeader(GridProgression& grid);
void RenderNumberKnightGame(GridProgression& grid, Questions& questions, State& state, float main_scale);
void DrawCurrentFrame(ImVec4 clear_color);
bool LoadTextureFromFile(const char* fileName, ID3D11Device* d3dDevice, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);

int main(int, char**)
{
    // DpiAwareness and Scale factor
    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    HWND hwnd = CreateApplicationWindow(main_scale);

    if (!InitializeImGui(hwnd, main_scale)) {
        return -1;
    }

    ApplyDashboardStyle(main_scale);

    // state -- UI Window Configuration
    bool show_window = false;
    ImVec4 clear_color = ImVec4(0.06f, 0.07f, 0.09f, 1.00f);

    // Instantiate game logic once so Questions and grid state persist across frames
    Questions questions;
    GridProgression grid;
    State state;

    // Set initial state for player
    state.visited[0][0] = true;
    questions.setTimeLimit();

    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (show_window)
            ImGui::ShowDemoWindow(&show_window);
        RenderNumberKnightGame(grid, questions, state, main_scale);
        DrawCurrentFrame(clear_color);
    }
    Cleanup(hwnd);
    return 0;
}
// Helper functions
static void ApplyDashboardStyle(float main_scale)
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsDark();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;
    ImGui::GetIO().FontGlobalScale = 1.25f;

    style.WindowRounding = 12.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.GrabRounding = 6.0f;
    style.WindowPadding = ImVec2(18.0f, 16.0f);
    style.FramePadding = ImVec2(10.0f, 8.0f);
    style.ItemSpacing = ImVec2(10.0f, 8.0f);
    style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.08f, 0.11f, 0.98f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.11f, 0.16f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.83f, 0.69f, 0.22f, 0.45f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.18f, 0.14f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.32f, 0.24f, 0.08f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.17f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.22f, 0.16f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.22f, 0.24f, 0.32f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.72f, 0.55f, 0.16f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.86f, 0.68f, 0.24f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.28f, 0.24f, 0.14f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.78f, 0.28f, 0.32f, 1.00f);
    colors[ImGuiCol_Text] = ImVec4(0.94f, 0.93f, 0.88f, 1.00f);
}
HWND CreateApplicationWindow(float main_scale) {
    static WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Number Knight", nullptr };
    ::RegisterClassExW(&wc);
    return ::CreateWindowW(wc.lpszClassName, L"Number Knight", WS_OVERLAPPEDWINDOW, 100, 100, (int)(1280 * main_scale), (int)(800 * main_scale), nullptr, nullptr, wc.hInstance, nullptr);
}
void Cleanup(HWND hwnd) {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(L"Number Knight", GetModuleHandle(nullptr));
}
bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}
void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}
void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer = nullptr;
    if (SUCCEEDED(g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer))) {
        if (pBackBuffer != nullptr) {
            g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
            pBackBuffer->Release();
        }
    };
}
void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam);
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
void RenderAdventureBoard(GridProgression& grid, Questions& questions, State& state) {
    ImGui::BeginChild("adventure_board", ImVec2(1110.0f, 0.0f), ImGuiChildFlags_Borders);
    ImGui::TextUnformatted("Adventure Board ");
    ImGui::SameLine();
    ImGui::TextDisabled("Gold = You | Green = Next | Blue = Goal");
    ImGui::Spacing();

    const ImVec2 squareSize(207.0f, 207.0f);
    for (int row = 0; row < BOARD_ROWS; ++row)
    {
        for (int col = 0; col < BOARD_COLS; ++col)
        {
            if (col > 0)
                ImGui::SameLine();

            ImGui::PushID(row * BOARD_COLS + col);
            const Difficulty squareDifficulty = GridProgression::DifficultyForSquare(row, col);
            const bool isPlayer = (row == grid.PlayerRow && col == grid.PlayerCol);
            const bool isTarget = (row == state.targetRow && col == state.targetCol);
            const bool isGoal = grid.isGoal(row, col);
            const bool isNextStep = grid.isAdjacent(row, col);
            const bool isVisited = state.visited[row][col];

            if (isPlayer)
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.72f, 0.55f, 0.16f, 1.0f));
            else if (isGoal)
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.42f, 0.62f, 1.0f));
            else if (isTarget)
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.62f, 0.42f, 0.12f, 1.0f));
            else if (isNextStep)
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.28f, 0.38f, 0.22f, 1.0f));
            else if (isVisited)
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.32f, 0.24f, 1.0f));
            else if (squareDifficulty == Difficulty::EASY)
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.28f, 0.24f, 1.0f));
            else if (squareDifficulty == Difficulty::MEDIUM)
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.32f, 0.28f, 0.16f, 1.0f));
            else
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.32f, 0.18f, 0.18f, 1.0f));

            std::string label;
            bool tilePressed = false;
            if (isPlayer) {
                if (g_AvatarTexture != nullptr) {
                    ImVec2 fixedSize = ImVec2(squareSize.x - 18.0f, squareSize.y - 18.0f);
                    tilePressed = ImGui::ImageButton("player_tile", (ImTextureID)g_AvatarTexture, fixedSize);
                }
                else {
                    label = "YOU";
                    tilePressed = ImGui::Button(label.c_str(), squareSize);
                }
            }
            else if (isGoal) {
                label = "END";
                tilePressed = ImGui::Button(label.c_str(), squareSize);
            }
            else {
                label = "";
                tilePressed = ImGui::Button(label.c_str(), squareSize);
            }

            if (tilePressed && !grid.isDefeated() && !state.showVictory)
            {
                if (grid.isAdjacent(row, col))
                {
                    questions.GenerateQuestionSafe(squareDifficulty);
                    questions.setTimeLimit();
                    state.targetRow = row;
                    state.targetCol = col;
                    state.awaitingAnswer = true;
                    state.focusAnswerBox = true;
                    state.answerInput = 0;
                    state.questionStartTime = ImGui::GetTime();
                    state.statusMessage = "Adjacent tile selected. Solve the question to move.";
                }
                else if (isPlayer)
                {
                    state.statusMessage = "You are already on this tile. Choose a neighboring square.";
                }
                else
                {
                    state.statusMessage = "That tile is too far. Move exactly one step from (" +
                        std::to_string(grid.PlayerRow) + ", " + std::to_string(grid.PlayerCol) + ").";
                }
            }
            ImGui::PopStyleColor();
            ImGui::PopID();
        }
    }

    ImGui::Spacing();
    // ImGui::TextDisabled("Gold = You   Green = Next step   Blue = Goal");
    ImGui::EndChild();
}
void RenderChallengePanel(GridProgression& grid, Questions& questions, State& state) {
    ImGui::BeginChild("challenge_panel", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders);
    ImGui::TextUnformatted("Current Challenge");
    ImGui::Spacing();
    ImGui::Spacing();

    if (state.targetRow >= 0)
        ImGui::Text("%s   |   %d pts",
            Questions::DifficultyName(questions.getDifficulty()),
            questions.getPoints());
    else
        ImGui::TextUnformatted("No adjacent tile selected yet.");

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::TextWrapped("%s", questions.getQuestionPrompt().c_str());
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    if (state.awaitingAnswer)
    {
        const int elapsed = (int)(ImGui::GetTime() - state.questionStartTime);
        const int timeLimit = questions.getTimeLimit();
        const int remaining = (std::max)(0, timeLimit - elapsed);
        const float timeFraction = timeLimit > 0 ? (float)remaining / (float)timeLimit : 0.0f;
        ImGui::Text("Time remaining  %ds", remaining);
        ImGui::ProgressBar(timeFraction, ImVec2(-1.0f, 0.0f), "");

        if (questions.isTimedOut(elapsed))
        {
            grid.loseHealth();
            state.awaitingAnswer = false;
            state.targetRow = -1;
            state.targetCol = -1;
            state.statusMessage = "Time ran out. You stay on your current tile and lose 1 health.";
        }
    }

    ImGui::Spacing();
    if (state.focusAnswerBox)
    {
        ImGui::SetKeyboardFocusHere();
        state.focusAnswerBox = false;
    }

    const bool pressedEnter = ImGui::InputInt("Your answer", &state.answerInput, 1, 10, ImGuiInputTextFlags_EnterReturnsTrue);
    if (ImGui::Button("Submit Answer", ImVec2(-1.0f, 90.0f)) || pressedEnter)
    {
        if (state.awaitingAnswer && !grid.isDefeated() && !state.showVictory)
        {
            const int elapsed = (int)(ImGui::GetTime() - state.questionStartTime);
            if (questions.isTimedOut(elapsed))
            {
                grid.loseHealth();
                state.awaitingAnswer = false;
                state.targetRow = -1;
                state.targetCol = -1;
                state.statusMessage = "Too late. You stay put and lose 1 health.";
            }
            else if (questions.checkAnswer(state.answerInput))
            {
                grid.Score += questions.getPoints();
                grid.moveTo(state.targetRow, state.targetCol);
                state.visited[grid.PlayerRow][grid.PlayerCol] = true;
                state.awaitingAnswer = false;
                state.statusMessage = "Correct! You moved to (" + std::to_string(grid.PlayerRow) + ", " + std::to_string(grid.PlayerCol) + ").";
                state.targetRow = -1;
                state.targetCol = -1;

                if (grid.isAtGoal())
                {
                    state.showVictory = true;
                    state.statusMessage = "You reached the goal corner (4, 4)!";
                }
            }
            else
            {
                grid.loseHealth();
                state.awaitingAnswer = false;
                state.statusMessage = "Incorrect. You do not move and lose 1 health.";
            }
        }
        else
        {
            state.statusMessage = "Select an adjacent tile before submitting.";
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextWrapped("%s", state.statusMessage.c_str());

    if (grid.isDefeated() && !state.showVictory)
    {
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.95f, 0.40f, 0.40f, 1.0f), "The knight has fallen.");
        if (ImGui::Button("Restart Quest", ImVec2(-20.0f, 82.0f)))
        {
            grid.reset();
            state.answerInput = 0;
            state.targetRow = -1;
            state.targetCol = -1;
            state.awaitingAnswer = false;
            state.showVictory = false;
            state.statusMessage = "A new quest begins!";
            for (int row = 0; row < BOARD_ROWS; ++row)
                for (int col = 0; col < BOARD_COLS; ++col)
                    state.visited[row][col] = false;
            state.visited[0][0] = true;
            questions.generateEasyQuestion();
            questions.setTimeLimit();
        }
    }

    ImGui::EndChild();
}
void RenderVictoryPopUp(GridProgression& grid, Questions& questions, State& state) {
    if (state.showVictory)
        ImGui::OpenPopup("Victory");

    ImGui::SetNextWindowSize(ImVec2(850.0f, 520.0f), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Victory", nullptr, ImGuiWindowFlags_NoResize))
    {   
        ImGui::TextUnformatted("Victory!");
        ImGui::Separator();
        ImGui::Text("You reached the end of the game!");
        ImGui::Text("Final score: %d", grid.Score);
        ImGui::Text("Health remaining: %d", grid.Health);
        ImGui::Spacing();
        if (ImGui::Button("Play Again", ImVec2(220.0f, 82.0f)))
        {
            grid.reset();
            state.answerInput = 0;
            state.targetRow = -1;
            state.targetCol = -1;
            state.awaitingAnswer = false;
            state.showVictory = false;
            state.statusMessage = "A new quest begins!";
            for (int row = 0; row < BOARD_ROWS; ++row)
                for (int col = 0; col < BOARD_COLS; ++col)
                    state.visited[row][col] = false;
            state.visited[0][0] = true;
            questions.setTimeLimit();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
void RenderStatsHeader(GridProgression& grid) {
    ImGui::TextUnformatted("Step to adjacent tiles and answer Math questions to win!");
    ImGui::Separator();

    const float healthFraction = (std::max)(0.0f, (std::min)(1.0f, grid.Health / 3.0f));
    ImGui::Text("Health");
    ImGui::SameLine(150.0f);
    ImGui::ProgressBar(healthFraction, ImVec2(220.0f, 0.0f), "");
    ImGui::SameLine(0.0f);
    ImGui::Text("%d / 3", grid.Health);
    ImGui::SameLine(0.0f, 380.0f);
    ImGui::Text("Score  %d", grid.Score);
    ImGui::SameLine(0.0f, 28.0f);
    ImGui::Separator();
}
void DrawCurrentFrame(ImVec4 clear_color) {
    ImGui::Render();
    const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    HRESULT hr = g_pSwapChain->Present(1, 0);
    g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
}
void RenderNumberKnightGame(GridProgression& grid, Questions& questions, State& state, float main_scale) {
    ImGui::SetNextWindowSize(ImVec2(920.0f * main_scale, 620.0f * main_scale), ImGuiCond_FirstUseEver);
    ImGui::Begin("Number Knight");
    RenderStatsHeader(grid);
    RenderAdventureBoard(grid, questions, state);
    ImGui::SameLine();
    RenderChallengePanel(grid, questions, state);
    RenderVictoryPopUp(grid, questions, state);
    ImGui::End();
}
bool InitializeImGui(HWND hwnd, float main_scale) {

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        return false;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromFileTTF("Calculator.ttf", 12.0f * main_scale);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    int image_width = 0;
    int image_height = 0;
    LoadTextureFromFile("knight-avatar-fullbody.png", g_pd3dDevice, &g_AvatarTexture, &image_width, &image_height);

    return true;
}
bool LoadTextureFromFile(const char* fileName, ID3D11Device* d3dDevice, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height) {
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(fileName, &image_width, &image_height, NULL, 4);
    if (image_data == NULL) return false;

    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D* pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;

    HRESULT hr = d3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    if (SUCCEEDED(hr) && pTexture != nullptr) {
        d3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
        pTexture->Release();
    }
    stbi_image_free(image_data);

    if (out_width) *out_width = image_width;
    if (out_height) *out_height = image_height;

    return true;
}
