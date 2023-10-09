#include "encryption.h"
#include <iomanip>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <sodium.h>
#include <vector>

#define EVP_SALT_SIZE 16 // 16 bytes (128 bits)
#define HEX_RANGE "0123456789ABCDEF"

PasswordManager::PasswordManager(const std::string secretKey) {
  this->secretKey = secretKey;
  init_encryption();
}

PasswordManager::~PasswordManager() {
  // Cleanup OpenSSL
  cleanup_encryption();
}

std::string PasswordManager::encrypt(const std::string &plaintext,
                                     const std::string *secret) {
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL,
                     reinterpret_cast<const unsigned char *>(
                         secret ? secret->c_str() : this->secretKey.c_str()),
                     NULL);

  int ciphertext_len;
  int len;
  std::string ciphertext(
      plaintext.size() + EVP_CIPHER_block_size(EVP_aes_128_ecb()), 0);

  EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char *>(&ciphertext[0]),
                    &len,
                    reinterpret_cast<const unsigned char *>(plaintext.c_str()),
                    plaintext.length());
  ciphertext_len = len;

  EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(&ciphertext[len]),
                      &len);
  ciphertext_len += len;

  EVP_CIPHER_CTX_free(ctx);

  // ciphertextlen is always longer than plaintextlen, so we need to trim the
  // string
  ciphertext.resize(ciphertext_len);
  return ciphertext;
}

std::string PasswordManager::decrypt(const std::string &ciphertext,
                                     const std::string *secret) {
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL,
                     reinterpret_cast<const unsigned char *>(
                         secret ? secret->c_str() : this->secretKey.c_str()),
                     NULL);

  int plaintext_len;
  int len;
  std::string plaintext(ciphertext.size(), 0);

  EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char *>(&plaintext[0]), &len,
                    reinterpret_cast<const unsigned char *>(ciphertext.c_str()),
                    ciphertext.length());
  plaintext_len = len;

  EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(&plaintext[len]),
                      &len);
  plaintext_len += len;

  EVP_CIPHER_CTX_free(ctx);

  // ciphertextlen is always longer than plaintextlen, so we need to trim the
  // string
  plaintext.resize(plaintext_len);
  return plaintext;
}

std::string PasswordManager::GenerateKey(std::string masterPassword) {
  if (sodium_init() < 0) {
    // Panic! The library couldn't be initialized; it's not safe to use.
    throw std::runtime_error("Sodium initialization failed.");
  }

  // Generate a random salt for password-based key derivation
  std::vector<uint8_t> salt(crypto_pwhash_SALTBYTES);
  randombytes_buf(salt.data(), salt.size());

  // Derive a key from the master password using Argon2
  std::vector<uint8_t> derivedKey(crypto_secretbox_KEYBYTES);
  if (crypto_pwhash(derivedKey.data(), derivedKey.size(),
                    masterPassword.c_str(), masterPassword.length(),
                    salt.data(), crypto_pwhash_OPSLIMIT_INTERACTIVE,
                    crypto_pwhash_MEMLIMIT_INTERACTIVE,
                    crypto_pwhash_ALG_DEFAULT) != 0) {
    throw std::runtime_error("Key derivation failed.");
  }

  // Convert the binary key to a hexadecimal string
  std::string keyStr;
  keyStr.reserve(derivedKey.size() * 2);
  for (size_t i = 0; i < derivedKey.size(); ++i) {
    keyStr += char(HEX_RANGE[((derivedKey[i] >> 4) & 0xF)]);
    keyStr += char(HEX_RANGE[(derivedKey[i] & 0xF)]);
  }

  // Encode the salt as a hexadecimal string and append it to the key
  std::string saltStr;
  saltStr.reserve(salt.size() * 2);
  for (size_t i = 0; i < salt.size(); ++i) {
    saltStr += char(HEX_RANGE[((salt[i] >> 4) & 0xF)]);
    saltStr += char(HEX_RANGE[(salt[i] & 0xF)]);
  }

  keyStr += saltStr;
  return keyStr;
}

bool PasswordManager::VerifyKey(const std::string &generatedKey,
                                const std::string &masterPassword) {
  // Check if the generated key has enough characters for the salt
  if (generatedKey.length() !=
      (crypto_secretbox_KEYBYTES * 2 + crypto_pwhash_SALTBYTES * 2)) {
    return false;
  }

  // Extract the salt from the generated key
  std::string saltHex = generatedKey.substr(crypto_secretbox_KEYBYTES * 2);

  // Convert the salt from a hexadecimal string to binary
  std::vector<uint8_t> salt(crypto_pwhash_SALTBYTES);
  for (size_t i = 0; i < crypto_pwhash_SALTBYTES; ++i) {
    char highNibble = saltHex[2 * i];
    char lowNibble = saltHex[2 * i + 1];

    uint8_t highValue = (highNibble >= 'A' && highNibble <= 'F')
                            ? (highNibble - 'A' + 10)
                            : (highNibble - '0');

    uint8_t lowValue = (lowNibble >= 'A' && lowNibble <= 'F')
                           ? (lowNibble - 'A' + 10)
                           : (lowNibble - '0');

    salt[i] = (highValue << 4) | lowValue;
  }

  // Extract the key part of the generated key
  std::string keyHex = generatedKey.substr(0, crypto_secretbox_KEYBYTES * 2);

  // Convert the hexadecimal key string to a binary key
  std::vector<uint8_t> derivedKey(crypto_secretbox_KEYBYTES);
  for (size_t i = 0; i < derivedKey.size(); ++i) {
    char highNibble = keyHex[2 * i];
    char lowNibble = keyHex[2 * i + 1];

    uint8_t highValue = (highNibble >= 'A' && highNibble <= 'F')
                            ? (highNibble - 'A' + 10)
                            : (highNibble - '0');

    uint8_t lowValue = (lowNibble >= 'A' && lowNibble <= 'F')
                           ? (lowNibble - 'A' + 10)
                           : (lowNibble - '0');

    derivedKey[i] = (highValue << 4) | lowValue;
  }

  // Derive a key from the master password using Argon2 with the extracted salt
  std::vector<uint8_t> verifiedDerivedKey(crypto_secretbox_KEYBYTES);
  if (crypto_pwhash(verifiedDerivedKey.data(), verifiedDerivedKey.size(),
                    masterPassword.c_str(), masterPassword.length(),
                    salt.data(), crypto_pwhash_OPSLIMIT_INTERACTIVE,
                    crypto_pwhash_MEMLIMIT_INTERACTIVE,
                    crypto_pwhash_ALG_DEFAULT) != 0) {
    return false;
  }

  // Compare the derived key to the verified derived key
  return derivedKey == verifiedDerivedKey;
}

void PasswordManager::init_encryption() {
  OpenSSL_add_all_algorithms();
  OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL);
  RAND_poll();
}

void PasswordManager::cleanup_encryption() {
  EVP_cleanup();
  CRYPTO_cleanup_all_ex_data();
}

std::string PasswordManager::base64Encode(const std::string &binaryData) {
  // Encode binary data to Base64
  BIO *b64 = BIO_new(BIO_f_base64());
  BIO *bio = BIO_new(BIO_s_mem());
  BIO_push(b64, bio);

  BIO_write(b64, binaryData.c_str(), binaryData.length());
  BIO_flush(b64);

  char encodedData[512]; // Adjust the buffer size as needed
  int len = BIO_read(bio, encodedData, sizeof(encodedData));
  std::string base64Data(encodedData, len);

  BIO_free_all(b64);
  return base64Data;
}

std::string PasswordManager::base64Decode(const std::string &base64Data) {
  // Decode Base64 data to binary
  BIO *b64 = BIO_new(BIO_f_base64());
  BIO *bio = BIO_new_mem_buf(base64Data.c_str(), base64Data.length());
  BIO_push(b64, bio);

  char decodedData[512];
  int len = BIO_read(b64, decodedData, sizeof(decodedData));
  std::string binaryData(decodedData, len);

  BIO_free_all(b64);
  return binaryData;
}

std::string PasswordManager::hexEncode(const std::string &binaryData) {
  // Encode binary data to hexadecimal
  std::string hexData;
  for (unsigned char byte : binaryData) {
    char hexBuffer[3];
    snprintf(hexBuffer, sizeof(hexBuffer), "%02x", byte);
    hexData += hexBuffer;
  }
  return hexData;
}

std::string PasswordManager::hexDecode(const std::string &hexData) {
  // Decode hexadecimal data to binary
  std::string binaryData;
  for (size_t i = 0; i < hexData.length(); i += 2) {
    std::string byteStr = hexData.substr(i, 2);
    unsigned char byte =
        static_cast<unsigned char>(std::stoi(byteStr, nullptr, 16));
    binaryData += byte;
  }
  return binaryData;
}
