#include "input.h"

#if defined(_WIN32) || defined(_WIN64)
// Function that accepts the password
std::string requestUserPassword(const std::string &prompt, char sp) {
  std::cout << prompt;

  // Stores the password
  std::string passwd = "";
  char ch_ipt;

  // Until condition is true
  while (true) {

    ch_ipt = getch();

    // if the ch_ipt
    if (ch_ipt == IN::IN_RET) {
      std::cout << std::endl;
      return passwd;
    } else if (ch_ipt == IN::IN_BACK && passwd.length() != 0) {
      passwd.pop_back();

      // std::Cout statement is very
      // important as it will erase
      // previously printed character
      std::cout << "\b \b";

      continue;
    }

    // Without using this, program
    // will crash as \b can't be
    // print in beginning of line
    else if (ch_ipt == IN::IN_BACK && passwd.length() == 0) {
      continue;
    }

    passwd.push_back(ch_ipt);
    std::cout << sp;
  }

  return passwd;
}

#else
// Function that accepts the password
std::string requestUserPassword(const std::string &prompt, char echoChar) {
  std::cout << prompt;

  // Stores the password
  std::string passwd = "";
  char ch_ipt;

  // Stores the input
  struct termios old_ipt, new_ipt;

  // Gets the input from the user
  tcgetattr(STDIN_FILENO, &old_ipt);
  new_ipt = old_ipt;
  // Turn off canonical mode (line-buffered input) and echo
  new_ipt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_ipt);

  // Until the condition is true
  while (true) {
    ch_ipt = getchar();

    // If Enter is pressed, break the loop
    if (ch_ipt == '\n') {
      std::cout << std::endl;
      break;
    } else if (ch_ipt == 127 && passwd.length() != 0) {
      passwd.pop_back();

      // Move cursor back, erase the character, and move cursor back again
      std::cout << "\b \b";
    } else {
      passwd.push_back(ch_ipt);
      std::cout << echoChar;
    }
  }

  // Sets the input mode back to the original state
  tcsetattr(STDIN_FILENO, TCSANOW, &old_ipt);

  return passwd;
}

#endif
