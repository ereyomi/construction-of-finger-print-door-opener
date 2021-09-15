  /*
  AS608-Optical-Fingerprint-Sensor-enroll
  https://electropeak.com/learn/
    based on Adafruit Library

*/
#include <Adafruit_Fingerprint.h>
/*
 * lcd include
*/
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
/*-----------------*/

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);


// SoftwareSerial mySerial(2, 3); // TX/RX
SoftwareSerial mySerial(4, 5); // TX/RX on fingerprint sensor

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;
/*
 * display LCD function
*/
void displayOnLcd(char* msg){
  
}

/*---------------*/

void setup()  
{
  // initialize the LCD
  lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();
  // ----------------- 
  delay(500);
  
  
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  
  delay(100);
  
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    
    lcd.print("Found fingerprint");
    lcd.setCursor(0, 1);
    lcd.print("sensor!");
    
    Serial.println("Found fingerprint sensor!");
  
    
  } else {
    
    lcd.print("Did not find");
    lcd.setCursor(0, 1);
    lcd.print("fingerprint sensor :("); 
        
    Serial.println("Did not find fingerprint sensor :(");
    
    while (1) { delay(1); }
  }
}

uint8_t readnumber(void) {
  uint8_t num = 0;
  
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void loop()                     // run over and over again
{
  
  delay(3000);
  lcd.clear();
  lcd.print("Ready to enroll a");
  lcd.setCursor(0, 1);
  lcd.print("fingerprint"); 
  
  Serial.println("Ready to enroll a fingerprint!");
  
  for(int i = 0; i < 2; i++){
    delay(4000);
    lcd.clear();
    lcd.print("Please type in the ID #");
    lcd.setCursor(0, 1);
    lcd.print("(from 1 to 127)"); 
    delay(3000);
    lcd.clear();
    lcd.print("you want to save"); 
    lcd.setCursor(0, 1);
    lcd.print("this finger as"); 
  }

  delay(4000);
  lcd.clear();
  lcd.print("Please type in the ID #");
  lcd.setCursor(0, 1);
  lcd.print("(from 1 to 127)");
  
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");

  
  id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }

  delay(2000);
  lcd.clear();
  lcd.print("Enrolling ID #");
  lcd.setCursor(0, 1);
  lcd.print(id); 
  
  Serial.print("Enrolling ID #");
  Serial.println(id);
  
  while (!  getFingerprintEnroll() );
}

uint8_t getFingerprintEnroll() {

  int p = -1;

  delay(2000);
  lcd.clear();
  lcd.print("Waiting for valid");
  lcd.setCursor(0, 1);
  lcd.print("finger to enroll as #"); 

  delay(4000);
  lcd.clear();
  String idA = String(id);
  lcd.print("# - " + idA);
  
  Serial.print("Waiting for valid finger to enroll as #"); 
  Serial.println(id);
  
  
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
    
      delay(2000);
      lcd.clear();
      lcd.print("Image taken");
      
      Serial.println("Image taken");
      
      break;
    case FINGERPRINT_NOFINGER:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:

      delay(2000);
      lcd.clear();
      lcd.print("Communication error");
      
      Serial.println("Communication error");
      
      break;
    case FINGERPRINT_IMAGEFAIL:

      delay(2000);
      lcd.clear();
      lcd.print("Imaging error");
      
      Serial.println("Imaging error");
      
      break;
    default:
    
      delay(2000);
      lcd.clear();
      lcd.print("Unknown error");
      
      Serial.println("Unknown error");
      
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:

      delay(2000);
      lcd.clear();
      lcd.print("Image converted");
      
      Serial.println("Image converted");
      break;
      
    case FINGERPRINT_IMAGEMESS:

      delay(2000);
      lcd.clear();
      lcd.print("Image too messy");
      
      Serial.println("Image too messy");
      return p;
      
    case FINGERPRINT_PACKETRECIEVEERR:

      delay(2000);
      lcd.clear();
      lcd.print("Communication error");
      
      Serial.println("Communication error");
      return p;
      
    case FINGERPRINT_FEATUREFAIL:

      delay(2000);
      lcd.clear();
      lcd.print("Could not find finger-");
      lcd.setCursor(0, 1);
      lcd.print("-print features");
      
      Serial.println("Could not find fingerprint features");
      return p;
      
    case FINGERPRINT_INVALIDIMAGE:

      delay(2000);
      lcd.clear();
      lcd.print("Could not find finger-");
      lcd.setCursor(0, 1);
      lcd.print("-print features");
      
      Serial.println("Could not find fingerprint features");
      return p;
      
    default:

      delay(2000);
      lcd.clear();
      lcd.print("Unknown error");
      
      Serial.println("Unknown error");
      return p;
  }

  delay(2000);
  lcd.clear();
  lcd.print("Remove finger");
      
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  delay(2000);
  lcd.clear();
  String idB = String(id);
  lcd.print("ID: " + idB);
  
  Serial.print("ID "); Serial.println(id);
  p = -1;

  delay(2000);
  lcd.clear();
  lcd.print("Place same finger again");
  
  Serial.println("Place same finger again");
  
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:

      delay(2000);
      lcd.clear();
      lcd.print("Image taken");
  
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:

      delay(2000);
      lcd.clear();
      lcd.print("Communication error");
      
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:

      delay(2000);
      lcd.clear();
      lcd.print("Imaging error");
      
      Serial.println("Imaging error");
      break;
    default:

      delay(2000);
      lcd.clear();
      lcd.print("Unknown error");
      
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:

      delay(2000);
      lcd.clear();
      lcd.print("Image converted");
      
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:

      delay(2000);
      lcd.clear();
      lcd.print("Image too messy");
      
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:

      delay(2000);
      lcd.clear();
      lcd.print("Communication error");
      
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:

      delay(2000);
      lcd.clear();
      lcd.print("Could not find fingerprint features");
      
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:

      delay(2000);
      lcd.clear();
      lcd.print("Could not find fingerprint features");
      
      Serial.println("Could not find fingerprint features");
      return p;
    default:

      delay(2000);
      lcd.clear();
      lcd.print("Unknown error");
      
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  delay(2000);
  lcd.clear();
  lcd.print("Creating model for #");
  String idAB = String(id);
  lcd.print("ID: " + idAB);
  
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {

    delay(2000);
    lcd.clear();
    lcd.print("Prints matched!");
      
    Serial.println("Prints matched!");
    
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {

    delay(2000);
      lcd.clear();
      lcd.print("Communication error");
      
    Serial.println("Communication error");
    return p;
    
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {

    delay(2000);
      lcd.clear();
      lcd.print("Fingerprints did not match");
      
    Serial.println("Fingerprints did not match");
    return p;
    
  } else {

    delay(2000);
    lcd.clear();
    lcd.print("Unknown error");
      
    Serial.println("Unknown error");
    return p;
  }   


  delay(2000);
  lcd.clear();
  String idC = String(id);
  lcd.print("ID: " + idC);
  
  Serial.print("ID "); Serial.println(id);
  
  p = finger.storeModel(id);
  
  if (p == FINGERPRINT_OK) {

    delay(2000);
      lcd.clear();
      lcd.print("Stored!");
      
    Serial.println("Stored!");
    
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {

    delay(2000);
      lcd.clear();
      lcd.print("Communication error");
      
    Serial.println("Communication error");
    return p;
    
  } else if (p == FINGERPRINT_BADLOCATION) {

    delay(2000);
    lcd.clear();
    lcd.print("Could not store in that location");
      
    Serial.println("Could not store in that location");
    return p;
    
  } else if (p == FINGERPRINT_FLASHERR) {

    delay(2000);
    lcd.clear();
    lcd.print("Error writing to flash");
      
    Serial.println("Error writing to flash");
    return p;
    
  } else {

    delay(2000);
    lcd.clear();
    lcd.print("Unknown error");
      
    Serial.println("Unknown error");
    return p;
    
  }   
}
