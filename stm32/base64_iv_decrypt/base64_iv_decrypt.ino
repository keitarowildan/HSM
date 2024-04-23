#include <AESLib.h>
#include <stm32f4xx_hal.h>

AESLib aesLib;

const int MAX_INPUT_LENGTH = 512; // Define max length for input data
char ciphertext[MAX_INPUT_LENGTH]; // Buffer to store input ciphertext

byte aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
byte aes_iv[N_BLOCK] = { 0 }; // All-zero IV (N_BLOCK should be 16 for AES-128)

void setup() {
  Serial.begin(9600);
  while (!Serial) continue; // Wait for Serial to be ready
}

void loop() {
  memset(ciphertext, 0, sizeof(ciphertext)); // Reset the ciphertext buffer

  if (Serial.available() > 0) {
    int bytesRead = Serial.readBytesUntil('\n', ciphertext, MAX_INPUT_LENGTH - 1);
    ciphertext[bytesRead] = '\0'; // Null-terminate the string

    aesLib.set_paddingmode(paddingMode::ZeroLength);

    int msgLen = strlen(ciphertext);
    char decrypted[MAX_INPUT_LENGTH] = {0}; // Allocate enough space for decrypted message
    aesLib.decrypt64(ciphertext, msgLen, (byte*)decrypted, aes_key, sizeof(aes_key), aes_iv);
    
    Serial.print("\nDecrypted Text: ");
    Serial.println(decrypted);
    delay(100);
    NVIC_SystemReset();
  }
}
