#include <imgui.h>
#include "overlay_dx9.cpp"

class MenuManager {
private:
    Overlay overlay;
    bool menu_visible = false;

public:
    void toggle_menu() {
        menu_visible = !menu_visible;
        overlay.set_menu_visible(menu_visible);
    }

    void initialize() {
        if (!overlay.initialize()) {
            std::cout << "Failed to initialize overlay!" << std::endl;
            return;
        }

        // Main loop
        while (true) {
            if (GetAsyncKeyState(VK_INSERT) & 1) {
                toggle_menu();
            }

            if (GetAsyncKeyState(VK_END) & 1) {
                break;
            }

            overlay.render();
            Sleep(16); // ~60 FPS
        }

        overlay.cleanup();
    }
};