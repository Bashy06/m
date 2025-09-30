#include <d3d9.h>
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <iostream>

#pragma comment(lib, "d3d9.lib")

class Overlay {
private:
    HWND overlay_window;
    HWND target_window;
    LPDIRECT3D9 d3d;
    LPDIRECT3DDEVICE9 device;
    bool show_menu = false;

public:
    bool initialize() {
        // Find Rust window
        target_window = FindWindowA("UnityWndClass", "Rust");
        if (!target_window) {
            std::cout << "Rust window not found!" << std::endl;
            return false;
        }

        // Create overlay window
        WNDCLASSEXA wc = { sizeof(WNDCLASSEXA) };
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = DefWindowProc;
        wc.hInstance = GetModuleHandleA(0);
        wc.lpszClassName = "RustOverlay";
        RegisterClassExA(&wc);

        overlay_window = CreateWindowExA(WS_EX_LAYERED | WS_EX_TRANSPARENT,
            "RustOverlay", "", WS_POPUP,
            0, 0, 1920, 1080, nullptr, nullptr, nullptr, nullptr);

        SetLayeredWindowAttributes(overlay_window, RGB(0, 0, 0), 0, LWA_COLORKEY);
        ShowWindow(overlay_window, SW_SHOW);

        // Initialize DirectX9
        d3d = Direct3DCreate9(D3D_SDK_VERSION);
        if (!d3d) return false;

        D3DPRESENT_PARAMETERS d3dpp = { 0 };
        d3dpp.Windowed = TRUE;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow = overlay_window;
        d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;

        if (FAILED(d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, overlay_window,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &device))) {
            return false;
        }

        // Setup ImGui
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(overlay_window);
        ImGui_ImplDX9_Init(device);

        std::cout << "Overlay initialized successfully!" << std::endl;
        return true;
    }

    void render() {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // ESP Rendering
        render_esp();

        // Menu
        if (show_menu) {
            render_menu();
        }

        ImGui::EndFrame();
        ImGui::Render();

        device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);
        device->BeginScene();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
        device->Present(nullptr, nullptr, nullptr, nullptr);
    }

    void cleanup() {
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        if (device) device->Release();
        if (d3d) d3d->Release();
        DestroyWindow(overlay_window);
    }

private:
    void render_esp() {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(1920, 1080));
        ImGui::Begin("ESP", nullptr,
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoInputs);

        // Draw test ESP - replace with real player data
        ImGui::GetWindowDrawList()->AddText(ImVec2(10, 10), IM_COL32(255, 255, 255, 255), "Rust CR3 ESP Active");
        ImGui::GetWindowDrawList()->AddRect(ImVec2(100, 100), ImVec2(200, 300), IM_COL32(255, 0, 0, 255));

        ImGui::End();
    }

    void render_menu() {
        ImGui::SetNextWindowSize(ImVec2(400, 300));
        ImGui::Begin("Rust CR3 Cheat", &show_menu);

        ImGui::Text("Kernel Driver: Connected");
        ImGui::Separator();

        if (ImGui::Button("Unload")) {
            // Add unload logic
        }

        ImGui::End();
    }
};