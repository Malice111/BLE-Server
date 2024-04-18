#include <ArduinoBLE.h>

BLEService srvc("BLE");

// Define characteristic UUID's
#define FLOAT_CHAR_UUID "0F6774CF-7CD7-405A-A639-69414FEA2EE1"
#define INT_CHAR_UUID "A025CB76-6DD5-4592-8C81-268A5A0984EB"
#define CHAR_CHAR_UUID "DCC8EB71-368E-4418-BBE0-EF73A82DDB6A"
#define BYTE_CHAR_UUID "C1A0DE73-13C8-4A49-A35F-3D4E84183ED6"
#define BYTE_ARRAY_CHAR_UUID "5F63A6E7-BBD1-442F-A44E-3506873D17E7"

// Define custom objects
BLEFloatCharacteristic floatChar(FLOAT_CHAR_UUID, BLERead | BLEWrite);
BLEIntCharacteristic intChar(INT_CHAR_UUID, BLERead | BLEWrite);
BLEStringCharacteristic charChar(CHAR_CHAR_UUID, BLERead | BLEWrite, 20); // Max length of 20 char
BLEByteCharacteristic byteChar(BYTE_CHAR_UUID, BLERead | BLEWrite);
BLECharacteristic byteArrayChar(BYTE_ARRAY_CHAR_UUID, BLERead | BLEWrite, 256); // 256 bytes, array

void updateRandomValue() {
  float randomValue = random();
  floatChar.writeValue(randomValue);
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }
  
  BLE.setLocalName("BLE-Test"); // Set the local name for the BLE device
  BLE.setAdvertisedService(srvc); // Advertise our service

  // Add characteristics to the service
  srvc.addCharacteristic(floatChar);
  srvc.addCharacteristic(intChar);
  srvc.addCharacteristic(charChar);
  srvc.addCharacteristic(byteChar);
  srvc.addCharacteristic(byteArrayChar);

  BLE.addService(srvc); // Add the service
  
  // Start advertising
  BLE.advertise();
  Serial.println("BLE server started!");
}

void loop() {
  // Handle BLE events
  BLEDevice central = BLE.central();
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    
    while (central.connected()) {
      // Update the random value
      updateRandomValue();

      // Check if any characteristic are written to
      if (floatChar.written()) {
        int intValue = 12;
        float floatValue;
        floatChar.readValue(&intValue, sizeof(intValue));
      
        floatValue = *(float*)intValue;
    
        Serial.print("Float value: ");
        Serial.println(floatValue);
      }

      if (intChar.written()) {
        int intValue = intChar.value();
        Serial.print("Int value: ");
        Serial.println(intValue);
      }
      if (charChar.written()) {
        String charValue = charChar.value();
        Serial.print("Char value: ");
        Serial.println(charValue);
      }
      if (byteChar.written()) {
        byte byteValue = byteChar.value();
        Serial.print("Byte value: 0x");
        Serial.println(byteValue, HEX);
      }
      if (byteArrayChar.written()) {
        byte byteArrayValue[256];
        int byteArraySize = byteArrayChar.valueLength();
        byteArrayChar.readValue(byteArrayValue, byteArraySize);
        Serial.print("Byte array value: ");
        for (int i = 0; i < byteArraySize; i++) {
          Serial.print(byteArrayValue[i], HEX);
          Serial.print(" ");
        }
        Serial.println();
      }
    }
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}