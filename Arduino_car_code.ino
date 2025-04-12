//#include <IRremote.h>
//#include <Servo.h>

#define FORWARD 0xFF18E7
#define BACKWARD 0xFF4AB5
#define LEFT 0xFF10EF
#define RIGHT 0xFF5AA5
#define STOP 0xFF38C7

#define SERVO_PIN 3

#define ULTRASONIC_ECHO A1
#define ULTRASONIC_TRIG A2

#define MIN_DISTANCE 25

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

  //moveServo(-90);
}

void loop()
{
  
  CheckDistance();

  if (distance >= MIN_DISTANCE)
  {
    Forward(1);
    moveServo(90);
  }
  else
  {
    Stop();
    //Left();
    //delay(1100);

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

      if (leftDistance < rightDistance)
      {
        Left(1);
        delay(400);
      }
      else
      {
        Right(1);
        delay(400);
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
  distance = duration *0.034/2;
}

void CheckLeftSide()
{
  moveServo(270);
}

void CheckRightSide()
{
  moveServo(-90);
}

void Forward(int duration)
{
  analogWrite(speed1, 100);
  analogWrite(speed2, 100);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(duration * 100);
}

void Backward(int duration)
{
  analogWrite(speed1, 100);
  analogWrite(speed2, 100);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(duration * 100);
}

void Stop()
{
  analogWrite(speed1, 0);
  analogWrite(speed2, 0);
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
  delay(duration * 100);
}

int Right(int duration)
{
  analogWrite(speed1, 150);
  analogWrite(speed2, 150);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(duration * 100);
}
