#include "src/lib_mbedtls/aes.h"

// Predefined key and initialization vector (IV)
unsigned char key[32]; // 32 bytes for AES-256
unsigned char iv[16]; // 16 bytes for AES block size

// Buffers for encryption
unsigned char input[128]; // Input buffer
unsigned char output[128]; // Output buffer

mbedtls_aes_context aes;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect - needed for native USB port only
  }

  // Set key and IV directly, avoiding the null terminator issue
  memcpy(key, "12345678901234567890123456789012", 32);
  memcpy(iv, "1234567890123456", 16);

  Serial.println("Enter text to encrypt:");

  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, key, 256); // Setup encryption key for AES-256
}

void loop() {
  if (Serial.available() > 0) {
    // Read input from serial
    int i = 0;
    while (Serial.available() > 0 && i < sizeof(input) - 1) {
      char c = Serial.read();
      if (c == '\n') {
        break;
      }
      input[i++] = c;
    }
    input[i] = '\0'; // Null-terminate the string

    // Padding to ensure the input size is a multiple of AES block size (16 bytes)
    int paddingLength = 16 - (i % 16);
    memset(input + i, paddingLength, paddingLength);
    i += paddingLength;

    // Encrypt the input
    mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, i, iv, input, output);

    // Output the encrypted data in hex format
    Serial.println("Encrypted output (hex):");
    for (int j = 0; j < i; j++) {
      if (output[j] < 16) {
        Serial.print("0");
      }
      Serial.print(output[j], HEX);
    }
    Serial.println();

    Serial.println("Enter text to encrypt:");
  }
}

void cleanup() {
  mbedtls_aes_free(&aes);
}
