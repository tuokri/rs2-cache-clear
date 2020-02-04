#pragma once

#include <filesystem>
#include <functional>

namespace Utils
{

std::filesystem::path
getUserRS2ConfigDirPath();

int
countItemsInPath(const std::filesystem::path& path);

}
