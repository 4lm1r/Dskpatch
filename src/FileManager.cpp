#include "FileManager.hpp"
#include "WindowManager.hpp"
#include "HexEditor.hpp"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <ncurses.h>
#include <cstring>

namespace dskpatch {

int FileManager::mapMemory(unsigned long offset, size_t len) {
    int fd = open(filename.c_str(), O_RDWR);
    if (fd == -1) {
        mvwprintw(window_manager.getScreen(3), 2, 12, "Error: Cannot open file: %s", strerror(errno));
        wrefresh(window_manager.getScreen(3));
        return -1;
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        mvwprintw(window_manager.getScreen(3), 2, 12, "Error: Failed to get file size: %s", strerror(errno));
        wrefresh(window_manager.getScreen(3));
        close(fd);
        return -1;
    }
    if (file_size == 0) {
        mvwprintw(window_manager.getScreen(3), 2, 12, "Error: File is empty");
        wrefresh(window_manager.getScreen(3));
        close(fd);
        return -1;
    }
    if (offset >= file_size) {
        mvwprintw(window_manager.getScreen(3), 2, 12, "Error: Offset %lx exceeds file size %lx", offset, file_size);
        wrefresh(window_manager.getScreen(3));
        close(fd);
        return -1;
    }
    if (offset + len > file_size) {
        len = file_size - offset;
    }

    // Use read instead of mmap for simplicity and reliability
    unsigned char buffer[256] = {0};
    if (lseek(fd, offset, SEEK_SET) == -1) {
        mvwprintw(window_manager.getScreen(3), 2, 12, "Error: Failed to seek to offset %lx: %s", offset, strerror(errno));
        wrefresh(window_manager.getScreen(3));
        close(fd);
        return -1;
    }
    ssize_t bytes_read = read(fd, buffer, len);
    if (bytes_read == -1) {
        mvwprintw(window_manager.getScreen(3), 2, 12, "Error: Failed to read file: %s", strerror(errno));
        wrefresh(window_manager.getScreen(3));
        close(fd);
        return -1;
    }

    // Copy buffer to data array
    for (size_t i = 0, lin = 0, col = 0; i < bytes_read; ++i) {
        data[lin][col] = buffer[i];
        col++;
        if (col >= 16) {
            col = 0;
            lin++;
        }
    }
    // Clear remaining data if bytes_read < 256
    for (size_t i = bytes_read; i < 256; ++i) {
        data[i / 16][i % 16] = 0;
    }

    // Debug: Show first few bytes
    mvwprintw(window_manager.getScreen(3), 3, 12, "Loaded %zd bytes at %lx: %02x %02x %02x", bytes_read, offset,
              bytes_read > 0 ? buffer[0] : 0, bytes_read > 1 ? buffer[1] : 0, bytes_read > 2 ? buffer[2] : 0);
    wrefresh(window_manager.getScreen(3));

    close(fd);
    return 0;
}

int FileManager::createBackup() {
    std::string backup_filename = filename + ".bak";
    int src_fd = open(filename.c_str(), O_RDONLY);
    if (src_fd == -1) {
        mvwprintw(window_manager.getScreen(3), 3, 10, "Error: Cannot open file for backup: %s", strerror(errno));
        wrefresh(window_manager.getScreen(3));
        return -1;
    }

    int dst_fd = open(backup_filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd == -1) {
        mvwprintw(window_manager.getScreen(3), 3, 10, "Error: Cannot create backup file: %s", strerror(errno));
        wrefresh(window_manager.getScreen(3));
        close(src_fd);
        return -1;
    }

    char buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        ssize_t bytes_written = write(dst_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            mvwprintw(window_manager.getScreen(3), 3, 10, "Error: Failed to write backup: %s", strerror(errno));
            wrefresh(window_manager.getScreen(3));
            close(src_fd);
            close(dst_fd);
            return -1;
        }
    }
    if (bytes_read == -1) {
        mvwprintw(window_manager.getScreen(3), 3, 10, "Error: Failed to read file for backup: %s", strerror(errno));
        wrefresh(window_manager.getScreen(3));
        close(src_fd);
        close(dst_fd);
        return -1;
    }

    close(src_fd);
    close(dst_fd);
    return 0;
}

int FileManager::saveToFile() {
    if (createBackup() != 0) {
        return -1;
    }

    int fd = open(filename.c_str(), O_WRONLY);
    if (fd == -1) {
        mvwprintw(window_manager.getScreen(3), 3, 10, "Error: Cannot open file for writing: %s", strerror(errno));
        wrefresh(window_manager.getScreen(3));
        return -1;
    }

    std::array<unsigned char, 256> buffer = {};
    size_t index = 0;
    for (const auto& row : data) {
        for (const auto& byte : row) {
            buffer[index++] = byte;
        }
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        mvwprintw(window_manager.getScreen(3), 3, 10, "Error: Failed to get file size: %s", strerror(errno));
        wrefresh(window_manager.getScreen(3));
        close(fd);
        return -1;
    }

    off_t required_size = HexEditor::getAddress() + 256;
    if (required_size > file_size) {
        if (ftruncate(fd, required_size) == -1) {
            mvwprintw(window_manager.getScreen(3), 3, 10, "Error: Failed to extend file: %s", strerror(errno));
            wrefresh(window_manager.getScreen(3));
            close(fd);
            return -1;
        }
    }

    size_t bytes_to_write = 256;
    ssize_t bytes_written = pwrite(fd, buffer.data(), bytes_to_write, HexEditor::getAddress());
    if (bytes_written == -1) {
        mvwprintw(window_manager.getScreen(3), 3, 10, "Error: Failed to write to file: %s", strerror(errno));
        wrefresh(window_manager.getScreen(3));
        close(fd);
        return -1;
    }

    close(fd);
    mvwprintw(window_manager.getScreen(3), 3, 10, "Saved successfully");
    wrefresh(window_manager.getScreen(3));
    return 0;
}

bool FileManager::confirmSave() {
    WINDOW* screen3 = window_manager.getScreen(3);
    mvwprintw(screen3, 3, 10, "Save changes? (y/n)");
    wrefresh(screen3);
    int ch;
    while ((ch = wgetch(screen3)) != ERR) {
        if (ch == 'y' || ch == 'Y') {
            mvwprintw(screen3, 3, 10, "                    ");
            wrefresh(screen3);
            return true;
        }
        if (ch == 'n' || ch == 'N') {
            mvwprintw(screen3, 3, 10, "Save canceled");
            wrefresh(screen3);
            return false;
        }
    }
    mvwprintw(screen3, 3, 10, "Save canceled");
    wrefresh(screen3);
    return false;
}

} // namespace dskpatch
