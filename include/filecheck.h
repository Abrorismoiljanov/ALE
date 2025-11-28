#include <vector>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;

std::vector<std::string> GetOBJFiles(const std::string& folderPath) {
    std::vector<std::string> objFiles;

    if (!fs::exists(folderPath)) return objFiles;

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.path().extension() == ".obj") {
            objFiles.push_back(entry.path().string());
        }
    }

    return objFiles;
}
