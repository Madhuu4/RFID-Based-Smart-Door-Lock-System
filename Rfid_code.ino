#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#define UNLOCKED_LED_PIN 8
#define LOCKED_LED_PIN 7
#define SERVO_PIN 5
#define BUZZER_PIN 6

MFRC522 rfid(SS_PIN, RST_PIN);
String lockCard = "51 233 58 197";
String unlockCard = "19 167 145 245";

bool locked = true;
Servo myServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  Serial.println("Starting the RFID Reader...");
  SPI.begin();
  rfid.PCD_Init();
  lcd.init();
  lcd.backlight();
  pinMode(UNLOCKED_LED_PIN, OUTPUT);
  pinMode(LOCKED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LOCKED_LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, LOW);
  myServo.attach(SERVO_PIN);
}

void loop() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String rfidDetected = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      rfidDetected += String(rfid.uid.uidByte[i]);
      if (i < rfid.uid.size - 1) {
        rfidDetected += " ";
      }
    }
    Serial.println(rfidDetected);

    if (rfidDetected == lockCard) {
      locked = true;
      digitalWrite(UNLOCKED_LED_PIN, LOW);
      digitalWrite(LOCKED_LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      myServo.write(180);
      displayCentered("Locked");
    } else if (rfidDetected == unlockCard) {
      locked = false;
      digitalWrite(LOCKED_LED_PIN, LOW);
      digitalWrite(UNLOCKED_LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, LOW);
      myServo.write(0);
      displayCentered("Unlocked");
    } else {
      digitalWrite(UNLOCKED_LED_PIN, LOW);
      digitalWrite(LOCKED_LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, HIGH);
      displayCentered("Unauthorized");
    }
    delay(2000); // Delay after displaying status
    digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer
    displayCentered(""); // Clear display
  }
}

void displayCentered(String text) {
  int length = text.length();
  int spaces = (16 - length) / 2;
  lcd.clear();
  lcd.setCursor(spaces, 0);
  lcd.print(text);
}
