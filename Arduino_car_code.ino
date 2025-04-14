//#include <IRremote.h>
//#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define FORWARD 0xFF18E7
#define BACKWARD 0xFF4AB5
#define LEFT 0xFF10EF
#define RIGHT 0xFF5AA5
#define STOP 0xFF38C7

#define SERVO_PIN 3

#define ULTRASONIC_ECHO A1
#define ULTRASONIC_TRIG A2

#define MIN_DISTANCE 15
#define MAX_DISTANCE 900

LiquidCrystal_I2C lcd(0x27, 2, 16);

/* IR variables
  const int RemotePin = 12;
  IRrecv irrecv(RemotePin);
  decode_results results;
*/

int in1 = 9;
int in2 = 8;
int in3 = 7;
int in4 = 6;

int speed1 = 5;
int speed2 = 10;

bool checking = false;

int leftDistance, rightDistance;

int distance;
float duration;

void moveServo(int angle)
{
  int pulseWidth = map(angle, 0, 180, 1000, 2000);
  digitalWrite(SERVO_PIN, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(SERVO_PIN, LOW);
  delay(20);
}

void setup()
{

  Serial.begin(9600);

  //irrecv.enableIRIn(); //IR setup

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(speed1, OUTPUT);
  pinMode(speed2, OUTPUT);
  pinMode(SERVO_PIN, OUTPUT);
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);

  moveServo(90);

  lcd.init();
  lcd.backlight();

  //moveServo(-90);
}

void loop()
{
  
  CheckDistance();

  if (distance > MAX_DISTANCE)
  {
    distance = 0;
  }

  if (distance >= MIN_DISTANCE)
  {
    Forward(100);
    moveServo(90);

    lcd.init();
    lcd.setCursor(0,0);
    lcd.print("Distance: ");
    lcd.setCursor(10,0);
    lcd.print(distance);
    lcd.setCursor(13,0);
    lcd.print("cm");

  }
  else
  {
    Stop();
    //Left();
    //delay(1100);

    Backward(600);
    Stop();

    checking = true;

    if (checking)
    {
      CheckLeftSide();
      CheckDistance();
      leftDistance = distance;
      delay(1000);

      CheckRightSide();
      CheckDistance();
      rightDistance = distance;
      delay(1000);

      lcd.init();

      lcd.setCursor(0,0);
      lcd.print("Left: ");
      lcd.setCursor(6,0);
      lcd.print(leftDistance);
      lcd.setCursor(9,0);
      lcd.print("cm");

      lcd.setCursor(0,1);
      lcd.print("Right: ");
      lcd.setCursor(7,1);
      lcd.print(rightDistance);
      lcd.setCursor(10,1);
      lcd.print("cm");

      if (leftDistance > 500)
      {
        Left(100);
        Stop();
      }

      if (rightDistance > 500)
      {
        Right(100);
        Stop();
      }

      if (leftDistance < rightDistance)
      {
        Left(400);
        Stop();
      }
      else
      {
        Right(400);
        Stop();
      }

      checking = false;
    }

    Serial.print("Left:");
    Serial.println(leftDistance);
    Serial.print("Right:");
    Serial.println(rightDistance);
  }

/*  IR movement
  if (irrecv.decode(&results)) {
    if (results.value == FORWARD) {
      Forward();
    } else if (results.value == BACKWARD) {
      Backward();
    } else if (results.value == LEFT) {
      Left();
    } else if (results.value == RIGHT) {
      Right();
    } else if (results.value == STOP) {
      Stop();
    }

    irrecv.resume();
  }
*/

}

void CheckDistance()
{
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(1);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(5);
  digitalWrite(ULTRASONIC_TRIG, LOW);

  duration = pulseIn(ULTRASONIC_ECHO , HIGH);
  distance = duration*0.034/2;
}

void CheckLeftSide()
{
  moveServo(180);
}

void CheckRightSide()
{
  moveServo(0);
}

void Forward(int duration)
{
  analogWrite(speed1, 100);
  analogWrite(speed2, 100);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(duration);
}

void Backward(int duration)
{
  analogWrite(speed1, 100);
  analogWrite(speed2, 100);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(duration);
}

void Stop()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

int Left(int duration)
{
  analogWrite(speed1, 150);
  analogWrite(speed2, 150);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(duration);
}

int Right(int duration)
{
  analogWrite(speed1, 150);
  analogWrite(speed2, 150);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(duration);
}
