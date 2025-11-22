#ifndef UI_RENDERER_HPP
#define UI_RENDERER_HPP

#include "WindowManager.hpp"
#include <array>
#include <vector>
#include <string>

namespace dskpatch {

class UIRenderer {
private:
    WindowManager& window_manager;
    std::vector<std::string> list = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F"};
    std::string head = "0123456789ABCDEF";

    std::string format_address(unsigned long addr) const;

public:
    UIRenderer(WindowManager& wm) : window_manager(wm) {}

    void displayHeaders(unsigned long address);
    void updateAddressDisplay(unsigned long address);
    void fillBoard(const std::array<std::array<unsigned char, 16>, 16>& data, int i, int k);
    void drawByte(int i, int k, int x, int y, int pp, bool highlight, const std::array<std::array<unsigned char, 16>, 16>& data);
    void showHelpScreen();
};

} // namespace dskpatch

#endif // UI_RENDERER_HPP
