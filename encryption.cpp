#include "encryption.h"

PasswordManager::PasswordManager(const std::string secretKey) {
  this->secretKey = secretKey;
  // Initialize OpenSSL library
  init_encryption();
}

PasswordManager::~PasswordManager() {
  // Cleanup OpenSSL
  cleanup_encryption();
}

std::string PasswordManager::encrypt(const std::string &plaintext) {
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  EVP_EncryptInit_ex(
      ctx, EVP_aes_128_ecb(), NULL,
      reinterpret_cast<const unsigned char *>(this->secretKey.c_str()), NULL);

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

std::string PasswordManager::decrypt(const std::string &ciphertext) {
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  EVP_DecryptInit_ex(
      ctx, EVP_aes_128_ecb(), NULL,
      reinterpret_cast<const unsigned char *>(this->secretKey.c_str()), NULL);

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

std::string PasswordManager::GenerateKey() {
  // Generate a new secret key
  unsigned char key[EVP_MAX_KEY_LENGTH];
  unsigned char iv[EVP_MAX_IV_LENGTH];
  RAND_bytes(key, EVP_MAX_KEY_LENGTH);
  RAND_bytes(iv, EVP_MAX_IV_LENGTH);

  std::string keyStr(reinterpret_cast<char *>(key), EVP_MAX_KEY_LENGTH);
  std::string ivStr(reinterpret_cast<char *>(iv), EVP_MAX_IV_LENGTH);

  std::string keyHex = hexEncode(keyStr);
  std::string ivHex = hexEncode(ivStr);

  std::string keyB64 = base64Encode(keyHex);
  std::string ivB64 = base64Encode(ivHex);

  std::string secretKey = keyB64 + ivB64;
  return secretKey;
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
