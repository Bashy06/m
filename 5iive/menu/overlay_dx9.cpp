#include "overlay_dx9.h"
#include "memory_reader.h"
#include "ProcessUtils.h"
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

namespace Overlay {
    LPDIRECT3D9 g_pD3D = NULL;
    LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
    D3DPRESENT_PARAMETERS g_d3dpp = {};
    HWND g_overlayHwnd = NULL;

    // Game data structures
    struct Vector3 {
        float x, y, z;
    };

    struct PlayerEntity {
        Vector3 position;
        Vector3 headPosition;
        bool isAlive;
        float health;
        char name[32];
    };

    // Initialize overlay window
    bool InitializeOverlay() {
        // Create transparent overlay window
        WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"RustOverlay", NULL };
        RegisterClassEx(&wc);

        g_overlayHwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
            wc.lpszClassName, L"Rust Overlay", WS_POPUP,
            0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
            NULL, NULL, wc.hInstance, NULL);

        SetLayeredWindowAttributes(g_overlayHwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
        ShowWindow(g_overlayHwnd, SW_SHOW);

        // Initialize DirectX 9
        if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
            return false;

        ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
        g_d3dpp.Windowed = TRUE;
        g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        g_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
        g_d3dpp.EnableAutoDepthStencil = TRUE;
        g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
        g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

        if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_overlayHwnd,
            D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
            return false;

        return true;
    }

    // World to screen conversion
    bool WorldToScreen(Vector3 worldPos, Vector2& screenPos, float matrix[16], int windowWidth, int windowHeight) {
        Vector4 clipCoords;
        clipCoords.x = worldPos.x * matrix[0] + worldPos.y * matrix[4] + worldPos.z * matrix[8] + matrix[12];
        clipCoords.y = worldPos.x * matrix[1] + worldPos.y * matrix[5] + worldPos.z * matrix[9] + matrix[13];
        clipCoords.z = worldPos.x * matrix[2] + worldPos.y * matrix[6] + worldPos.z * matrix[10] + matrix[14];
        clipCoords.w = worldPos.x * matrix[3] + worldPos.y * matrix[7] + worldPos.z * matrix[11] + matrix[15];

        if (clipCoords.w < 0.1f)
            return false;

        Vector3 NDC;
        NDC.x = clipCoords.x / clipCoords.w;
        NDC.y = clipCoords.y / clipCoords.w;
        NDC.z = clipCoords.z / clipCoords.w;

        screenPos.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
        screenPos.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);

        return true;
    }

    // Main ESP rendering loop
    void RenderESP() {
        // Get game window dimensions
        RECT gameRect;
        GetWindowRect(FindWindow(NULL, L"Rust"), &gameRect);
        int width = gameRect.right - gameRect.left;
        int height = gameRect.bottom - gameRect.top;

        // Read view matrix from game memory
        float viewMatrix[16];
        if (!MemoryReader::ReadViewMatrix(viewMatrix)) {
            return;
        }

        // Read player entities
        std::vector<PlayerEntity> players = MemoryReader::ReadPlayerEntities();

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
        g_pd3dDevice->BeginScene();

        for (const auto& player : players) {
            if (!player.isAlive || player.health <= 0) continue;

            Vector2 feetPos, headPos;
            if (WorldToScreen(player.position, feetPos, viewMatrix, width, height) &&
                WorldToScreen(player.headPosition, headPos, viewMatrix, width, height)) {

                float height = feetPos.y - headPos.y;
                float width = height / 2.4f;

                // Draw player box
                DrawBox(headPos.x - width / 2, headPos.y, width, height, D3DCOLOR_ARGB(255, 255, 0, 0));

                // Draw health bar
                float healthPercentage = player.health / 100.0f;
                DrawHealthBar(headPos.x - width / 2 - 5, headPos.y, 3, height, healthPercentage);

                // Draw name
                DrawText(player.name, headPos.x, headPos.y - 15, D3DCOLOR_ARGB(255, 255, 255, 255));
            }
        }

        g_pd3dDevice->EndScene();
        g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
    }

    void DrawBox(float x, float y, float width, float height, D3DCOLOR color) {
        D3DRECT rects[4] = {
            {x, y, x + width, y + 1}, // top
            {x, y + height, x + width, y + height + 1}, // bottom
            {x, y, x + 1, y + height}, // left
            {x + width, y, x + width + 1, y + height} // right
        };
        g_pd3dDevice->Clear(4, rects, D3DCLEAR_TARGET, color, 0, 0);
    }

    void DrawHealthBar(float x, float y, float width, float height, float healthPercent) {
        float healthHeight = height * healthPercent;
        D3DRECT healthRect = { x, y + (height - healthHeight), x + width, y + height };
        g_pd3dDevice->Clear(1, &healthRect, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 255, 0), 0, 0);
    }

    void DrawText(const char* text, float x, float y, D3DCOLOR color) {
        // Implement text drawing using D3DXFont
        // Simplified placeholder
    }

    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}