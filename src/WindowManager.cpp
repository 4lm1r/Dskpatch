#include "WindowManager.hpp"
#include <stdexcept>

namespace dskpatch {

volatile sig_atomic_t WindowManager::resize_flag = 0;

WindowManager::WindowManager() {
    initscr();
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_BLUE);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_WHITE, COLOR_BLUE);
    noecho();
    cbreak();
    signal(SIGWINCH, handleSigwinch);
}

WindowManager::~WindowManager() {
    cleanup();
}

void WindowManager::createWindows() {
    int lines, cols;
    getmaxyx(stdscr, lines, cols);
    if (lines < 25 || cols < 108) {
        endwin();
        throw std::runtime_error("Terminal too small for main UI (min 25x108)");
    }

    const int alt[6] = {3, 3, 18, 5, 18, 18};
    const int linha[6] = {1, 4, 7, 25, 7, 7};
    const int comp[6] = {72, 72, 50, 72, 4, 18};
    const int col[6] = {36, 36, 40, 36, 36, 90};

    for (int i = 0; i < 6; ++i) {
        screen[i] = newwin(alt[i], comp[i], linha[i], col[i]);
        if (!screen[i]) throw std::runtime_error("Failed to create window");
        box(screen[i], 0, 0);
        wbkgd(screen[i], COLOR_PAIR(i == 0 || i == 3 ? 4 : 2));
        wrefresh(screen[i]);
    }
}

void WindowManager::setupPanels() {
    for (int i = 0; i < 6; ++i) {
        panels[i] = new_panel(screen[i]);
        if (!panels[i]) throw std::runtime_error("Failed to create panel");
    }
    for (int i = 0; i < 5; ++i) {
        set_panel_userptr(panels[i], panels[i + 1]);
    }
    set_panel_userptr(panels[5], panels[0]);
    update_panels();
}

void WindowManager::handleResize() {
    endwin();
    refresh();
    for (int i = 0; i < 6; ++i) {
        if (panels[i]) del_panel(panels[i]);
        if (screen[i]) delwin(screen[i]);
        panels[i] = nullptr;
        screen[i] = nullptr;
    }
    createWindows();
    setupPanels();
}

void WindowManager::cleanup() {
    for (int i = 0; i < 6; ++i) {
        if (panels[i]) del_panel(panels[i]);
        if (screen[i]) delwin(screen[i]);
    }
    endwin();
}

void WindowManager::handleSigwinch(int sig) {
    resize_flag = 1;
}

} // namespace dskpatch
