//Prateek
//https://justdoelectronics.com
//https://www.youtube.com/@JustDoElectronics/videos

#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 3, 14, 15, 16, 17);

#include <MFRC522.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>

#define CS_RFID 10
#define RST_RFID 9
#define CS_SD 8


File myFile;
MFRC522 rfid(CS_RFID, RST_RFID);
String uidString;
RTC_DS1307 rtc;

const int checkInHour = 19;
const int checkInMinute = 50;
int userCheckInHour;
int userCheckInMinute;
const int redLED = 6;
const int greenLED = 7;
const int buzzer = 5;

void setup() {

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  while (!Serial)
    ;

  SPI.begin();
  rfid.PCD_Init();
  Serial.print("Initializing SD card...");
  lcd.print("Initializing ");
  lcd.setCursor(0, 1);
  lcd.print("SD card...");
  delay(3000);
  lcd.clear();
  if (!SD.begin(CS_SD)) {
    Serial.println("initialization failed!");
    lcd.print("Initializing ");
    lcd.setCursor(0, 1);
    lcd.print("failed!");
    return;
  }
  Serial.println("initialization done.");
  lcd.print("Initialization ");
  lcd.setCursor(0, 1);
  lcd.print("Done...");


  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    lcd.clear();
    lcd.print("Couldn't find RTC");
    while (1)
      ;
  } else {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    lcd.clear();
    lcd.print("RTC Not Running!");
  }
}

void loop() {

  if (rfid.PICC_IsNewCardPresent()) {
    readRFID();
    logCard();
    verifyCheckIn();
  }
  delay(10);
}

void readRFID() {
  rfid.PICC_ReadCardSerial();
  lcd.clear();
  Serial.print("Tag UID: ");
  lcd.print("Tag UID: ");
  uidString = String(rfid.uid.uidByte[0]) + " " + String(rfid.uid.uidByte[1]) + " " + String(rfid.uid.uidByte[2]) + " " + String(rfid.uid.uidByte[3]);
  Serial.println(uidString);
  lcd.setCursor(0, 1);
  lcd.print(uidString);
  delay(2000);

  tone(buzzer, 2000);
  delay(200);
  noTone(buzzer);

  delay(200);
}

void logCard() {

  digitalWrite(CS_SD, LOW);


  myFile = SD.open("DATA.txt", FILE_WRITE);


  if (myFile) {
    Serial.println("File opened ok");
    lcd.clear();
    lcd.print("File opened ok");
    delay(2000);
    myFile.print(uidString);
    myFile.print(", ");


    DateTime now = rtc.now();
    myFile.print(now.year(), DEC);
    myFile.print('/');
    myFile.print(now.month(), DEC);
    myFile.print('/');
    myFile.print(now.day(), DEC);
    myFile.print(',');
    myFile.print(now.hour(), DEC);
    myFile.print(':');
    myFile.println(now.minute(), DEC);

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.println(now.minute(), DEC);
    Serial.println("sucessfully written on SD card");

    lcd.clear();
    lcd.print(now.year(), DEC);
    lcd.print(':');
    lcd.print(now.month(), DEC);
    lcd.print(':');
    lcd.print(now.day(), DEC);
    lcd.print(' ');
    lcd.setCursor(11, 0);
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.setCursor(0, 1);
    lcd.print("Written on SD...");
    delay(2000);

    myFile.close();

    userCheckInHour = now.hour();
    userCheckInMinute = now.minute();
  } else {

    Serial.println("error opening data.txt");
    lcd.clear();
    lcd.print("error opening data.txt");
  }

  digitalWrite(CS_SD, HIGH);
}

void verifyCheckIn() {
  if ((userCheckInHour < checkInHour) || ((userCheckInHour == checkInHour) && (userCheckInMinute <= checkInMinute))) {
    digitalWrite(greenLED, HIGH);
    delay(2000);
    digitalWrite(greenLED, LOW);
    Serial.println("You're welcome!");
    lcd.clear();
    lcd.print("You're Welcome!");
  } else {
    digitalWrite(redLED, HIGH);
    delay(2000);
    digitalWrite(redLED, LOW);
    Serial.println("You are late...");
    lcd.clear();
    lcd.print("You are Late...");
    delay(3000);
    lcd.clear();
    lcd.print("Put RFID to Scan");
  }
}
