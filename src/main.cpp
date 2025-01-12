#include <Arduino.h>
#include <Crypto.h>
#include <AES.h>
#include <EEPROM.h> // Include EEPROM library

/*
  Receives data from Serial, encrypts it, saves it in the built-in memory, and returns it decrypted to the serial.
  library: rweather/Crypto@^0.4.0


*/

byte key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F}; // 128-bit key
AES128 aes128;                                                                                                   // Create AES object
char inputString[17];                                                                                            // To store 16 characters + 1 null-terminator
int inputIndex = 0;

void setup()
{
  Serial.begin(9600); // Start serial communication at 9600 baud
  Serial.println("Enter a string to encrypt (exactly 16 characters):");

  aes128.setKey(key, 16); // Set AES key for encryption

  EEPROM.begin(512); // Initialize EEPROM (size depends on the board; typically 512 bytes for ESP8266)

  // Check if encrypted data is already stored in EEPROM
  Serial.println("Checking for saved data in EEPROM...");
  byte savedData[16];
  bool dataAvailable = false;

  // Read from EEPROM (first 16 bytes reserved for the encrypted string)
  for (int i = 0; i < 16; i++)
  {
    savedData[i] = EEPROM.read(i);
  }

  // If the first byte is not 0xFF, we assume it's valid data
  if (savedData[0] != 0xFF)
  {
    Serial.println("Encrypted data found in EEPROM!");
    byte decrypted[16];
    aes128.decryptBlock(decrypted, savedData); // Decrypt the stored data

    // Print the decrypted string
    Serial.print("Decrypted String: ");
    for (int i = 0; i < 16; i++)
    {
      Serial.print((char)decrypted[i]);
    }
    Serial.println();
  }
  else
  {
    Serial.println("No encrypted data found in EEPROM.");
  }
}

void loop()
{
  // Check if there's any data available on the serial
  if (Serial.available() > 0)
  {
    char incomingChar = Serial.read(); // Read one character at a time

    // Handle backspace key (if needed)
    if (incomingChar == 8 || incomingChar == 127)
    { // 8 is backspace, 127 is delete
      if (inputIndex > 0)
      {
        inputIndex--;
        inputString[inputIndex] = '\0'; // Remove last character
        Serial.print("\b \b");          // Clear the last character on the screen
      }
    }
    // Handle newline (Enter key)
    else if (incomingChar == '\n')
    {
      inputString[inputIndex] = '\0'; // Null-terminate the string

      // Check if the string length is 16 characters
      if (inputIndex == 16)
      {
        byte plaintext[16];
        // Convert the input string to a byte array for encryption
        for (int i = 0; i < 16; i++)
        {
          plaintext[i] = inputString[i];
        }

        byte encrypted[16];
        aes128.encryptBlock(encrypted, plaintext); // Encrypt the data

        // Display the original input string
        Serial.print("Original String: ");
        Serial.println(inputString);

        // Display the encrypted string in hexadecimal format
        Serial.print("Encrypted (Hex): ");
        for (int i = 0; i < 16; i++)
        {
          Serial.print(encrypted[i], HEX);
          Serial.print(" ");
        }
        Serial.println();

        // Save the encrypted string to EEPROM
        Serial.println("Saving encrypted data to EEPROM...");
        for (int i = 0; i < 16; i++)
        {
          EEPROM.write(i, encrypted[i]);
        }
        EEPROM.commit(); // Commit changes to EEPROM

        // Notify that the data has been saved
        Serial.println("Encrypted data saved to EEPROM!");
      }
      else
      {
        // Notify the user that the string must be exactly 16 characters
        Serial.println("Input must be exactly 16 characters long.");
      }

      // Reset for next input
      inputIndex = 0;
      Serial.println("\nEnter another string (exactly 16 characters):");
    }
    // Add character to string if it's not backspace or newline
    else if (inputIndex < 16)
    {
      inputString[inputIndex] = incomingChar;
      inputIndex++;
      Serial.print(incomingChar); // Display the character on screen
    }
  }
}
