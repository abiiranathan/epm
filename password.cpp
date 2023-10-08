#include "password.h"

PasswordEntry::PasswordEntry() noexcept {
  memset(name, 0, sizeof(name));
  memset(password, 0, sizeof(password));
}

PasswordEntry::PasswordEntry(const std::string &name,
                             const std::string &password) noexcept {
  SetName(name);
  SetPassword(password);
}

void PasswordEntry::SetPassword(const std::string &password) {
  memset(this->password, 0, sizeof(this->password));
  if (password.size() <= sizeof(this->password)) {
    memcpy(this->password, password.c_str(), password.size());
  } else {
    std::cerr << "Password size exceeds the maximum allowed size." << std::endl;
  }
}

void PasswordEntry::SetName(const std::string &name) {
  memset(this->name, 0, sizeof(this->name));
  if (name.size() <= sizeof(this->name)) {
    memcpy(this->name, name.c_str(), name.size());
  } else {
    std::cerr << "Name size exceeds the maximum allowed size." << std::endl;
  }
}

// Serialize a PasswordEntry to binary format
void PasswordEntry::Serialize(std::ostream &output) const {
  output.write(name, sizeof(name));
  output.write(password, sizeof(password));
}

// Deserialize a PasswordEntry from binary format
void PasswordEntry::Deserialize(std::istream &input) {
  input.read(name, sizeof(name));
  input.read(password, sizeof(password));
}

std::string PasswordEntry::GetName() const { return std::string(name); }
std::string PasswordEntry::GetPassword() const { return std::string(password); }
