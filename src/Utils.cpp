#include <algorithm>
#include <sstream>
#include <functional>
#include <filesystem>

#include <windows.h>
#include <shlobj.h>

#include "Utils.hpp"

#pragma comment(lib, "shell32.lib")

namespace fs = std::filesystem;

fs::path
Utils::getUserRS2ConfigDirPath()
{
    wchar_t* documents = nullptr;
    HRESULT result = SHGetKnownFolderPath(
        FOLDERID_Documents, KF_FLAG_DEFAULT, nullptr, &documents);

    std::wstringstream ss;
    ss << documents;
    CoTaskMemFree(static_cast<void*>(documents));

    if (result != S_OK)
    {
        throw std::runtime_error("unable to locate user documents directory");
    }

    ss << R"(\My Games\Rising Storm 2\ROGame)";
    fs::path path = fs::path(ss.str());
    if (!fs::exists(path))
    {
        throw std::runtime_error("unable to locate ROGame directory; missing or already cleared");
    }

    return path;
}

int
Utils::countItemsInPath(const fs::path& path)
{
    auto iter = fs::recursive_directory_iterator(path);
    return static_cast<int>(std::distance(fs::begin(iter), fs::end(iter)));
}
