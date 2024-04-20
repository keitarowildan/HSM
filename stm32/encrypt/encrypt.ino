#include <AESLib.h>
#include <xbase64.h>

// Define the AES key and IV (Initialization Vector)
const byte aes_key[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
const byte aes_iv[] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F };

// Define buffer for encrypted text
char encrypted_text[256]; // Adjust size accordingly

// AES library instance
AESLib aes;

void setup() {
    // Initialize Serial communication
    Serial.begin(9600);
}

void loop() {
    // Wait until data is available on Serial
    if (Serial.available() > 0) {
        // Read the string from Serial
        String input = Serial.readStringUntil('\n');

        // Convert String to char array
        char inputBuffer[input.length() + 1];
        input.toCharArray(inputBuffer, input.length() + 1);

        // Encrypt the input string
        uint16_t enc_len = aes.encrypt64((byte *)inputBuffer, strlen(inputBuffer), encrypted_text, aes_key, 128, (byte *)aes_iv);

        // Print the encrypted text
        Serial.print("Encrypted text: ");
        Serial.println(encrypted_text);

        // Decrypt the encrypted text (for demonstration)
        byte decrypted_text[256]; // Adjust size accordingly
        uint16_t dec_len = aes.decrypt64(encrypted_text, enc_len, decrypted_text, aes_key, 128, (byte *)aes_iv);

        // Null-terminate the decrypted text
        decrypted_text[dec_len] = '\0';

        // Print the decrypted text
        Serial.print("Decrypted text: ");
        Serial.println((char *)decrypted_text);
    }
}
