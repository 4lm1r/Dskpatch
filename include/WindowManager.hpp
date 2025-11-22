#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#include <ncurses.h>
#include <panel.h>
#include <signal.h>

namespace dskpatch {

class WindowManager {
private:
    WINDOW* screen[6] = {nullptr};
    PANEL* panels[6] = {nullptr};
    static volatile sig_atomic_t resize_flag;

public:
    WindowManager();
    ~WindowManager();

    void createWindows();
    void setupPanels();
    void handleResize();
    void cleanup();
    WINDOW* getScreen(int index) const { return screen[index]; }
    PANEL* getPanel(int index) const { return panels[index]; }

    // Static accessors for resize_flag
    static bool getResizeFlag() { return resize_flag != 0; }
    static void setResizeFlag(bool value) { resize_flag = value ? 1 : 0; }

private:
    static void handleSigwinch(int sig);
};

} // namespace dskpatch

#endif // WINDOW_MANAGER_HPP
