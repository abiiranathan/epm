#include "epass.h"
#include "utils.h"

#define KEY_FILE "epass.key"

Epass::Epass() noexcept {
  path = getPlatformPath();
  makeDirs(path);
  baseDir = path.parent_path();
}

bool Epass::KeyExists() { return fs::exists(baseDir / KEY_FILE); }

void Epass::GenerateKey() {
  std::string secret = pm.GenerateKey();
  std::cout << "Generated new secret key: " << secret << std::endl;

  // check if the key file already exists
  if (fs::exists(baseDir / KEY_FILE)) {
    std::cout << "Key file already exists. Overwrite? [y/N] ";
    std::string response;
    std::cin >> response;
    if (response != "y" && response != "Y") {
      std::cout << "Aborting." << std::endl;
      return;
    }
  }

  // TODO: save the secret key to a file
  std::fstream file(baseDir / KEY_FILE, std::ios::out | std::ios::trunc);
  if (!file.is_open()) {
    std::cout << "Could not open key file for writing." << std::endl;
    return;
  }

  file << secret;
  std::cout << "Key file written to " << baseDir / KEY_FILE << std::endl;
  file.close();
}

void Epass::Load() {
  if (KeyExists()) {
    std::fstream file(baseDir / KEY_FILE, std::ios::in);
    if (!file.is_open()) {
      std::cout << "Could not open key file for reading." << std::endl;
      exit(1);
    }

    std::string secret;
    file >> secret;
    pm = PasswordManager(secret);
    file.close();
  } else {
    std::cout << "Secret Key file does not exist. Please run 'epm keygen' to "
                 "generate a new key."
              << std::endl;
    exit(1);
  }

  // Clear existing entries
  entries.clear();

  // Open the file for reading
  std::fstream file(path, std::ios::in | std::ios::binary);
  if (!file.is_open()) {
    std::cout << "Could not open file for reading." << std::endl;
    return;
  }

  // if file is empty, return
  std::uintmax_t fileSize = fs::file_size(path);
  std::uintmax_t minSize = sizeof(PasswordEntry);

  if (fileSize == 0) {
    return;
  }

  if (fileSize < minSize) {
    // data corrupted
    std::cout << "data appears to be corrupted. Other operations may fail or "
                 "return wrong passwords"
              << std::endl;

    std::cout << "Clean file and start from scratch? [y/n]" << std::endl;
    std::string answer;
    std::cin >> answer;

    if (answer == "y" || answer == "Y") {
      // Set the write position to the beginning of the file
      file.seekp(0, std::ios::beg);
      file << "";
      file.flush();
    }
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
