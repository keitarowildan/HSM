#include "src/lib_mbedtls/md.h"
#include "src/lib_mbedtls/aes.h"

unsigned char key[32];
unsigned char iv_orig[16];
unsigned char iv[16];
unsigned char plain[128];
unsigned char cipher[128];

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  Serial.println("AES Encryption Setup");
}

void loop() {
  encrypt();
  delay(10000); // Wait for 10 seconds before repeating the process
}

void encrypt() {
  Serial.println("----- AES Encryption -----");
  memset(plain, 0, sizeof(plain));
  memset(cipher, 0, sizeof(cipher));
  
  generateKey();
  generateIV();
  inputPlaintext();
  
  mbedtls_aes_context aes;
  mbedtls_aes_setkey_enc(&aes, key, 256);
  memcpy(iv, iv_orig, sizeof(iv)); // Restore original IV for encryption
  mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, sizeof(plain), iv, plain, cipher);
  
  Serial.println("Ciphertext (hex):");
  for (int i = 0; i < sizeof(cipher); i++) {
    Serial.print((char)cipher[i], HEX);
  }
  Serial.println();
}

void generateKey() {
  Serial.println("Generating random key:");
  for (int i = 0; i < sizeof(key); i++) {
    key[i] = (char)(90 - random(25));
    Serial.write(key[i]);
  }
  Serial.println();
}

void generateIV() {
  Serial.println("Generating random IV:");
  for (int i = 0; i < sizeof(iv); i++) {
    iv_orig[i] = (char)(122 - random(25));
    Serial.write(iv_orig[i]);
  }
  Serial.println();
}

void inputPlaintext() {
  Serial.println("Enter plaintext (up to 128 characters):");
  while (Serial.available() == 0);
  for (int i = 0; i < sizeof(plain) - 1; i++) {
    plain[i] = Serial.read();
    if (plain[i] == '\n') {
      plain[i] = '\0';
      break;
    }
  }
  Serial.println("Plaintext entered:");
  Serial.println((char*)plain);
}
