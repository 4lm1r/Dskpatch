#include "HexEditor.hpp"
#include <stdexcept>

namespace dskpatch {

HexEditor::HexEditor(const std::string& filename)
    : file_manager(filename, data, window_manager),
      ui_renderer(window_manager),
      input_handler(window_manager, ui_renderer, file_manager, *this) {}

void HexEditor::initialize() {
    window_manager.createWindows();
    window_manager.setupPanels();
    ui_renderer.displayHeaders(address);
    ui_renderer.updateAddressDisplay(address);
}

void HexEditor::run() {
    if (file_manager.mapMemory(address, BLOCK_SIZE) != 0) {
        mvwprintw(window_manager.getScreen(3), 4, 12, "Press any key to exit...");
        wrefresh(window_manager.getScreen(3));
        wgetch(window_manager.getScreen(3));
        window_manager.cleanup();
        throw std::runtime_error("Failed to map initial memory");
    }
    ui_renderer.fillBoard(data, i, k);
    keypad(window_manager.getScreen(2), TRUE);
    curs_set(0);

    int ch;
    while ((ch = wgetch(window_manager.getScreen(2))) != 'q') {
        if (WindowManager::getResizeFlag()) {
            window_manager.handleResize();
            ui_renderer.displayHeaders(address);
            ui_renderer.updateAddressDisplay(address);
            ui_renderer.fillBoard(data, i, k);
            keypad(window_manager.getScreen(2), TRUE);
            curs_set(0);
            WindowManager::setResizeFlag(false);
        }

        prev_i = i;
        prev_k = k;
        int prev_x = x, prev_y = y, prev_pp = pp;

        input_handler.handleInput(ch);

        ui_renderer.drawByte(prev_i, prev_k, prev_x, prev_y, prev_pp, false, data);
        ui_renderer.drawByte(i, k, x, y, pp, true, data);

        wrefresh(window_manager.getScreen(2));
        wrefresh(window_manager.getScreen(5));
    }
}

} // namespace dskpatch
