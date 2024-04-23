#include <AESLib.h>

AESLib aesLib;

const int MAX_INPUT_LENGTH = 128; // Define max length for input data
char plaintext[MAX_INPUT_LENGTH]; // Buffer to store input plaintext

byte aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
byte aes_iv[N_BLOCK] = { 0 }; // All-zero IV (N_BLOCK should be 16 for AES-128)

void setup() {
  Serial.begin(9600);
  while (!Serial) continue; // Wait for Serial to be ready
  Serial.println("Enter plaintext (ensure it's a multiple of 16 bytes):");
}

void loop() {
  if (Serial.available() > 0) {
    int bytesRead = Serial.readBytesUntil('\n', plaintext, MAX_INPUT_LENGTH - 1);
    plaintext[bytesRead] = '\0'; // Null-terminate the string

    aesLib.set_paddingmode(paddingMode::ZeroLength);

    int msgLen = strlen(plaintext);
    char encrypted[2 * msgLen] = {0};
    aesLib.encrypt64((const byte*)plaintext, msgLen, encrypted, aes_key, sizeof(aes_key), aes_iv);
    
    Serial.print("Encrypted Text (Base64): ");
    Serial.println(encrypted);

    while (true); // Stop the loop after encrypting once
  }
}
