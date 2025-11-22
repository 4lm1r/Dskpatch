#include "UIRenderer.hpp"
#include "HexEditor.hpp"
#include <ncurses.h>
#include <iomanip>
#include <sstream>

namespace dskpatch {

void UIRenderer::displayHeaders(unsigned long address) {
    WINDOW* screen0 = window_manager.getScreen(0);
    WINDOW* screen1 = window_manager.getScreen(1);
    WINDOW* screen4 = window_manager.getScreen(4);

    wclear(screen0);
    box(screen0, 0, 0);
    wbkgd(screen0, COLOR_PAIR(4));
    wattron(screen0, A_BOLD | A_STANDOUT);
    mvwprintw(screen0, 1, 25, "### DSKPATCH - 4.3 ###");
    wrefresh(screen0);

    wclear(screen1);
    box(screen1, 0, 0);
    wbkgd(screen1, COLOR_PAIR(2));
    int q = 5;
    for (const auto& item : list) {
        mvwprintw(screen1, 1, q, "%s", item.data());
        q += 3;
    }
    int pp = 55;
    for (char c : head) {
        mvwprintw(screen1, 1, pp++, "%c", c);
    }
    wrefresh(screen1);

    wclear(screen4);
    box(screen4, 0, 0);
    wbkgd(screen4, COLOR_PAIR(2));
    for (int u = 0; u < 16; ++u) {
        std::string addr_str = format_address(address + u * 16);
        mvwprintw(screen4, u + 1, 1, "%s", addr_str.c_str());
    }
    wrefresh(screen4);
}

void UIRenderer::updateAddressDisplay(unsigned long address) {
    WINDOW* screen3 = window_manager.getScreen(3);
    wclear(screen3);
    box(screen3, 0, 0);
    wbkgd(screen3, COLOR_PAIR(4));
    mvwprintw(screen3, 1, 10, "BASE ADDRESS:  +  OFFSET:");
    mvwprintw(screen3, 2, 12, "%lx     +    %x", address, 255);
    wrefresh(screen3);
}

void UIRenderer::fillBoard(const std::array<std::array<unsigned char, 16>, 16>& data, int i, int k) {
    WINDOW* screen2 = window_manager.getScreen(2);
    WINDOW* screen5 = window_manager.getScreen(5);
    wclear(screen2);
    box(screen2, 0, 0);
    wbkgd(screen2, COLOR_PAIR(2));
    wclear(screen5);
    box(screen5, 0, 0);
    wbkgd(screen5, COLOR_PAIR(2));
    for (int row = 0; row < 16; ++row) {
        for (int col = 0, y = 1, pp = 1; col < 16; ++col, y += 3, ++pp) {
            drawByte(row, col, row + 1, y, pp, row == i && col == k, data);
        }
    }
    wrefresh(screen2);
    wrefresh(screen5);
}

void UIRenderer::drawByte(int i, int k, int x, int y, int pp, bool highlight, const std::array<std::array<unsigned char, 16>, 16>& data) {
    WINDOW* screen2 = window_manager.getScreen(2);
    WINDOW* screen5 = window_manager.getScreen(5);
    if (highlight) {
        wattron(screen2, A_STANDOUT);
        wattron(screen5, A_STANDOUT);
    }
    char item[4];
    snprintf(item, sizeof(item), "%02x ", data[i][k]);
    mvwprintw(screen2, x, y, "%s", item);
    char temp = (data[i][k] < 32 || data[i][k] > 126) ? '.' : data[i][k];
    mvwprintw(screen5, x, pp, "%c", temp);
    if (highlight) {
        wattroff(screen2, A_STANDOUT);
        wattroff(screen5, A_STANDOUT);
    }
}

void UIRenderer::showHelpScreen() {
    std::vector<std::string> help_lines = {
        "q: Quit",
        "h: Help",
        "Arrows: Move cursor",
        "+/PgDn: Next block",
        "-/PgUp: Prev block",
        "Esc: Enter address",
        "Enter: Edit byte",
        "s: Save changes"
    };

    int lines, cols;
    getmaxyx(stdscr, lines, cols);

    if (lines < 8 || cols < 30) {
        WINDOW* screen3 = window_manager.getScreen(3);
        wclear(screen3);
        box(screen3, 0, 0);
        wbkgd(screen3, COLOR_PAIR(4));

        int max_lines = 3;
        int scroll_offset = 0;
        bool exit = false;

        while (!exit) {
            for (int i = 0; i < max_lines && i + scroll_offset < help_lines.size(); ++i) {
                mvwprintw(screen3, 1 + i, 2, "%-60s", help_lines[i + scroll_offset].c_str());
            }
            if (scroll_offset > 0) {
                mvwprintw(screen3, 1, 62, "^");
            }
            if (scroll_offset + max_lines < help_lines.size()) {
                mvwprintw(screen3, 3, 62, "v");
            }
            mvwprintw(screen3, 4, 2, "Press any key...");
            wrefresh(screen3);

            int ch = wgetch(screen3);
            switch (ch) {
                case KEY_UP:
                    if (scroll_offset > 0) scroll_offset--;
                    break;
                case KEY_DOWN:
                    if (scroll_offset + max_lines < help_lines.size()) scroll_offset++;
                    break;
                default:
                    exit = true;
                    break;
            }
        }

        wclear(screen3);
        box(screen3, 0, 0);
        wbkgd(screen3, COLOR_PAIR(4));
        updateAddressDisplay(HexEditor::getAddress());
        return;
    }

    int win_height = std::min(15, static_cast<int>(lines - 4));
    int win_width = std::min(50, static_cast<int>(cols - 4));
    win_height = std::max(8, win_height);
    win_width = std::max(30, win_width);
    int start_y = (lines - win_height) / 2;
    int start_x = (cols - win_width) / 2;
    int max_lines = win_height - 4;
    int scroll_offset = 0;

    for (int i = 0; i < 6; ++i) {
        if (window_manager.getPanel(i)) hide_panel(window_manager.getPanel(i));
    }
    update_panels();
    doupdate();

    WINDOW* help_win = newwin(win_height, win_width, start_y, start_x);
    if (!help_win) {
        WINDOW* screen3 = window_manager.getScreen(3);
        mvwprintw(screen3, 3, 10, "Error: Failed to create help window");
        wrefresh(screen3);
        for (int i = 0; i < 6; ++i) {
            if (window_manager.getPanel(i)) show_panel(window_manager.getPanel(i));
        }
        update_panels();
        doupdate();
        napms(2000);
        mvwprintw(screen3, 3, 10, "                    ");
        wrefresh(screen3);
        return;
    }

    PANEL* help_panel = new_panel(help_win);
    if (!help_panel) {
        delwin(help_win);
        WINDOW* screen3 = window_manager.getScreen(3);
        mvwprintw(screen3, 3, 10, "Error: Failed to create help panel");
        wrefresh(screen3);
        for (int i = 0; i < 6; ++i) {
            if (window_manager.getPanel(i)) show_panel(window_manager.getPanel(i));
        }
        update_panels();
        doupdate();
        napms(2000);
        mvwprintw(screen3, 3, 10, "                    ");
        wrefresh(screen3);
        return;
    }

    keypad(help_win, TRUE);
    bool exit = false;
    while (!exit) {
        wclear(help_win);
        box(help_win, 0, 0);
        wbkgd(help_win, COLOR_PAIR(4));
        mvwprintw(help_win, 1, 2, "DSKPATCH 2.0 - Help");
        mvwprintw(help_win, 2, 2, "Keybindings:");
        for (int i = 0; i < max_lines && i + scroll_offset < help_lines.size(); ++i) {
            mvwprintw(help_win, 3 + i, 4, "%s", help_lines[i + scroll_offset].c_str());
        }
        if (scroll_offset > 0) {
            mvwprintw(help_win, 3, win_width - 3, "^");
        }
        if (scroll_offset + max_lines < help_lines.size()) {
            mvwprintw(help_win, win_height - 2, win_width - 3, "v");
        }
        mvwprintw(help_win, win_height - 2, 2, "Press any key...");
        wrefresh(help_win);

        int ch = wgetch(help_win);
        switch (ch) {
            case KEY_UP:
                if (scroll_offset > 0) scroll_offset--;
                break;
            case KEY_DOWN:
                if (scroll_offset + max_lines < help_lines.size()) scroll_offset++;
                break;
            default:
                exit = true;
                break;
        }
    }

    del_panel(help_panel);
    delwin(help_win);
    for (int i = 0; i < 6; ++i) {
        if (window_manager.getPanel(i)) show_panel(window_manager.getPanel(i));
    }
    update_panels();
    doupdate();
}

std::string UIRenderer::format_address(unsigned long addr) const {
    std::stringstream ss;
    ss << std::hex << std::setw(8) << std::setfill('0') << addr;
    std::string full_addr = ss.str();
    return full_addr.substr(0, 2);
}

} // namespace dskpatch
