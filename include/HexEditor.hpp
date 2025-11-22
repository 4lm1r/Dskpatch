#ifndef HEX_EDITOR_HPP
#define HEX_EDITOR_HPP

#include "WindowManager.hpp"
#include "UIRenderer.hpp"
#include "InputHandler.hpp"
#include "FileManager.hpp"
#include <array>
#include <string>

namespace dskpatch {

class HexEditor {
private:
    static constexpr size_t ROWS = 16;
    static constexpr size_t COLS = 16;
    static constexpr size_t BLOCK_SIZE = 256;

    std::array<std::array<unsigned char, COLS>, ROWS> data = {};
    unsigned long address = 0;
    int x = 1, y = 1, pp = 1;
    int i = 0, k = 0;
    int prev_i = 0, prev_k = 0;

    WindowManager window_manager;
    UIRenderer ui_renderer;
    FileManager file_manager;
    InputHandler input_handler;

public:
    HexEditor(const std::string& filename);
    void initialize();
    void run();

    static std::array<std::array<unsigned char, COLS>, ROWS>& getData() { static std::array<std::array<unsigned char, COLS>, ROWS> data; return data; }
    static unsigned long& getAddress() { static unsigned long address = 0; return address; }
    int& getX() { return x; }
    int& getY() { return y; }
    int& getPP() { return pp; }
    int& getI() { return i; }
    int& getK() { return k; }
    int& getPrevI() { return prev_i; }
    int& getPrevK() { return prev_k; }
};

} // namespace dskpatch

#endif // HEX_EDITOR_HPP
