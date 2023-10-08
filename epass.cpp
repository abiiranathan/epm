#include "epass.h"

static fs::path getPlatformPath() {
  fs::path path;

  // Check for Windows
#if defined(_WIN32) || defined(_WIN64)
  // Get the APPDATA environment variable
  const char *appdata = std::getenv("APPDATA");
  if (appdata == nullptr) {
    std::cout << "Could not get APPDATA environment variable." << std::endl;
    return "";
  }

  path = fs::path(appdata) / "epass" / "epass.bin";

// Check for macOS
#elif defined(__APPLE__)
  // Get the HOME environment variable
  const char *home = std::getenv("HOME");
  if (home == nullptr) {
    std::cout << "Could not get HOME environment variable." << std::endl;
    return "";
  }

  path = fs::path(home) / "Library" / "Application Support" / "epass" /
         "epass.bin";

// Assume Linux or other POSIX-compliant systems
#else
  // Get the HOME environment variable
  const char *home = std::getenv("HOME");
  if (home == nullptr) {
    std::cout << "Could not get HOME environment variable." << std::endl;
    return "";
  }

  path = fs::path(home) / ".config" / "epass" / "epass.bin";
#endif

  return path;
}

static void makeDirs(const fs::path &path) {
  fs::path directory = path.parent_path();
  if (!fs::exists(directory)) {
    fs::create_directories(directory);
  }
}

Epass::Epass(const std::string &secret) : pm(PasswordManager(secret)) {
  path = getPlatformPath();
  makeDirs(path);
}

void Epass::Load() {
  // Clear existing entries
  entries.clear();

  // Open the file for reading
  std::fstream file(path, std::ios::in | std::ios::binary);
  if (!file.is_open()) {
    std::cout << "Could not open file for reading." << std::endl;
    return;
  }

  // Read the binary data
  while (file.good() && !file.eof()) {
    PasswordEntry entry;
    entry.Deserialize(file);
    entries[entry.GetName()] = entry;
  }
}

void Epass::AddEntry(const std::string &name, const std::string &password) {
  std::string encryptedPassword = pm.encrypt(password);
  PasswordEntry entry(name, encryptedPassword);
  entries[entry.GetName()] = entry;
  save();
}

void Epass::PrintEntry(std::string name) {
  for (auto &[_, entry] : entries) {
    if (entry.GetName() == name) {
      std::cout << entry;
      return;
    }
  }
}

void Epass::PrintRawEntry(std::string name) {
  for (auto &[_, entry] : entries) {
    if (entry.GetName() == name) {
      std::cout << entry.GetName() << std::endl;
      std::string decryptedPassword = pm.decrypt(entry.GetPassword());
      std::cout << decryptedPassword << std::endl;
      return;
    }
  }
}

void Epass::DeleteEntry(std::string name) {
  size_t erasedKeys = entries.erase(name);
  if (erasedKeys == 0) {
    std::cout << "No entry with name '" << name << "'." << std::endl;
    return;
  }
  save();
}

void Epass::ListEntries() {
  if (entries.empty()) {
    std::cout << "No entries." << std::endl;
    return;
  }

  for (auto &[name, _] : entries) {
    std::cout << name << std::endl;
    std::cout << "-------------------------" << std::endl;
  }
}

void Epass::save() {
  std::fstream file(path, std::ios::out | std::ios::trunc | std::ios::binary);
  if (!file.is_open()) {
    std::cout << "Could not open file for writing." << std::endl;
    return;
  }

  for (auto &[_, entry] : entries) {
    if (entry.GetName().empty() || entry.GetPassword().empty()) {
      continue;
    }
    entry.Serialize(file);
  }
  file.close();
}
