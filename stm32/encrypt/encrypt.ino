#include "src/lib_mbedtls/md.h"
#include "src/lib_mbedtls/aes.h"

unsigned char         key[32];
unsigned char     iv_orig[16]; // iv is changed during encryption, so we need to keep a copy of the original
unsigned char          iv[16];
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

  Serial.write("----------------------------AES Encryption/Decryption--------------------\n");
  
  // Prepare data, generate according to AES algorithm
  // Generate key -- bad way for production, just for testing
  while (Serial.available() == 0);
  Serial.write("Enter the key (32 characters):\n");
  for (i = 0; i < sizeof(key); i++) {
    key[i] = Serial.read();
    Serial.write((char)key[i]);
  }
  Serial.println();
  Serial.write("Size of key: ");
  Serial.println(sizeof(key), DEC);
  Serial.println();
  
  // Generate IV
  Serial.write("Enter the IV (16 characters):\n");
  while (Serial.available() == 0);
  for (i = 0; i < sizeof(iv); i++) {
    iv_orig[i] = iv[i] = Serial.read();
    Serial.write((char)iv_orig[i]);
  }
  Serial.println();
  Serial.write("Size of IV: ");
  Serial.println(sizeof(iv), DEC);
  Serial.println();
  
  // Input plaintext
  Serial.write("Enter plaintext (up to 128 characters):\n");
  while (Serial.available() == 0);
  for (i = 0; i < sizeof(plain) - 1; i++) {
    plain[i] = Serial.read();
    if (plain[i] == '\n') {
      plain[i] = '\0';
      break;
    }
  }
  Serial.print("Plaintext entered (ascii):\n");
  Serial.println((char*)plain);
  Serial.write("Size of plaintext: ");
  Serial.println(strlen((char*)plain), DEC);
  Serial.println();
  
  // Begin encryption
  mbedtls_aes_context aes_encrypt, aes_decrypt;
  mbedtls_aes_setkey_enc(&aes_encrypt, key, sizeof(key) * 8);
  mbedtls_aes_crypt_cbc(&aes_encrypt, MBEDTLS_AES_ENCRYPT, sizeof(plain), iv, plain, cipher);

  Serial.write("Ciphertext (hex):\n");
  for (i = 0; i < sizeof(cipher); i++) {
    Serial.print((char)cipher[i], HEX);
  }
  Serial.write("\nSize of ciphertext: ");
  Serial.println(sizeof(cipher), DEC);
  Serial.println();

  // Begin decryption
  mbedtls_aes_setkey_dec(&aes_decrypt, key, sizeof(key) * 8);
  mbedtls_aes_crypt_cbc(&aes_decrypt, MBEDTLS_AES_DECRYPT, sizeof(cipher), iv_orig, cipher, plain_again);

  Serial.write("Plaintext (again):\n");
  for (i = 0; i < sizeof(plain_again); i++) {
    Serial.write((char)plain_again[i]);
  }
  Serial.write("\nSize of plaintext again: ");
  Serial.println(sizeof(plain_again), DEC);
  Serial.println();

  // Clear plain_again array
  for (i = 0; i < sizeof(plain_again); i++) {
    plain_again[i] = 0;
  }

  Serial.write("------------------------------------------------------------\n");
}
