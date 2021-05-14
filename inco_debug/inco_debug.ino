// Arduino UNO R3
//
// RFID-RC522
//    RST - 09
//    SDA - 10
//    MOSI - 11
//    MISO - 12
//    SCK - 13
//    IRQ - 3 - Interrupt
//
// HCSR04
//    ECHO - 04, 05, 06, 07
//    TRIG - A0
//
// LED Indicators
//    Red - A3 (Passenger Onboard)
//    Green - A4 (No passenger)
//    Blue - A5 (RFID card detected)

#include "SPI.h" // SPI library
#include "MFRC522.h" // RFID library (https://github.com/miguelbalboa/rfid)
#include "HCSR04.h" // HCSR04 library for multi input (https://github.com/gamegine/HCSR04-ultrasonic-sensor-lib)

const int LEDRed = A3; // Passenger onboard
const int LEDGr = A4; // No Passenger
const int LEDBlue = A5; // Card detected

const int pinIRQ = 3;
const int pinRST = 9;
const int pinSDA = 10;
MFRC522 mfrc522(pinSDA, pinRST); // Set up mfrc522 on the Arduino

HCSR04 hc(A0, new int[4] {4, 5, 6, 7}, 4); //initialization class HCSR04 (trig pin , echo pins, number of sensors)

volatile bool cardPresent = false;

void setup() {
  pinMode(pinIRQ, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinIRQ), RFIDInt, RISING); //interrupt for RC522 - IRQ

  pinMode(LEDRed, OUTPUT);
  pinMode(LEDGr, OUTPUT);
  pinMode(LEDBlue, OUTPUT);

  SPI.begin(); // open SPI connection
  mfrc522.PCD_Init(); // Initialize Proximity Coupling Device (PCD)
  Serial.begin(9600); // open serial connection
}

void loop() {
  if (cardPresent) {
    while (mfrc522.PICC_IsNewCardPresent()) { // (true, if RFID tag/card is present ) PICC = Proximity Integrated Circuit Card
      
      while (mfrc522.PICC_ReadCardSerial()) { // true, if RFID tag/card was read
        Serial.print("RFID TAG DETECTED - ID:");
        for (byte i = 0; i < mfrc522.uid.size; ++i) { // read id (in parts)
          Serial.print(mfrc522.uid.uidByte[i], HEX); // print id as hex values
          Serial.print(" "); // add space between hex blocks to increase readability
        }
        Serial.println(); // Print out of id is complete.
      }
      
      digitalWrite(LEDBlue, HIGH);
    }
  }

  for (int i = 0; i < 4; i++ ) {
    if (hc.dist(i) >= 50 || hc.dist(i) <= 0) {
      Serial.print("Sensor ");
      Serial.print(i + 1);
      Serial.println(": NO PASSENGER");
      digitalWrite(LEDGr, HIGH);
    }

    else {
      Serial.print("Sensor ");
      Serial.print(i + 1);
      Serial.print(": PASSENGER DETECTED - dis: ");
      Serial.print ( hc.dist(i) ); //return current distance (cm) in serial for sensor 1 to 4
      Serial.println (" cm");

      digitalWrite(LEDRed, HIGH);
    }
  }
  Serial.println("---------");
  digitalWrite(LEDRed, LOW);
  digitalWrite(LEDGr, LOW);
  digitalWrite(LEDBlue, LOW);
}


void RFIDInt() {
  cardPresent = true;
}
