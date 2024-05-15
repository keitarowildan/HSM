#include <SPI.h>
#include "SdFat.h"

#define SD_CS_PIN PB12
static SPIClass mySPI2(PB15, PB14, PB13);
#define SD2_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SD_SCK_MHZ(48), &mySPI2)

SdFat SD;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("\nInitializing SD card...");

  if (!SD.begin(SD2_CONFIG)) {
    Serial.println("initialization failed!");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    while (1);
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  // Open 'key.txt' for appending (writing at the end of the file without truncating it)
  // or create it if it does not exist.
  File keyFile = SD.open("key.txt", O_RDWR | O_CREAT | O_AT_END);
  
  if (!keyFile) {
    Serial.println("Opening file failed");
  } else {
    // Generate a random 24-character string
    char dataToWrite[25]; // 24 characters + 1 for the null terminator
    for (int i = 0; i < 24; i++) {
      dataToWrite[i] = random(32, 126); // Printable characters range
    }
    dataToWrite[24] = '\0'; // Null-terminate the string

    // Write the random string to the file, followed by a newline
    keyFile.println(dataToWrite);

    // Close the file
    keyFile.close();

    Serial.println("Data written to 'key.txt': ");
    Serial.println(dataToWrite);
  }
}

void loop() {
  // Do nothing
}
