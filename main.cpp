#include "epass.h"

static std::string subcommands[3] = {"add", "get", "list"};

static int handleAdd(int argc, char **argv, Epass &epass) {
  if (argc < 4) {
    std::cout << "Usage: " << argv[0] << " add <name> <password>" << std::endl;
    return 1;
  }

  if (strlen(argv[2]) > 64) {
    std::cout << "Name cannot be longer than 64 characters." << std::endl;
    return 1;
  }

  if (strlen(argv[3]) > 128) {
    std::cout << "Password cannot be longer than 128 characters." << std::endl;
    return 1;
  }

  std::string name{argv[2]};
  std::string password{argv[3]};

  // must not be empty
  if (name.empty()) {
    std::cout << "Name cannot be empty." << std::endl;
    return 1;
  }

  if (password.empty()) {
    std::cout << "Password cannot be empty." << std::endl;
    return 1;
  }

  epass.AddEntry(name, password);
  return 0;
}

static int handleGet(int argc, char **argv, Epass &epass) {
  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " get <name>" << std::endl;
    return 1;
  }

  std::string name{argv[2]};
  epass.PrintRawEntry(name);
  return 0;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <subcommand>" << std::endl;
    std::cout << "Subcommands: " << std::endl;
    for (auto &subcommand : subcommands) {
      std::cout << "  " << subcommand << std::endl;
    }
    return 0;
  }

  std::string subcommand{argv[1]};
  Epass epass("0123456789abcdef");
  epass.Load();

  if (subcommand == "add") {
    return handleAdd(argc, argv, epass);
  } else if (subcommand == "get") {
    return handleGet(argc, argv, epass);
  } else if (subcommand == "list") {
    epass.ListEntries();
  } else if (subcommand == "delete") {
    epass.DeleteEntry(argv[2]);
  } else {
    std::cout << "Unknown subcommand '" << subcommand << "'." << std::endl;
    return 1;
  }

  return 0;
}
