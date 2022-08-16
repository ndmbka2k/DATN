#include <ArduinoJson.h>
#include "BlynkEdgent.h"
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Keypad.h>

#define BLYNK_TEMPLATE_ID "TMPLI1p5A5Ie"
#define BLYNK_DEVICE_NAME "SmarthomeV6"
#define BLYNK_AUTH_TOKEN "nsc1BVnOORMKynl7aekQ40UjZT_Rb3oj"
#define BLYNK_FIRMWARE_VERSION "0.1.0"
#define BLYNK_PRINT Serial
#define APP_DEBUG
BlynkTimer timer;

// màn hình cửa chính và living room
LiquidCrystal_I2C lcd1(0x3f, 16, 2);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo1;
Servo servo2;

int ii;
char pass[4] = {0, 0, 0, 0};
const byte ROWS = 4;
const byte COLS = 3;
char hexaKeys[ROWS][COLS] =
    {
        {'1', '2', '3'},
        {'4', '5', '6'},
        {'7', '8', '9'},
        {'*', '0', '#'}};

byte rowPins[ROWS] = {32, 33, 25, 26};
byte colPins[COLS] = {27, 14, 13};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

#define RXD2 16
#define TXD2 17

#define PIN_SERVO1 23
#define PIN_SERVO2 12
#define PIN_LEDPWM 23
#define PIN_STEP1 4
#define PIN_DIR1 15

byte fanpwm = 90, fanpwm1 = 0;
int temp1, temp2, humi1, humi2, gas, light, rain, pir1, pir2, ifr, rem;
int pwmled = 0;
boolean tam1 = 0, tam2 = 0;
boolen isProcessingWindow = false;
String inputString = "";
bool stringComplete = false;
byte vuong[] =
    {
        B01110,
        B01010,
        B01110,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000};
int latchPin = 18; // (11) ST_CP [RCK] on 74HC595
int clockPin = 19; // (9) SH_CP [SCK] on 74HC595
int dataPin = 5;   // (12) DS [S1] on 74HC595

int swlroom, sw2room;
boolean fan1, fan2, fan3, dh, warning, door1, lamp1, lamp2, lamp3, lamp4, lamp5, automode, oc = 0;
unsigned long outmap = 0b0000000000000000;

unsigned long onfan3 = 0b1000000000000000;
unsigned long offfan3 = 0b0111111111111111;

unsigned long onfan2 = 0b0100000000000000;
unsigned long offfan2 = 0b1011111111111111;

unsigned long onfan1 = 0b0010000000000000;
unsigned long offfan1 = 0b1101111111111111;

unsigned long onlamp1 = 0b0001000000000000;
unsigned long offlamp1 = 0b1110111111111111;

unsigned long onlamp5 = 0b0000100000000000;
unsigned long offlamp5 = 0b1111011111111111;

unsigned long onlamp3 = 0b0000010000000000;
unsigned long offlamp3 = 0b1111101111111111;

unsigned long onlamp4 = 0b0000001000000000;
unsigned long offlamp4 = 0b1111110111111111;

unsigned long onlamp2 = 0b0000000100000000;
unsigned long offlamp2 = 0b1111111011111111;

unsigned long ondh = 0b0000000010000000;
unsigned long offdh = 0b1111111101111111;
// coi bao dong
unsigned long onbuzz1 = 0b0000000001000000;
unsigned long offbuzz1 = 0b1111111110111111;
// coi gas
unsigned long onbuzz2 = 0b0000000000100000;
unsigned long offbuzz2 = 0b1111111111011111;
unsigned long times;
BLYNK_WRITE(V5)
{
  fan1 = param.asInt();
  if (fan1 == 1)
  {
    outmap = onfan1 | outmap;
    shiftwrite(outmap);
  }
  else if (fan1 == 0)
  {
    outmap = offfan1 & outmap;
    shiftwrite(outmap);
  }
}
BLYNK_WRITE(V6)
{
  fan2 = param.asInt();
  if (fan2 == 1)
  {
    outmap = onfan2 | outmap;
    shiftwrite(outmap);
  }
  else if (fan2 == 0)
  {
    outmap = offfan2 & outmap;
    shiftwrite(outmap);
  }
}
BLYNK_WRITE(V7)
{
  fan3 = param.asInt();
  if (fan3 == 1)
  {
    outmap = onfan3 | outmap;
    shiftwrite(outmap);
  }
  else if (fan3 == 0)
  {
    outmap = offfan3 & outmap;
    shiftwrite(outmap);
  }
}
BLYNK_WRITE(V8)
{
  lamp1 = param.asInt();
  if (lamp1 == 1)
  {
    outmap = onlamp1 | outmap;
    shiftwrite(outmap);
  }
  else if (lamp1 == 0)
  {
    outmap = offlamp1 & outmap;
    shiftwrite(outmap);
  }
}
BLYNK_WRITE(V9)
{
  lamp2 = param.asInt();
  if (lamp2 == 1)
  {
    outmap = onlamp2 | outmap;
    shiftwrite(outmap);
  }
  else if (lamp2 == 0)
  {
    outmap = offlamp2 & outmap;
    shiftwrite(outmap);
  }
}
BLYNK_WRITE(V10)
{
  lamp3 = param.asInt();
  if (lamp3 == 1)
  {
    outmap = onlamp3 | outmap;
    shiftwrite(outmap);
  }
  else if (lamp3 == 0)
  {
    outmap = offlamp3 & outmap;
    shiftwrite(outmap);
  }
}
BLYNK_WRITE(V11)
{
  lamp4 = param.asInt();
  if (lamp4 == 1)
  {
    outmap = onlamp4 | outmap;
    shiftwrite(outmap);
  }
  else if (lamp4 == 0)
  {
    outmap = offlamp4 & outmap;
    shiftwrite(outmap);
  }
}
BLYNK_WRITE(V12)
{
  door1 = param.asInt();
  servo1.attach(PIN_SERVO1);
  if (door1 == 1)
    servo1.write(100);
  else
    servo1.write(10);
}
BLYNK_WRITE(V13)
{
  boolean door2 = param.asInt();
  servo2.attach(PIN_SERVO2);
  if (door2 == 1)
    servo2.write(120);
  else
    servo2.write(10);
}
BLYNK_WRITE(V14)
{
  dh = param.asInt();
  if (dh == 1)
  {
    outmap = offdh & outmap;
    shiftwrite(outmap);
  }
  else if (dh == 0)
  {
    outmap = ondh | outmap;
    shiftwrite(outmap);
  }
}
BLYNK_WRITE(V15)
{
  automode = param.asInt();
}
BLYNK_WRITE(V16)
{
  // Nút nhấn trên Blink đóng mở rèm
  oc = param.asInt();
  if (automode == 0)
  {
    if (oc == 0 && tam1 == 0)
    {
      window(0);
      // Is closing
      tam1 = 1;
    }
    else if (oc == 1 && tam1 == 1)
    {
      window(1);
      // Is Opening
      tam1 = 0;
    }
  }
}
void setup()
{
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.begin(9600);
  ii = -1;
  servo1.attach(PIN_SERVO1);
  servo2.attach(PIN_SERVO2);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(PIN_STEP1, OUTPUT);
  pinMode(PIN_DIR1, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd1.init();
  lcd1.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Password: ");
  shiftwrite(0b0000000000000000);
  outmap = ondh | outmap;
  shiftwrite(outmap);
  BlynkEdgent.begin();
  timer.setInterval(3000, updateBlynk);
}

void loop()
{
  BlynkEdgent.run();
  timer.run();
  char customKey = customKeypad.getKey();
  if (customKey)
  {
    lcd.print("*");
    ii++;
    pass[ii] = customKey;
    if (ii > 2)
    {
      ii = -1;
      invailid();
    }
  }

  getdata();
  autowindow();
  readsw();
}
void getdata()
{
  if (Serial2.available())
  {
    char inChar = (char)Serial2.read();
    inputString += inChar;
    if (inChar == '\n')
    {
      stringComplete = true;
      autoo();
    }
    if (stringComplete)
    {
      int TimA, TimB = -1;
      TimA = inputString.indexOf("A");
      TimB = inputString.indexOf("B");
      if (TimA >= 0 && TimB >= 0)
      {
        String stemp1 = "";
        stemp1 = inputString.substring(TimA + 1, TimB);
        temp1 = stemp1.toInt();
      }
      int TimC = -1;
      TimB = inputString.indexOf("B");
      TimC = inputString.indexOf("C");
      if (TimB >= 0 && TimC >= 0)
      {
        String stemp2 = "";
        stemp2 = inputString.substring(TimB + 1, TimC);
        temp2 = stemp2.toInt();
      }
      int TimD = -1;
      TimC = inputString.indexOf("C");
      TimD = inputString.indexOf("D");
      if (TimC >= 0 && TimD >= 0)
      {
        String shumi1 = "";
        shumi1 = inputString.substring(TimC + 1, TimD);
        humi1 = shumi1.toInt();
      }
      int TimE = -1;
      TimD = inputString.indexOf("D");
      TimE = inputString.indexOf("E");
      if (TimC >= 0 && TimE >= 0)
      {
        String shumi2 = "";
        shumi2 = inputString.substring(TimD + 1, TimE);
        humi2 = shumi2.toInt();
      }
      int TimF = -1;
      TimE = inputString.indexOf("E");
      TimF = inputString.indexOf("F");
      if (TimE >= 0 && TimF >= 0)
      {
        String sgas = "";
        sgas = inputString.substring(TimE + 1, TimF);
        gas = sgas.toInt();
      }
      int TimG = -1;
      TimF = inputString.indexOf("F");
      TimG = inputString.indexOf("G");
      if (TimF >= 0 && TimG >= 0)
      {
        String slight = "";
        slight = inputString.substring(TimF + 1, TimG);
        light = slight.toInt();
      }
      int TimH = -1;
      TimG = inputString.indexOf("G");
      TimH = inputString.indexOf("H");
      if (TimG >= 0 && TimH >= 0)
      {
        String spir1 = "";
        spir1 = inputString.substring(TimG + 1, TimH);
        pir1 = spir1.toInt();
      }
      int TimI = -1;
      TimH = inputString.indexOf("H");
      TimI = inputString.indexOf("I");
      if (TimH >= 0 && TimI >= 0)
      {
        String spir2 = "";
        spir2 = inputString.substring(TimH + 1, TimI);
        pir2 = spir2.toInt();
      }
      int TimK = -1;
      TimI = inputString.indexOf("I");
      TimK = inputString.indexOf("K");
      if (TimI >= 0 && TimK >= 0)
      {
        String sifr = "";
        sifr = inputString.substring(TimI + 1, TimK);
        ifr = sifr.toInt();
      }
      int TimL = -1;
      TimK = inputString.indexOf("K");
      TimL = inputString.indexOf("L");
      if (TimK >= 0 && TimL >= 0)
      {
        String srem = "";
        srem = inputString.substring(TimK + 1, TimL);
        rem = srem.toInt();
      }
      Serial.println(inputString);
      inputString = "";
      stringComplete = false;
    }
  }
}
void updateBlynk()
{
  Blynk.virtualWrite(V0, temp1);
  Blynk.virtualWrite(V1, temp2);
  Blynk.virtualWrite(V2, humi1);
  Blynk.virtualWrite(V3, humi2);
  Blynk.virtualWrite(V4, gas);
  Blynk.virtualWrite(V5, fan1);
  Blynk.virtualWrite(V7, fan3);
  Blynk.virtualWrite(V8, lamp1);
  Blynk.virtualWrite(V10, lamp3);
  Blynk.virtualWrite(V16, !tam1);
}

void autowindow()
{
  // tam=0 : Close
  if (isProcessingWindow && automode == 1)
  {
    if (light > 90)
    {
      isProcessingWindow = true;
      window(0);
      // next State = Open
    }
    else if (light < 90)
    {
      isProcessingWindow = true;
      window(1);
      // next State = Close
      tam1 = 0;
    }
  }
}
void window(byte n)
{
  // n=1=>Open
  if (n == 1)
  {
    digitalWrite(PIN_DIR1, 0);
    for (int i = 0; i < 250; i++)
    {
      digitalWrite(PIN_STEP1, 1);
      delayMicroseconds(1200);
      digitalWrite(PIN_STEP1, 0);
      delayMicroseconds(1200);
    }
    isProcessingWindow = flase;
  }
  // n=0=>Close
  else if (n == 0)
  {
    digitalWrite(PIN_DIR1, 1);
    for (int i = 0; i < 250; i++)
    {
      digitalWrite(PIN_STEP1, 1);
      delayMicroseconds(1200);
      digitalWrite(PIN_STEP1, 0);
      delayMicroseconds(1200);
    }
    isProcessingWindow = flase;
  }
  // Dừng rèm đóng hoặc mở
  else if (n == 2)
  {
    digitalWrite(PIN_STEP1, 0);
  }
}
void shiftwrite(unsigned long n)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, n);
  shiftOut(dataPin, clockPin, LSBFIRST, n >> 8);
  digitalWrite(latchPin, HIGH);
}
void autoo()
{
  if (ifr == 0)

  // lamp2 đèn chân cầu thang
  {
    outmap = onlamp2 | outmap;
    shiftwrite(outmap);
  }
  else
  {
    outmap = offlamp2 & outmap;
    shiftwrite(outmap);
  }
  if (gas > 43 || temp1 > 43 || temp2 > 43 || pir2 == 1)
  {
    outmap = onbuzz2 | outmap;
    shiftwrite(outmap);
  }
  else
  {
    outmap = offbuzz2 & outmap;
    shiftwrite(outmap);
  }
  // Auto ở phòng xem phim khi nút nhấm được nhấn
  if (rem == 1)
  {
    lamp3 = 0;
    fan3 = 1;
    outmap = offlamp3 & outmap;
    shiftwrite(outmap);
    outmap = onfan3 | outmap;
    shiftwrite(outmap);
  }

  // Auto điều hòa???? Bỏ mẹ đi thì hợp lý hơn
  if (temp2 > 40)
  {
    outmap = ondh | outmap;
    shiftwrite(outmap);
  }
  lcd1.setCursor(0, 0);
  lcd1.print("T: ");
  lcd1.print(temp1);
  lcd1.createChar(0, vuong);
  lcd1.setCursor(5, 0);
  lcd1.write(0);
  lcd1.print("C ");
  lcd1.print("H: ");
  lcd1.print(humi1);
  lcd1.print(" %  ");
  lcd1.setCursor(0, 1);
  lcd1.print("BAO DONG: ");
  if (warning == 0)
    lcd1.print("OFF");
  else
    lcd1.print("ON  ");
}
void invailid()
{
  if (pass[0] == '1' && pass[1] == '1' && pass[2] == '1' && pass[3] == '1')
  {
    fan1 = 1;
    lamp1 = 1;
    outmap = onfan1 | outmap;
    shiftwrite(outmap);
    outmap = onlamp1 | outmap;
    shiftwrite(outmap);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("    Welcome");
    servo1.attach(PIN_SERVO1);
    servo1.write(100);
    delay(3000);
    lcd.clear();
    servo1.write(10);
    lcd.setCursor(0, 0);
    lcd.print("Password: ");
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Wrong password");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Password: ");
  }
}
void readsw()
{

  // nút nhấn nối tiếp, convert voltage để xem nút nào được nhấn
  swlroom = map(analogRead(39), 0, 2900, 0, 4);
  sw2room = map(analogRead(34), 0, 2900, 0, 4);
  if (swlroom == 1)
  {
    delay(25);
    swlroom = map(analogRead(39), 0, 2900, 0, 4);

    // đèn phòng khách
    if (swlroom == 1)
    {
      swlroom = map(analogRead(39), 0, 2900, 0, 4);
      while (swlroom == 1)
        swlroom = map(analogRead(39), 0, 2900, 0, 4);
      lamp1 = !lamp1;
      if (lamp1 == 1)
      {
        outmap = onlamp1 | outmap;
        shiftwrite(outmap);
      }
      else if (lamp1 == 0)
      {
        outmap = offlamp1 & outmap;
        shiftwrite(outmap);
      }
    }
  }

  // Cửa phòng khách
  if (swlroom == 2)
  {
    delay(25);
    swlroom = map(analogRead(39), 0, 2900, 0, 4);
    if (swlroom == 2)
    {
      swlroom = map(analogRead(39), 0, 2900, 0, 4);
      while (swlroom == 2)
        swlroom = map(analogRead(39), 0, 2900, 0, 4);
      servo1.attach(PIN_SERVO1);
      door1 = !door1;
      if (door1 == 1)
        servo1.write(120);
      else
        servo1.write(10);
    }
  }

  // Nút báo động phòng khách
  if (swlroom == 3)
  {
    delay(25);
    swlroom = map(analogRead(39), 0, 2900, 0, 4);
    if (swlroom == 3)
    {
      swlroom = map(analogRead(39), 0, 2900, 0, 4);
      while (swlroom == 3)
        swlroom = map(analogRead(39), 0, 2900, 0, 4);
      warning = !warning;
      if (warning == 1)
      {
        outmap = onbuzz1 | outmap;
        shiftwrite(outmap);
      }
      else if (warning == 0)
      {
        outmap = offbuzz1 & outmap;
        shiftwrite(outmap);
      }
    }
  }
  // Quạt phòng khách
  if (swlroom == 4)
  {
    delay(25);
    swlroom = map(analogRead(39), 0, 2900, 0, 4);
    if (swlroom == 4)
    {
      swlroom = map(analogRead(39), 0, 2900, 0, 4);
      while (swlroom == 4)
        swlroom = map(analogRead(39), 0, 2900, 0, 4);
      fan1 = !fan1;
      if (fan1 == 1)
      {
        outmap = onfan1 | outmap;
        shiftwrite(outmap);
      }
      else if (fan1 == 0)
      {
        outmap = offfan1 & outmap;
        shiftwrite(outmap);
      }
    }
  }
  // sw2

  // Đèn phòng bếp
  if (sw2room == 2)
  {
    delay(25);
    sw2room = map(analogRead(34), 0, 2900, 0, 4);
    if (sw2room == 2)
    {
      sw2room = map(analogRead(34), 0, 2900, 0, 4);
      while (sw2room == 2)
        sw2room = map(analogRead(34), 0, 2900, 0, 4);
      lamp4 = !lamp4;
      if (lamp4 == 1)
      {
        outmap = onlamp4 | outmap;
        shiftwrite(outmap);
      }
      else if (lamp4 == 0)
      {
        outmap = offlamp4 & outmap;
        shiftwrite(outmap);
      }
    }
  }

  if (sw2room == 1)
  {
    delay(25);
    sw2room = map(analogRead(34), 0, 2900, 0, 4);

    /// Quạt nhà WC
    if (sw2room == 1)
    {
      sw2room = map(analogRead(34), 0, 2900, 0, 4);
      while (sw2room == 1)
        sw2room = map(analogRead(34), 0, 2900, 0, 4);
      fan2 = !fan2;
      if (fan2 == 1)
      {
        outmap = onfan2 | outmap;
        shiftwrite(outmap);
      }
      else if (fan2 == 0)
      {
        outmap = offfan2 & outmap;
        shiftwrite(outmap);
      }
    }
  }

  // Đèn phòng ngủ
  if (sw2room == 3)
  {
    delay(25);
    sw2room = map(analogRead(34), 0, 2900, 0, 4);
    if (sw2room == 3)
    {
      sw2room = map(analogRead(34), 0, 2900, 0, 4);
      while (sw2room == 3)
        sw2room = map(analogRead(34), 0, 2900, 0, 4);
      lamp5 = !lamp5;
      if (lamp5 == 1)
      {
        outmap = onlamp5 | outmap;
        shiftwrite(outmap);
      }
      else if (lamp5 == 0)
      {
        outmap = offlamp5 & outmap;
        shiftwrite(outmap);
      }
    }
  }

  // điều hòa phòng ngủ
  if (sw2room == 4)
  {
    delay(25);
    sw2room = map(analogRead(34), 0, 2900, 0, 4);
    if (sw2room == 4)
    {
      sw2room = map(analogRead(34), 0, 2900, 0, 4);
      while (sw2room == 4)
        sw2room = map(analogRead(34), 0, 2900, 0, 4);
      dh = !dh;
      if (dh == 1)
      {
        outmap = ondh | outmap;
        shiftwrite(outmap);
      }
      else if (dh == 0)
      {
        outmap = offdh & outmap;
        shiftwrite(outmap);
      }
    }
  }
}
