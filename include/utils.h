#ifndef __UTILS_H__
#define __UTILS_H__

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

fs::path getPlatformPath();
void makeDirs(const fs::path &path);

#endif /* __UTILS_H__ */
