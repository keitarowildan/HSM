#include <Arduino.h>
#include <stm32f4xx.h>
#include <AESLib.h>
#include <SPI.h>
#include "SdFat.h"


const int ARRAY_SIZE = 16;
byte aes_key[ARRAY_SIZE];
byte aes_decrypt_key[ARRAY_SIZE];

const int MAX_STRING_LENGTH = 128;
char plaintext[MAX_STRING_LENGTH];

AESLib aesLib;

SdFat SD;
// Define chip select pin for the SD card
#define SD_CS_PIN PB12

// Create an SPIClass object if using a custom SPI bus
static SPIClass mySPI2(PB15, PB14, PB13);  // MISO, MOSI, SCK pins

// Define the SPI configuration for the SdFat library
#define SD2_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SD_SCK_MHZ(48), &mySPI2)

char temp_for_decryption[256] = {0};
char ciphertext[512];

// General initialization vector (you must use your own IV's in production for full security!!!)
byte aes_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void setup() {
  Serial.begin(115200);  // Initialize serial communication at 115200 baud rate
  delay(300);
  while(!Serial){;}      // Wait for serial port to connect. Needed for native USB port only

  Serial.print("\nInitializing SD card...");

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!SD.begin(SD2_CONFIG)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    while (1);
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  randomSeed(analogRead(0)); // Seed the random number generator
  randomizeArray();          // Randomize the array
  printArray();              // Print the randomized array
  printBase64();             // Print and save Base64 encoded key to SD card
  printMenu();               // Display the menu options
}

void loop() {
  if (Serial.available() > 0) { // Check if data is available to read
    int option = Serial.parseInt(); // Read the option
    clearSerialBuffer();            // Clear any leftover characters in the buffer
    handleMenu(option);             // Handle the selected menu option
  }
}

void print_key_iv() {
  Serial.print("AES IV: ");
  for (unsigned int i = 0; i < sizeof(aes_iv); i++) {
    Serial.print(aes_iv[i], DEC);
    if ((i + 1) < sizeof(aes_iv)) {
      Serial.print(",");
    }
  }

  Serial.println("");
}

String encrypt_impl(char * msg, byte iv[]) {
  int msgLen = strlen(msg);
  char encrypted[2 * msgLen] = {0};
  aesLib.encrypt64((const byte*)msg, msgLen, encrypted, aes_key, sizeof(aes_key), iv);
  return String(encrypted);
}

String decrypt_impl(char * msg, byte iv[]) {
  int msgLen = strlen(msg);
  char decrypted[msgLen] = {0}; // half may be enough
  aesLib.decrypt64(msg, msgLen, (byte*)decrypted, aes_decrypt_key, sizeof(aes_decrypt_key), iv);
  return String(decrypted);
}

void printMenu() {
  Serial.println("Select an option:");
  Serial.println("1. Encrypt");
  Serial.println("2. Decrypt");
  Serial.println("3. List SD Card Content");
  Serial.println("4. Reset");
  Serial.println("5. Remove all keys");
}

void handleMenu(int option) {
  switch (option) {
    case 1:
      encryptData();
      break;
    case 2:
      decryptData();
      break;
    case 3:
      listSDCardContent();
      break;
    case 4:
      resetDevice();
      break;
    case 5:
      clearFileContents("aes_key.txt");
      break;
    default:
      Serial.println("Invalid option. Please try again.");
      printMenu(); // Only reprint menu if the option was invalid
      return;
  }
  printMenu(); // Print the menu again after completing any valid action
}

void encryptData() {
  Serial.println("Enter the plaintext:");

  // Wait for user input from serial monitor
  while (Serial.available() == 0) {
    // Wait until data is available
  }

  // Read input string from serial monitor
  int bytesRead = Serial.readBytesUntil('\n', plaintext, MAX_STRING_LENGTH - 1);
  plaintext[bytesRead] = '\0'; // Null-terminate the string
  clearSerialBuffer();
  
  // Copy plaintext to temp_for_decryption
  strncpy(temp_for_decryption, plaintext, sizeof(temp_for_decryption) - 1);
  temp_for_decryption[sizeof(temp_for_decryption) - 1] = '\0'; // Ensure null-termination

  // Print the entered plaintext
  Serial.print("Entered plaintext: ");
  Serial.println(plaintext);
  aesLib.set_paddingmode(paddingMode::ZeroLength);  

  byte enc_iv_A[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  Serial.println("Encrypting plaintext using null-IV with ZeroLength padding");
  String encrypted1 = encrypt_impl((char*)plaintext, enc_iv_A);
  Serial.print("Encrypted(1): "); Serial.println(encrypted1);
  print_key_iv();
}

void decryptData() {
  Serial.println("Enter key :");
  while (Serial.available() == 0) {
    // Wait until data is available
  }

  // Read the input from the Serial Monitor
  String encoded = Serial.readStringUntil('\n');
  
  // Remove trailing newline characters
  encoded.trim();

  // Allocate buffer for decoded data
  char buffer[16];  // Temporary buffer to hold decoded data

  // Decode Base64 string
  int decodedLength = base64_decode(buffer, encoded.c_str(), encoded.length());

  // Copy the decoded data to aes_decrypt_key
  memcpy(aes_decrypt_key, buffer, decodedLength);

  // Optionally print the decoded data for verification
  Serial.println("Decoded data:");
  for (int i = 0; i < decodedLength; i++) {
    Serial.print("0x");
    if (aes_decrypt_key[i] < 16) Serial.print("0");
    Serial.print(aes_decrypt_key[i], HEX);
    if (i < decodedLength - 1) Serial.print(", ");
  }
  Serial.println();  // Print newline to end the line

  clearSerialBuffer();
  Serial.println("Enter the ciphertext :");

  // Wait for user input from serial monitor
  while (Serial.available() == 0) {
    // Wait until data is available
  }

  // Read input string from serial monitor
  int bytesRead = Serial.readBytesUntil('\n', ciphertext , MAX_STRING_LENGTH - 1);
  ciphertext [bytesRead] = '\0'; // Null-terminate the string
  clearSerialBuffer();
  
  // Copy ciphertext  to temp_for_decryption
  strncpy(temp_for_decryption, ciphertext , sizeof(temp_for_decryption) - 1);
  temp_for_decryption[sizeof(temp_for_decryption) - 1] = '\0'; // Ensure null-termination

  // Print the entered ciphertext 
  Serial.print("Entered ciphertext : ");
  Serial.println(ciphertext);
  String for_decryption = String(temp_for_decryption);
  byte dec_iv_B[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  Serial.println("Decrypting using null-IV ZeroLength padding");
  String decrypted = decrypt_impl((char*)for_decryption.c_str(), dec_iv_B); // aes_iv fails here, incorrectly decoded...
  Serial.print("Cleartext: ");
  Serial.println(decrypted);
}

void listSDCardContent() {
  Serial.println("Listing SD Card content...");
  // Soon
}

void resetDevice() {
  Serial.println("Resetting device...");
  NVIC_SystemReset();
}

void clearSerialBuffer() {
  while (Serial.available() > 0) {
    Serial.read();  // Read and discard any leftover characters
  }
}

void randomizeArray() {
  // Shuffle the array using the Fisher-Yates algorithm
  for (int i = 0; i < ARRAY_SIZE; ++i) {
    aes_key[i] = random(256);
  }
}

void printArray() {
  Serial.print("byte aes_key[] = { ");
  for (int i = 0; i < ARRAY_SIZE; ++i) {
    Serial.print("0x");
    if (aes_key[i] < 16) {
      Serial.print("0"); // Print leading zero for single-digit hex numbers
    }
    Serial.print(aes_key[i], HEX);
    if (i < ARRAY_SIZE - 1) {
      Serial.print(", ");
    }
  }
  Serial.println(" };");
}

void printBase64() {
  char encodedData[24]; // Enough space to encode 16 bytes
  char targetBuffer[25]; // Buffer to copy into, 24 chars + null terminator
  int encodedLen = base64_encode(encodedData, (char*)aes_key, ARRAY_SIZE);

  Serial.print("Base64 encoded key: ");
  Serial.println(encodedData);

  // Copy encoded data to another buffer before writing to SD
  memcpy(targetBuffer, encodedData, 24);
  targetBuffer[24] = '\0'; // Ensure null termination (optional for this context but good practice)

  // Open the file in append mode (or create it if it doesn't exist)
  File file = SD.open("aes_key.txt", O_RDWR | O_CREAT | O_AT_END);
  if (!file) {
    Serial.println("Error opening aes_key.txt");
    return;
  }

  // Write the copied data to the file followed by a newline
  file.println(targetBuffer);

  // Close the file to save the data
  file.close();
  Serial.println("Base64 encoded key written to SD card.");
}

void clearFileContents(const char* filename) {
    // Open the file in write mode (O_WRITE | O_TRUNC)
    // O_TRUNC will truncate the file to zero length if it exists
    File file = SD.open(filename, O_WRITE | O_TRUNC);
    if (!file) {
        Serial.println("Failed to open file for clearing");
        return;
    }

    // After opening in O_WRITE | O_TRUNC mode, the file is already empty.
    file.close(); // Close the file
    Serial.println("File contents cleared.");
}
