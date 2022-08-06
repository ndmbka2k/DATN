#include <ArduinoJson.h>
#include <SoftwareSerial.h>
SoftwareSerial serialsend(2, 3);
#include <DHT.h>
DHT dht1(4, DHT11);
DHT dht2(5, DHT11);
int temp1,temp2,humi1,humi2,gas,rain,pir1,pir2,ifr,rem;

// rem là biến rèm phòng chiếu phim
unsigned int light;
boolean tam1,tam2=0;
byte cnt=0;
String A = "A";
String B = "B";
String C = "C";
String D = "D";
String E = "E";
String F = "F";
String G = "G";
String H = "H";
String I = "I";
String K = "K";
String L = "L";
String SendESP32 = "";
long times;
byte vuong[] = 
{
  B01110,
  B01010,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
LiquidCrystal_I2C lcd1(0x26,16,2);
//input
#define PIN_GAS     A7
#define PIN_LIGHT   A3
#define PIN_RAIN    A6
#define PIN_PIR1    A2
#define PIN_PIR2    A1
#define PIN_IFR     A0
#define PIN_SW      11      // cong tac cho dong mo rem phong chieu phium
//output
#define PIN_DIR2    6
#define PIN_STEP2   7
#define PIN_DIR3    9
#define PIN_STEP3   8
#define PIN_LEDPWM     10
void setup() 
{
    rem=0;
    Serial.begin(9600);
    serialsend.begin(9600);
    delay(200);
    pinMode(PIN_PIR1,INPUT); pinMode(PIN_PIR2,INPUT); pinMode(PIN_IFR,INPUT); pinMode(PIN_SW,INPUT_PULLUP);
    
    pinMode(PIN_DIR2,OUTPUT); pinMode(PIN_STEP2,OUTPUT); pinMode(PIN_DIR3,OUTPUT); pinMode(PIN_STEP3,OUTPUT); pinMode(PIN_LEDPWM,OUTPUT);
    dht1.begin();
    dht2.begin(); 
    lcd.init();
    lcd.backlight();
    lcd1.init();
    lcd1.noBacklight();
    lcd1.setCursor(0,0);
    lcd1.print("TURN OFF TV");
}
void loop() 
{
  //Send data to esp32
  read_sensor();
  send_data();

  // nut nhan o phong chieu phim, auto dong rem, bat quat, bat tv
  if(digitalRead(PIN_SW)==0)
  {
    delay(25);
    if(digitalRead(PIN_SW)==0)
    {
      while(digitalRead(PIN_SW)==0)
      rem=!rem;
      lcd1.setCursor(0,0);
      if(rem==0)  
      {
        lcd1.noBacklight();
        lcd1.print("TURN OFF TV");
      }
      else
      {  
        lcd1.backlight();      
        lcd1.print("TURN ON TV ");
      }
    }
  }
  autowindow();
  autodp();
}
void send_data()
{
  cnt++;    

// Để tạo tgian delay cho UART
  if (cnt>10)
  {
    SendESP32 = A + temp1 + B + temp2 + C + humi1 + D + humi2 + E + gas + F + light + G + pir1 + H + pir2 + I + ifr + K + rem + L;
    serialsend.println(SendESP32);
    serialsend.flush();
    cnt=0;
  }
          
}
void read_sensor()
{
    // nhiệt độ trong nhà
    temp1 = dht1.readTemperature();
    // nhiệt độ phòng ngủ
    temp2 = dht2.readTemperature();
    // nhiệt độ trong nhà
    humi1 = dht1.readHumidity();
    // nhiệt độ phòng ngủ
    humi2 = dht2.readHumidity();
    lcd.setCursor(0,0);
    lcd.print("T: "); lcd.print(temp2);lcd.createChar(0,vuong);lcd.setCursor(5,0);lcd.write(0);lcd.print("C ");lcd.print("H: "); lcd.print(humi2);lcd.print(" %  ");
    
    // khí gas trong bếp
    gas   = (analogRead(PIN_GAS)*0.0978);       // đổi từ analog 0 4095 to %
    
    // dây phơi
    rain  = analogRead(PIN_RAIN);
    
    // Cảm biến ánh sáng để bật đèn theo thời thiết
    light = map(analogRead(PIN_LIGHT),0,1024,0,100);

    // Cam bien chuyen dong
    // ngoai san
    pir1  = digitalRead(PIN_PIR1);
    // trong bep
    pir2  = digitalRead(PIN_PIR2);

    // Cảm biến hồng ngoại phòng khách
    ifr   = digitalRead(PIN_IFR);
    if(pir1==1) analogWrite(PIN_LEDPWM,light);
    else        digitalWrite(PIN_LEDPWM,0);
    //Serial.println(rain);
}
void danphoi(boolean n)
{
  if(n==0)
  {
     //dong
      (PIN_DIR2,1);
      // test để tính tgian mở rèm từ 0->full
    for(int i=0;i<90;i++)
    {
      digitalWrite(PIN_STEP2,1);
      delayMicroseconds(1600);
      digitalWrite(PIN_STEP2,0);
      delayMicroseconds(1600);
    }
  }
  if(n==1)
  {
     //mo
    digitalWrite(PIN_DIR2,0);
    // tgian quay
    for(int i=0;i<90;i++)
    {
      digitalWrite(PIN_STEP2,1);
      // tốc độ quay
      delayMicroseconds(1600);
      digitalWrite(PIN_STEP2,0);
      delayMicroseconds(1600);
    }
  }
}
void autodp()
{
  //neu co mua
  if(rain>600&&tam2==0)
  {
    danphoi(false);
    tam2=1;
  }
  else if(rain<500&&tam2==1)
  {
    danphoi(true);
    tam2=0;
  }
}
void autowindow()
{
  if(rem==1&&tam1==0)
  {
    window(0);
    tam1=1;
  }
  else if(rem==0&&tam1==1)
  {
    window(1);
    tam1=0;
  }
}
void window(byte n)
{
  //n=1=>Open
  if(n==1)
  {
    digitalWrite(PIN_DIR3,1);
    for(int i=0;i<250;i++)
    {
      digitalWrite(PIN_STEP3,1);
      delayMicroseconds(1200);
      digitalWrite(PIN_STEP3,0);
      delayMicroseconds(1200);
    }
  }
  //n=0=>Close
  else if(n==0)
  {
    digitalWrite(PIN_DIR3,0);
    for(int i=0;i<250;i++)
    {
      digitalWrite(PIN_STEP3,1);
      delayMicroseconds(1200);
      digitalWrite(PIN_STEP3,0);
      delayMicroseconds(1200);
    }
  }
}
