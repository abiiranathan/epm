#include "epass.h"

static std::string subcommands[] = {"keygen", "add",    "get",
                                    "list",   "delete", "help"};

static void printHelp();
static int handleAdd(int argc, char **argv, Epass &epass);
static int handleGet(int argc, char **argv, Epass &epass);

int main(int argc, char **argv) {
  // Handle HELP
  if (argc < 2 || strcmp(argv[1], "help") == 0 ||
      strcmp(argv[1], "--help") == 0) {
    printHelp();
    return 1;
  }

  // Initialise an Epass instance
  Epass epass;

  //   Handle key generation before calling Load.
  // Load will check for secret key and initialize PasswordManager or fail.
  if (strcmp(argv[1], "keygen") == 0) {
    epass.GenerateKey();
    return 0;
  }

  // will exit with code 1 if key does not exist
  epass.Load();

  std::string subcommand{argv[1]};
  if (subcommand == "add") {
    return handleAdd(argc, argv, epass);
  } else if (subcommand == "get") {
    return handleGet(argc, argv, epass);
  } else if (subcommand == "list") {
    epass.ListEntries();
  } else if (subcommand == "delete") {
    epass.DeleteEntry(argv[2]);
  } else if (subcommand == "keygen") {
    epass.GenerateKey();
  } else {
    std::cout << "Unknown subcommand '" << subcommand << "'." << std::endl;
    return 1;
  }
  return 0;
}

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

static void printHelp() {
  // print extended help
  std::cout << "Usage: epass <subcommand> [arguments]" << std::endl;
  std::cout << "Subcommands: " << std::endl;
  for (auto &subcommand : subcommands) {
    std::cout << "  " << subcommand << std::endl;

    if (subcommand == "add") {
      std::cout << "    Add a new entry to the password store." << std::endl;
      std::cout << "    Usage: epass add <name> <password>" << std::endl;
    } else if (subcommand == "get") {
      std::cout << "    Get an entry from the password store." << std::endl;
      std::cout << "    Usage: epass get <name>" << std::endl;
    } else if (subcommand == "list") {
      std::cout << "    List all entries in the password store." << std::endl;
    } else if (subcommand == "delete") {
      std::cout << "    Delete an entry from the password store." << std::endl;
      std::cout << "    Flags: epass delete <name>" << std::endl;
    } else if (subcommand == "help") {
      std::cout << "    Print this help message." << std::endl;
    } else if (subcommand == "keygen") {
      std::cout << "    Generate an encryption key." << std::endl;
    }
  }
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