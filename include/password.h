#ifndef PASSWORD_H
#define PASSWORD_H
#include <cstring>
#include <iostream>
#include <string>

class PasswordEntry {

public:
  PasswordEntry() noexcept;
  PasswordEntry(const std::string &name, const std::string &password) noexcept;

  void SetPassword(const std::string &password);
  void SetName(const std::string &name);

  std::string GetName() const;
  std::string GetPassword() const;

  void Serialize(std::ostream &output) const;
  void Deserialize(std::istream &input);

  friend std::ostream &operator<<(std::ostream &os,
                                  const PasswordEntry &entry) {
    os << "Name: " << entry.GetName() << std::endl;
    os << "Password: " << entry.GetPassword() << std::endl;
    return os;
  }

private:
  char name[64];
  char password[128];
};

#endif /* PASSWORD_H */
