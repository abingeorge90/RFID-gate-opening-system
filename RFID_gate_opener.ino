//abin

#include <SPI.h>
#include <MFRC522.h>
#include <OnewireKeypad.h>
#include <Servo.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo servo;
int servoPos = 0;

#define sensorPin1 A2
#define sensorPin2 A3
#define buzzerPin 6

int senVal1 = 0;
int senVal2 = 0;

#define RST_PIN 8
#define SS_PIN 5

int card1Balance = 0;
int card2Balance = 400;

#define num 7
char Data[num];
byte data_count = 0;

String num1, num2, card, card2;
int a, b;
char Key;

bool recharge = true;

MFRC522 mfrc522(SS_PIN, RST_PIN);

int state = 0;

char KEYS[] = {
  '1', '2', '3', 'A',
  '4', '5', '6', 'B',
  '7', '8', '9', 'C',
  '*', '0', '#', 'D'
};

OnewireKeypad <Print, 16 > KP2(Serial, KEYS, 4, 4, A0, 4700, 1000 );

void setup () {
 lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  servo.attach(9);

  servo.write(90);

  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  pinMode(buzzerPin, OUTPUT);

  KP2.SetKeypadVoltage(5.0);

  SPI.begin();
  mfrc522.PCD_Init();

  lcd.setCursor(0, 0);
  lcd.print(" Automatic gate");
  lcd.setCursor(0, 1);
  lcd.print("opening system");
  delay(3000);
  lcd.clear();
}
void loop()
{

  if (recharge == 0)
  {
    reCharge();
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("   Welcome!!!");
    sensorRead();
    rfid();
    KeyPad();
    if (senVal1 == 0)
    {
      servoDown();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vehicle detected");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Place your card");
      lcd.setCursor(0, 1);
      lcd.print("on the reader...");
      delay(2000);
      lcd.clear();

    }
    else if (senVal2 == 0 && state == 1)
    {
      servoUp();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Have a great ");
      lcd.setCursor(0, 1);
      lcd.print("  Day..  ");
      delay(2000);
      lcd.clear();
      state = 0;
      servoDown();
      delay(5);
    }
  }
}
void sensorRead()
{
  senVal1 = digitalRead(sensorPin1);
  senVal2 = digitalRead(sensorPin2);
}

void servoDown()
{
  servo.attach(3);
  for (servoPos = 90; servoPos <= 180; servoPos += 1)
  {
    servo.write(servoPos);
    delay(5);
  }
}

void servoUp()
{
  servo.attach(3);
  for (servoPos = 180; servoPos >= 90; servoPos -= 1)
  {
    servo.write(servoPos);
    delay(10);
  }
}



void rfid()
{
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();

  if (content.substring(1) == "14 C2 53 88")
  {
    if (card1Balance >= 0)
    {
      lcdPrint();
      card1Balance = card1Balance ;
      lcd.setCursor(9, 1);
      lcd.print(" Abin ");
      delay(2000);
      lcd.clear();
      state = 1;
    }
    else
    {
      card = content.substring(1);
      LcdPrint();
      lcd.setCursor(9, 1);
      lcd.print(" _ ");
      lcd.print(" ");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" ");
      delay(1000);
      lcd.clear();
      state = 0;
    }
  }
 
  

  else   {
    digitalWrite(buzzerPin, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("Unknown Vehicle");
    lcd.setCursor(0, 1);
    lcd.print("Access denied");
    delay(1500);
    lcd.clear();
    digitalWrite(buzzerPin, LOW);
  }
}

void KeyPad()
{
  byte KState = KP2.Key_State();

  if (KState == PRESSED)
  {
    Key = KP2.Getkey();
    if (Key)
    {
      if (Key == 'A')
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("...");
        lcd.setCursor(0, 1);
        lcd.print("................");
        delay(1500);
        lcd.clear();
        recharge = 0;
      }
    }
  }
}

void clearData()
{
  while (data_count != 0)
  {
    Data[data_count--] = 0;
  }
  return;
}



void reCharge()
{

  lcd.setCursor(0, 0);
  lcd.print ("...");

  byte KState = KP2.Key_State();

  if (KState == PRESSED)
  {
    Key = KP2.Getkey();
    if (Key)
    {
      if (Key == 'D')
      {
        if (card == "14 C2 53 88")
        {
          num1 = Data;
          card1Balance = num1.toInt() + card1Balance;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(" _ ");
          lcd.setCursor(0, 1);
          lcd.print(" ");
          lcd.setCursor(9, 1);
          lcd.print ("_");
          lcd.print(" . ");
          delay(3000);
          clearData();
          lcd.clear();
          recharge = 1;
        }
        
      }
      else
      {
        Data[data_count] = Key;
        lcd.setCursor(data_count, 1);
        lcd.print(Data[data_count]);
        data_count++;
      }
    }
  }
}
void lcdPrint()
{
  digitalWrite(buzzerPin, HIGH);
  delay(200);
  digitalWrite(buzzerPin, LOW);
  delay(100);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Authorized");
  lcd.setCursor(0, 1);
  lcd.print(" Access");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome");
  lcd.setCursor(0, 1);
  lcd.print("Home: ");
}

void LcdPrint()
{
  digitalWrite(buzzerPin, HIGH);
  delay(200);
  digitalWrite(buzzerPin, LOW);
  delay(100);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" ..");
  lcd.setCursor(0, 1);
  lcd.print(" ...");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("'''");
  lcd.setCursor(0, 1);
  lcd.print("... ");
}
