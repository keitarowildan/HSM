#include "src/lib_mbedtls/md.h"
#include "src/lib_mbedtls/aes.h"

extern unsigned char key[32] = "RERKXZHUEBGXQJTYUTNCCNEWFLLWDHCF";
extern unsigned char iv_orig[16] = "rpwtbmgbepyzpggx";
extern unsigned char cipher[128];
unsigned char plain_again[128];

void setup() {
  Serial.begin(9600);
  Serial.println("AES Decryption Setup");
}

void loop() {
  decrypt();
  delay(10000); // Wait for 10 seconds before repeating the process
}

void decrypt() {
  Serial.println("----- AES Decryption -----");
  memset(plain_again, 0, sizeof(plain_again));
  
  mbedtls_aes_context aes;
  mbedtls_aes_setkey_dec(&aes, key, 256);
  mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, sizeof(cipher), iv_orig, cipher, plain_again);
  
  Serial.println("Recovered Plaintext:");
  Serial.println((char*)plain_again);
}
