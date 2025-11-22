#ifndef INPUT_HANDLER_HPP
#define INPUT_HANDLER_HPP

#include "WindowManager.hpp"
#include "UIRenderer.hpp"
#include "FileManager.hpp"

namespace dskpatch {

class HexEditor;

class InputHandler {
private:
    WindowManager& window_manager;
    UIRenderer& ui_renderer;
    FileManager& file_manager;
    HexEditor& hex_editor;

public:
    InputHandler(WindowManager& wm, UIRenderer& ur, FileManager& fm, HexEditor& he)
        : window_manager(wm), ui_renderer(ur), file_manager(fm), hex_editor(he) {}
    void handleInput(int ch);
};

} // namespace dskpatch

#endif // INPUT_HANDLER_HPP
