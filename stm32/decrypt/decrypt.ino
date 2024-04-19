#include "src/lib_mbedtls/md.h"
#include "src/lib_mbedtls/aes.h"

unsigned char         key[32] = {
  0x51, 0x51, 0x46, 0x52, 0x4B, 0x56, 0x4E, 0x4F,
  0x44, 0x49, 0x4A, 0x47, 0x51, 0x52, 0x49, 0x42,
  0x59, 0x4F, 0x47, 0x52, 0x56, 0x49, 0x50, 0x4C,
  0x5A, 0x4A, 0x4E, 0x48, 0x46, 0x53, 0x4C, 0x00
};

unsigned char     iv_orig[16] = {
  'n', 't', 'u', 'z', 'e', 'r', 'y', 'k',
  'd', 'h', 'k', 'g', 'j', 'u', 'w', 'b'
}; // iv is changed during encryption, so we need to keep a copy of the original

unsigned char          iv[16] = {
  'n', 't', 'u', 'z', 'e', 'r', 'y', 'k',
  'd', 'h', 'k', 'g', 'j', 'u', 'w', 'b'
};

unsigned char       plain[128];
unsigned char plain_again[128];
unsigned char      cipher[128];

int i;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));

  Serial.println("Here we go....mbedtls on Arduino Primo....");
}

void loop() {

  Serial.write("----------------------------AES Decryption--------------------\n");

  // Clearing the arrays
  memset(plain_again, 0, sizeof(plain_again));

  // Input ciphertext
  Serial.write("Enter ciphertext (hex):\n");
  while (Serial.available() == 0);
  for (i = 0; i < sizeof(cipher); i++) {
    cipher[i] = Serial.read();
  }
  Serial.print("Ciphertext entered (hex):\n");
  for (i = 0; i < sizeof(cipher); i++) {
    Serial.print((char)cipher[i], HEX);
  }
  Serial.write("\nSize of ciphertext: ");
  Serial.println(sizeof(cipher), DEC);
  Serial.println();
  
  // Begin decryption
  mbedtls_aes_context aes_decrypt;
  mbedtls_aes_setkey_dec(&aes_decrypt, key, sizeof(key) * 8);
  mbedtls_aes_crypt_cbc(&aes_decrypt, MBEDTLS_AES_DECRYPT, sizeof(cipher), iv_orig, cipher, plain_again);

  Serial.write("Plaintext:\n");
  for (i = 0; i < sizeof(plain_again); i++) {
    Serial.write((char)plain_again[i]);
  }
  Serial.write("\nSize of plaintext: ");
  Serial.println(sizeof(plain_again), DEC);
  Serial.println();

  // Clear plain_again array
  memset(plain_again, 0, sizeof(plain_again));

  Serial.write("------------------------------------------------------------\n");
}
