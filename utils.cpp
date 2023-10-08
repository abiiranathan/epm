#include "utils.h"

#define BASENAME "epass.bin"

fs::path getPlatformPath() {
  fs::path path;

  // Check for Windows
#if defined(_WIN32) || defined(_WIN64)
  const char *appdata = std::getenv("APPDATA");
  if (appdata == nullptr) {
    std::cout
        << "Could not get APPDATA environment variable. Using current path"
        << std::endl;
    return fs::current_path();
  }

  path = fs::path(appdata) / "epass" / BASENAME;

// Check for macOS
#elif defined(__APPLE__)
  // Get the HOME environment variable
  const char *home = std::getenv("HOME");
  if (home == nullptr) {
    std::cout << "Could not get HOME environment variable. Using current path"
              << std::endl;
    return fs::current_path();
  }

  path =
      fs::path(home) / "Library" / "Application Support" / "epass" / BASENAME;

// Assume Linux or other POSIX-compliant systems
#else
  // Get the HOME environment variable
  const char *home = std::getenv("HOME");
  if (home == nullptr) {
    std::cout << "Could not get HOME environment variable. Using current path"
              << std::endl;
    return fs::current_path();
  }

  path = fs::path(home) / ".config" / "epass" / BASENAME;
#endif

  return path;
}

void makeDirs(const fs::path &path) {
  fs::path directory = path.parent_path();
  if (!fs::exists(directory)) {
    fs::create_directories(directory);
  }
}
