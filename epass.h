#ifndef __EPASS_H__
#define __EPASS_H__
#include "encryption.h"
#include "password.h"

#include <assert.h>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

class Epass {
public:
  Epass() noexcept;
  void GenerateKey();
  bool KeyExists();
  void Load();
  void AddEntry(const std::string &name, const std::string &password);
  void PrintEntry(std::string name);
  void PrintRawEntry(std::string name);
  void DeleteEntry(std::string name);
  void ListEntries();

private:
  fs::path path;
  fs::path baseDir;
  std::unordered_map<std::string, PasswordEntry> entries;
  PasswordManager pm;

  void save();
};

#endif /* __EPASS_H__ */
