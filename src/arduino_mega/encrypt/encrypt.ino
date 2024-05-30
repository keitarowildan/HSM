#include <AESLib.h>

AESLib aesLib;

const byte aes_key[] = { 
  0x2B, 0x7E, 0x15, 0x16, 
  0x28, 0xAE, 0xD2, 0xA6, 
  0xAB, 0xF7, 0x15, 0x88, 
  0x09, 0xCF, 0x4F, 0x3C 
};

const byte aes_iv[] = { 
  0x00, 0x01, 0x02, 0x03, 
  0x04, 0x05, 0x06, 0x07, 
  0x08, 0x09, 0x0A, 0x0B, 
  0x0C, 0x0D, 0x0E, 0x0F 
};

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    String encrypted = encrypt(input);
    Serial.println("Encrypted: " + encrypted);
  }
}

String encrypt(String input) {
  int inputLen = input.length() + 1; // +1 for null terminator
  char encrypted[inputLen * 2]; // allocate enough space for encrypted data
  aesLib.encrypt64((byte*)input.c_str(), inputLen, encrypted, aes_key, sizeof(aes_key), aes_iv);
  return String(encrypted);
}
