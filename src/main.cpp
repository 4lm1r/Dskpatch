#include "HexEditor.hpp"
#include <iostream>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file>\n";
        return 1;
    }
    int fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        std::cerr << "Error: Cannot open file '" << argv[1] << "' with read/write permissions: " << strerror(errno) << '\n';
        return 1;
    }
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        std::cerr << "Error: Failed to get file size: " << strerror(errno) << '\n';
        close(fd);
        return 1;
    }
    close(fd);

    try {
        dskpatch::HexEditor editor(argv[1]);
        editor.initialize();
        editor.run();
    } catch (const std::exception& e) {
        endwin();
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
