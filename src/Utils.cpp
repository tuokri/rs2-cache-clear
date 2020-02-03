#include <algorithm>
#include <sstream>
#include <functional>

#include <windows.h>
#include <ShlObj_core.h>

#include "Utils.hpp"

#pragma comment(lib, "shell32.lib")

namespace fs = std::filesystem;

fs::path
Utils::getUserDocumentsPath()
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

    return fs::path(ss.str());
}

int
Utils::countItemsInPath(const fs::path& path)
{
    auto iter = fs::recursive_directory_iterator(path);
    return static_cast<int>(std::distance(fs::begin(iter), fs::end(iter)));
}

void
Utils::rmdirCallback(const fs::path& path,
                     const std::function<void(const std::wstring& log)>& callback)
{
    std::wstringstream ss;

    for (const auto& subPath: fs::recursive_directory_iterator(path))
    {
        // fs::remove_all(subPath);
        ss << "Removed " << subPath.path().wstring() << "\n";
        callback(ss.str());
        ss.clear();
    }

    // fs::remove(path);
    ss << "Removed " << path.wstring() << "\n";
    callback(ss.str());
    ss.clear();
}
