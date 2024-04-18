#include "src/lib_mbedtls/aes.h"

// Predefined key and initialization vector (IV)
unsigned char key[32]; // 32 bytes for AES-256
unsigned char iv[16];  // 16 bytes for AES block size

// Buffer for ciphertext and decrypted text
unsigned char ciphertext[128];
unsigned char decryptedtext[128];

// AES context
mbedtls_aes_context aes;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect - needed for native USB port only
  }

  // Set key and IV directly, avoiding the null terminator issue
  memcpy(key, "12345678901234567890123456789012", 32);
  memcpy(iv, "1234567890123456", 16);

  Serial.println("Enter ciphertext to decrypt:");
}

void loop() {
  if (readCiphertext()) {
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, key, 256); // Setup decryption key for AES-256

    // Decrypt the ciphertext
    mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, sizeof(ciphertext), iv, ciphertext, decryptedtext);

    // Output the decrypted text
    Serial.println("Decrypted text:");
    Serial.println((char*)decryptedtext);

    // Clean up AES context
    mbedtls_aes_free(&aes);

    Serial.println("Enter ciphertext to decrypt:");
  }
}

bool readCiphertext() {
  static String inputHex = "";
  while (Serial.available() > 0) {
    char ch = Serial.read();
    if (ch == '\n') {
      // Convert hex string to byte array
      int len = inputHex.length();
      for (int i = 0; i < len; i += 2) {
        ciphertext[i / 2] = strtol(inputHex.substring(i, i + 2).c_str(), NULL, 16);
      }
      return true;
    } else {
      inputHex += ch;
    }
  }
  return false;
}
