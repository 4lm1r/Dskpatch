#include "InputHandler.hpp"
#include "HexEditor.hpp"
#include <ncurses.h>

namespace dskpatch {

void InputHandler::handleInput(int ch) {
    WINDOW* screen3 = window_manager.getScreen(3);
    mvwprintw(screen3, 3, 10, "                    ");
    wrefresh(screen3);

    int& i = hex_editor.getI();
    int& k = hex_editor.getK();
    int& x = hex_editor.getX();
    int& y = hex_editor.getY();
    int& pp = hex_editor.getPP();
    int& prev_i = hex_editor.getPrevI();
    int& prev_k = hex_editor.getPrevK();
    unsigned long& address = hex_editor.getAddress();

    switch (ch) {
        case KEY_LEFT:
            k = (k <= 0) ? 15 : k - 1;
            y = (y <= 1) ? 46 : y - 3;
            pp = (pp <= 1) ? 16 : pp - 1;
            break;
        case KEY_RIGHT:
            k = (k >= 15) ? 0 : k + 1;
            y = (y >= 46) ? 1 : y + 3;
            pp = (pp >= 16) ? 1 : pp + 1;
            break;
        case KEY_UP:
            i = (i <= 0) ? 15 : i - 1;
            x = (x <= 1) ? 16 : x - 1;
            break;
        case KEY_DOWN:
            i = (i >= 15) ? 0 : i + 1;
            x = (x >= 16) ? 1 : x + 1;
            break;
        case KEY_NPAGE:
        case '+':
            address += 256;
            file_manager.mapMemory(address, 256);
            ui_renderer.displayHeaders(address); // Ensure headers are updated
            ui_renderer.fillBoard(file_manager.getData(), i, k);
            ui_renderer.updateAddressDisplay(address);
            break;
        case KEY_PPAGE:
        case '-':
            if (address >= 256) address -= 256;
            else address = 0;
            file_manager.mapMemory(address, 256);
            ui_renderer.displayHeaders(address); // Ensure headers are updated
            ui_renderer.fillBoard(file_manager.getData(), i, k);
            ui_renderer.updateAddressDisplay(address);
            break;
        case 27: // Escape
        {
            echo();
            curs_set(1);
            char read_add[20];
            mvwgetstr(screen3, 2, 12, read_add);
            curs_set(0);
            noecho();
            char* endptr;
            address = strtol(read_add, &endptr, 16);
            if (*endptr != '\0' || endptr == read_add) {
                mvwprintw(screen3, 2, 12, "Invalid address");
                address = 0;
            }
            file_manager.mapMemory(address, 256);
            ui_renderer.displayHeaders(address); // Update headers after address change
            ui_renderer.fillBoard(file_manager.getData(), i, k);
            ui_renderer.updateAddressDisplay(address);
            break;
        }
        case 9: // TAB 
        {
          echo();
          curs_set(1);
          char input[2] = {0};
          mvwgetnstr(window_manager.getScreen(3), x, y, input, 1);
          curs_set(0);
          noecho();
          char* endptr;
          long value = strtol(input, &endptr, 16);
          if(*endptr == '\0' && endptr != input){
            file_manager.getData()[i][k] = static_cast<unsigned char>(value);
          }
          ui_renderer.fillBoard(file_manager.getData(), i, k);
          break;
        }
        case 10: // Enter
        {
            echo();
            curs_set(1);
            char input[3] = {0};
            mvwgetnstr(window_manager.getScreen(2), x, y, input, 2);
            curs_set(0);
            noecho();
            char* endptr;
            long value = strtol(input, &endptr, 16);
            if (*endptr == '\0' && endptr != input) {
                file_manager.getData()[i][k] = static_cast<unsigned char>(value);
            }
            ui_renderer.fillBoard(file_manager.getData(), i, k);
            break;
        }
        case 's':
            if (file_manager.confirmSave()) {
                file_manager.saveToFile();
            }
            break;
        case 'h':
            ui_renderer.showHelpScreen();
            break;
    }
}

} // namespace dskpatch
