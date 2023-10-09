#ifndef __INPUT_H__
#define __INPUT_H__

#if defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
#include <iostream>

// Enumerator
enum IN {
  // 13 is ASCII for carriage
  // return
  IN_BACK = 8,
  IN_RET = 13

};

// Function that accepts the password
std::string requestUserPassword(const std::string &prompt, char echoChar = '*');

#endif /* __INPUT_H__ */
