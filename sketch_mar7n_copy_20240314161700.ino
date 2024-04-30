#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#define RED_LED 7
#define GREEN_LED 6
#define BUZZER_PIN 8
#define servoPin  3


MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27,20,4);

Servo servo;
int pos = 0;
const int openPos = 180;    // Adjust this value according to your servo's range
const int closedPos = 0;

const unsigned long doorOpenTime = 1000;   // 3 seconds
const unsigned long doorClosedTime = 1000;

byte validCard[] = {0X0B, 0X6D,0XA3,0X89}; // Change this according to your RFID tag

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
    lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Hello,welcome!");
  lcd.setCursor(0,1);
  lcd.print("scan your card!");
  servo.attach(servoPin);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    if (checkRFID()) {
      accessGranted();
      
    } else {
      accessDenied();
      
    }
  }
}

bool checkRFID() {
  for (int i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] != validCard[i]) {
      mfrc522.PICC_HaltA();
      return false;
    }
  }
  mfrc522.PICC_HaltA();
  return true;
  

}

void accessGranted() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Access Granted!");
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  tone(BUZZER_PIN, 5000); // Buzz at 1000Hz
  delay(500); // Buzz for 0.5 seconds
  noTone(BUZZER_PIN); // Stop buzzing
  delay(1000); // Wait for 1 second
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  openDoor();
  delay(doorOpenTime);
   // Close the door
  closeDoor();
  delay(doorClosedTime);
  printNormalModeMessage();

}

void accessDenied() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Access Denied!");
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);
  printNormalModeMessage();
  
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 5000); // Buzz at 2000Hz
    delay(200); // Buzz for 0.2 seconds
    noTone(BUZZER_PIN); // Stop buzzing
    delay(200); // Wait for 0.2 seconds
  }
  delay(1000); // Wait for 1 second
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  
}

void printNormalModeMessage() {
  delay(1500);
  lcd.clear();
  lcd.print("hello!, welcome");
  lcd.setCursor(0, 1);
  lcd.print(" Scan Your card!");
}
void openDoor() {
  servo.write(openPos); // Set servo to open position
  delay(1000); // Delay for servo to reach position
}

// Function to close the door
void closeDoor() {
  servo.write(closedPos); // Set servo to closed position
  delay(1000); // Delay for servo to reach position
}

