/*
  AS608-Optical-Fingerprint-Sensor-enroll
    Modified on 06 Jan 2021
  by Amir Mohammad Shojaee @ Electropeak
  https://electropeak.com/learn/
    based on Adafruit Library

*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

/*
 * defined variables
*/
#define myRedLedPositive 10 // pin 10
#define myGreenLedPositive 12
#define buzzerPositive 13 // pin 9
#define RELAY 8           // pin 8

/* ------------------ */

#include <Adafruit_Fingerprint.h>

volatile int finger_status = -1;

// SoftwareSerial mySerial(2, 3); // TX/RX on fingerprint sensor
SoftwareSerial mySerial(4, 5); // TX/RX on fingerprint sensor

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

//Create software serial object to communicate with SIM800L
SoftwareSerial mySmsSerial(3, 2); //SIM800L Tx & Rx is connected to Arduino #3 & #2

int notMatchCount = 0;

void setup()
{
  // initialize the LCD
  lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();

  // put your setup code here, to run once:
  pinMode(myRedLedPositive, OUTPUT);   // Set myRedLedPositive as OUTPUT pin
  pinMode(myGreenLedPositive, OUTPUT); // Set myGreenLedPositive as OUTPUT pin
  pinMode(buzzerPositive, OUTPUT);     // Set buzzer positive as Output pin

  // initialize the LED pin as an output:
  pinMode(RELAY, OUTPUT);

  // put on red light on start
  digitalWrite(myRedLedPositive, HIGH);

  Serial.begin(9600);
  delay(1000);
  //Begin serial communication with Arduino and SIM800L
  mySmsSerial.begin(9600);

  while (!Serial)
    ; // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword())
  {

    Serial.println("Found fingerprint sensor!");

    lcd.print("Found Fingerprint");
    lcd.setCursor(0, 1);
    lcd.print("sensor");
  }
  else
  {
    Serial.println("Did not find fingerprint sensor :(");

    lcd.print("Did not find");
    lcd.setCursor(0, 1);
    lcd.print("fingerprint Sensor");

    while (1)
    {
      delay(1);
    }
  }

  finger.getTemplateCount();

  Serial.print("Sensor contains ");
  Serial.print(finger.templateCount);
  Serial.println(" templates");

  String templateCount = String(finger.templateCount);

  lcd.clear();
  lcd.print("Sensor contains");
  lcd.setCursor(0, 1);
  lcd.print(templateCount + " templates");

  Serial.println("Waiting for valid finger...");

  lcd.clear();
  lcd.print("Waiting for");
  lcd.setCursor(0, 1);
  lcd.print("valid finger...");

  delay(2000);
  lcd.clear();
  lcd.print("Place Finger to");
  lcd.setCursor(0, 1);
  lcd.print("open door");
}

void loop() // run over and over again
{

  finger_status = getFingerprintIDez();

  if (finger_status != -1 and finger_status != -2)
  {

    Serial.print("Match");

    // turn LED on:
    digitalWrite(RELAY, HIGH);

    lcd.clear();
    lcd.print("Match");
    lcd.setCursor(0, 1);
    lcd.print("Door Opened");

    // on green led
    digitalWrite(myGreenLedPositive, HIGH);
    // of buzzer tone if on
    noTone(buzzerPositive);
    // off red light if on
    digitalWrite(myRedLedPositive, LOW);

    notMatchCount = 0;

    delay(3000);
    lcd.clear();
    lcd.print("Door Closed");

    // off green led
    digitalWrite(myGreenLedPositive, LOW);
    // on red light if off
    digitalWrite(myRedLedPositive, HIGH);

    // turn LED off:
    digitalWrite(RELAY, LOW);

    delay(2000);

    lcd.clear();
    lcd.print("Place finger to");
    lcd.setCursor(0, 1);
    lcd.print("open door");
  }
  else
  {

    if (finger_status == -2)
    {

      Serial.print("Not Match");

      lcd.clear();
      lcd.print("Not Match");

      // off green light
      digitalWrite(myGreenLedPositive, LOW);
      // on red Light
      digitalWrite(myRedLedPositive, HIGH);

      notMatchCount = notMatchCount + 1;

      Serial.print(notMatchCount);

      delay(2000);

      if (notMatchCount > 2)
      {

        sendSms();

        lcd.clear();
        lcd.print("Intruder");

        // on buzzer alerm
        tone(buzzerPositive, 5000);
      }

      delay(2000);
      // of buzzer tone if on
      noTone(buzzerPositive);
      lcd.clear();
      lcd.print("Place finger");
      lcd.setCursor(0, 1);
      lcd.print("again");
    }
  }

  delay(50); //don't ned to run this at full speed.
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez()
{
  uint8_t p = finger.getImage();
  if (p != 2)
  {
    Serial.println(p);
  }
  if (p != FINGERPRINT_OK)
    return -1;

  p = finger.image2Tz();
  if (p != 2)
  {
    Serial.println(p);
  }
  if (p != FINGERPRINT_OK)
    return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)
    return -2;

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  return finger.fingerID;
}

void sendSms()
{
  mySmsSerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();

  mySmsSerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"+ZZxxxxxxxxxx\""); //change ZZ with country code and xxxxxxxxxxx with phone number to sms
  mySmsSerial.println("AT+CMGS=\"+2347035432921\"");
  updateSerial();
  mySmsSerial.print("INTRUDER ALERT!!!\nSomeone is trying to Forcefully open your Door.\nPlease contact The police. \n\n(Ereyomi Oluwaseyi Samuel: COS/1864/2016)"); //text content
  updateSerial();
  mySmsSerial.write(26);
}

void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    mySmsSerial.write(Serial.read()); //Forward what Serial received to Software Serial Port
  }
  while (mySmsSerial.available())
  {
    Serial.write(mySmsSerial.read()); //Forward what Software Serial received to Serial Port
  }
}
