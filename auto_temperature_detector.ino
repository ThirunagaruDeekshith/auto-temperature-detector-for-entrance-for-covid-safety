#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MLX90614.h> //headers
const int ldr1 = A0;
bool ldr1state;
const int ldr2 = 8; //ldr pins
const int buzz = 10; //buzzer pin
int pc = 0;
int pmax = 5;
int tmax = 40; //limits for people and temperature
String BT_input; //bluetooth input
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards
int pos = 0;    // variable to store the servo position
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
void setup() {
  // put your setup code here, to run once:
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.begin (16, 2);
  lcd.setBacklight(HIGH); //Lighting backlight
  lcd.home ();
  mlx.begin();
  myservo.attach(9);
  Serial.begin(9600);
  pinMode(buzz, OUTPUT);
 
  lcd.setCursor(0, 0);
  lcd.print("Auto Temperature");
  lcd.setCursor(0, 1);
  lcd.print("Detector");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("By Batch 07");
  delay(1000);
  lcd.clear();
  lcd.print("start ");
}
void bluetooth() {
  unsigned long time1 = millis() + 2000;
  while (millis() < time1)
  {
    if (Serial.available()) {
      BT_input = Serial.readString();   // read input string from bluetooth

      // Serial.println(BT_input);
      tmax = (BT_input[0] - 48) * 10 + (BT_input[1] - 48);
      pmax = (BT_input[2] - 48) * 10 + (BT_input[3] - 48);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("values set");
      break;
    }
  }
}
void buzzer() {
  tone(buzz, 1000); // Send 1KHz sound signal...
  delay(1000);        // ...for 1 sec
  noTone(buzz);

}
//void scroll(){
//for (int positionCounter = 0; positionCounter < 6; positionCounter++) {
// scroll one position left:
//lcd.scrollDisplayLeft();
//wait a bit:
//delay(200);
//}
//}
void gate() {
  for (pos = 0; pos <= 100; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  delay(1000);
  for (pos = 100; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}
void tcheck() {
  lcd.setCursor(0, 0);
  lcd.print("Ambient ");
  lcd.print(mlx.readAmbientTempC());
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Target  ");
  lcd.print(mlx.readObjectTempC());
  lcd.print(" C");
  if (mlx.readObjectTempC() >= tmax)
  { lcd.clear();
    lcd.setCursor(0, 0); lcd.print("temperature");
    lcd.setCursor(0, 1);
    lcd.print("exceeded");
    // scroll();
    buzzer();
    loop();
  }
  else
  {  
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("please Enter");
    myservo.write(0);
    delay(100);
    gate();
  }

}
void ldr_1() {
  int ldrStatus = analogRead(ldr1);
  Serial.println(ldrStatus);
  if (ldrStatus <= 700) {
    ldr1state = LOW;
    Serial.println(ldr1state);
  }
  else {
    ldr1state = HIGH;
    Serial.println(ldr1state);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  bluetooth();
  Serial.println(pmax);
  Serial.println(tmax);
  ldr_1();
  if (ldr1state == HIGH && digitalRead(ldr2) == LOW )
  { tcheck();
    Serial.println("inc");
    unsigned long int time = millis() + 3000;
    while (millis() < time)
    {
      if (digitalRead(ldr2) == HIGH ) {
        pc = pc + 1;
        Serial.println(pc);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("People inroom:" + (String)pc);
        // scroll();
        break;
      }
    }

  }
  else {
    ldr_1();
    if (digitalRead(ldr2) == HIGH && ldr1state == LOW )
    {
      Serial.println("dec");
      lcd.clear();
    lcd.setCursor(0, 0);
      lcd.print("Thank You");
      gate();
      unsigned long int time = millis() + 3000;
      while (millis() <= time) {
        ldr_1();
        if (ldr1state == HIGH ) {
          pc == 0 ? 0 : pc = pc - 1;
          Serial.println(pc);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("People inroom: "+(String)pc);
          //   scroll();
          break;
        }
      }
    }
  }
  if (pc >= pmax)
  { lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("room is full:"+(String)pc);
    lcd.setCursor(0, 1);
    lcd.println("please wait..,");
    //scroll();
    buzzer();
  }
}