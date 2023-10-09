#ifndef __ENCRYPTION_H__
#define __ENCRYPTION_H__

#include <iostream>
#include <memory>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string>

class PasswordManager {
public:
  // PasswordManager constructor
  PasswordManager() = default;

  // PasswordManager constructor
  PasswordManager(const std::string secretKey);

  ~PasswordManager();

  // Encrypts the given plaintext and returns the ciphertext in base64 format.
  std::string encrypt(const std::string &plaintext,
                      const std::string *secret = nullptr);

  // Decrypts the given ciphertext in base64 format and returns the plaintext.
  std::string decrypt(const std::string &b64_cipher,
                      const std::string *secret = nullptr);

  // Generate a new secret key
  std::string GenerateKey(std::string masterPassword);

  bool VerifyKey(const std::string &generatedKey,
                 const std::string &masterPassword);

  // Helper functions
  // encode binary data to base64
  static std::string base64Encode(const std::string &binaryData);
  // decode base64 data to binary
  static std::string base64Decode(const std::string &base64Data);

  // encode binary data to hex
  static std::string hexEncode(const std::string &binaryData);
  // decode hex data to binary
  static std::string hexDecode(const std::string &hexData);

private:
  std::string secretKey;

  void init_encryption();
  void cleanup_encryption();
};

#endif /* __ENCRYPTION_H__ */
