#pragma once

#include <filesystem>
#include <functional>

namespace Utils
{

std::filesystem::path
getUserDocumentsPath();

int
countItemsInPath(const std::filesystem::path& path);

void
rmdirCallback(const std::filesystem::path& path,
              const std::function<void(const std::wstring& log)>& callback);

}
