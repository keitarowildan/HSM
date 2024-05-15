# STM32 Hardware Security Module

## Description
The STM32 Hardware Security Module leverages the STM32F411CEU6 microcontroller along with an SD card module to perform AES-128 encryption and decryption of files. This project uniquely focuses on securely generating and managing cryptographic keys within the STM32 environment, enabling robust encryption and decryption functionalities.

## Installation Instructions
To successfully install and run the STM32 Hardware Security Module, follow these preparatory steps:
1. **Arduino IDE Setup**:
   - Follow the detailed setup instructions for installing the STM32 board package in the Arduino IDE as provided [here](https://www.sgbotic.com/index.php?dispatch=pages.view&page_id=49).

2. **Library Dependencies**:
   - Install the `SdFat` library via the Library Manager to enable operations on the SD card. [SdFat Library on GitHub](https://github.com/greiman/SdFat.git)
   - Install the `AESLib` library to facilitate AES-128 cryptographic operations. [AESLib on GitHub](https://github.com/suculent/thinx-aes-lib.git)

## Hardware Setup
Connect the STM32F411CEU6 to the SD card module as follows:

| SD card module | STM32F411CEU6 |
|----------------|---------------|
| CS             | PB12          |
| SCK            | PB13          |
| MOSI           | PB15          |
| MISO           | PB14          |
| VCC            | 5V/3.3V       |
| GND            | GND           |

## Usage
The module supports several operations through a simple menu-driven interface, accessible via a connected serial monitor:
- **Encrypt**: Encrypt a text inputted to the serial monitor by the user. The encrypted text is stored on the SD card.
- **Decrypt**: Decrypt a ciphertext inputted to the serial monitor.
- **List SD Card Content**: Display a list of files stored on the SD card.
- **Reset**: Reboot the STM32 device, generate a new key, and save the new key inside the SD card.
- **Remove all keys**: Securely delete all cryptographic keys from the SD card.

## Project Status
The project is currently **in development**. Future enhancements will focus on improving security measures, optimizing performance, and expanding the capabilities of file and text handling.
