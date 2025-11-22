#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP

#include <array>
#include <string>

namespace dskpatch {

class WindowManager;

class FileManager {
private:
    std::string filename;
    std::array<std::array<unsigned char, 16>, 16>& data;
    WindowManager& window_manager;

public:
    FileManager(const std::string& fname, std::array<std::array<unsigned char, 16>, 16>& d, WindowManager& wm)
        : filename(fname), data(d), window_manager(wm) {}
    int mapMemory(unsigned long offset, size_t len);
    int createBackup();
    int saveToFile();
    bool confirmSave();
    std::array<std::array<unsigned char, 16>, 16>& getData() { return data; }
};

} // namespace dskpatch

#endif // FILE_MANAGER_HPP
