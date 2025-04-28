#include <IRremote.h>

#define FORWARD 0xFF18E7
#define BACKWARD 0xFF4AB5
#define LEFT 0xFF10EF
#define RIGHT 0xFF5AA5
#define STOP 0xFF38C7

#define BUTTON1 0xFFA25D
#define BUTTON2 0xFF629D
#define BUTTON3 0xFFE21D
#define BACK 0xFF6897

#define IN1 9
#define IN2 8
#define IN3 7
#define IN4 6

#define SPEED1 5
#define SPEED2 10

#define SERVO 3

#define IR 12

#define ULTRASONIC_ECHO A1
#define ULTRASONIC_TRIG A2

#define IR_LINE_LEFT 2
#define IR_LINE_RIGHT 4

#define MIN_DISTANCE 15
#define MAX_DISTANCE 1000

/* IR Remote codes
  1 - FFA25D
  2 - FF629D
  3 - FFE21D
  4 - FF22DD
  5 - FF02FD
  6 - FFC23D
  7 - FFE01F
  8 - FFA857
  9 - FF906F
  * - FF6897
  0 - FF9867
  # - FFB04F
  ^ - FF18E7
  ˇ - FF4AB5
  < - FF10EF
  > - FF5AA5
  OK - FF38C7
*/

byte functionType = 0;

IRrecv irrecv(IR);
decode_results results;

byte movement;
unsigned long lastCommandTime = 0;
int lastMovementDirection;

int forward = FORWARD;
int backward = BACKWARD;
int left = LEFT;
int right = RIGHT;

int leftDistance, rightDistance;
int distance;
float duration;

int SR;
int SL;

void setup()
{

  Serial.begin(9600);

  irrecv.enableIRIn();

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(SPEED1, OUTPUT);
  pinMode(SPEED2, OUTPUT);
  pinMode(SERVO, OUTPUT);
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  pinMode(IR_LINE_LEFT, INPUT);
  pinMode(IR_LINE_RIGHT, INPUT);

  moveServo(90);
}

void loop()
{

  //Serial.println(functionType);

  if (irrecv.decode(&results))
  {
    //Serial.println(results.value, HEX);
    if (functionType == 0)
    {
      if (results.value == BUTTON1)
      {
        functionType = 1;
      }
      else if (results.value == BUTTON2)
      {
        functionType = 2;
      }
      else if (results.value == BUTTON3)
      {
        functionType = 3;
      }
      irrecv.resume();
    }
/*     else
    {
      if (results.value == BACK)
      {
        functionType = 0;
        moveServo(90);
        irrecv.resume();
        return;
      }
    } */
    
  }

  if (functionType == 1) //IR remote mode
  {
    if (irrecv.decode(&results))
    {
      //Serial.println(results.value, HEX);

      if (results.value == BACK)
      {
        functionType = 0;
        moveServo(90);
        irrecv.resume();
        Serial.println("Back - IR");
        return;
      }

      if (results.value != 0xFFFFFFFF)
      {
        lastMovementDirection = results.value;
      }
      
      if (results.value == 0xFFFFFFFF)
      {
        if (lastMovementDirection == forward)
        {
          movement = 1;
          lastCommandTime = millis();
        }
        else if (lastMovementDirection == backward)
        {
          movement = 2;
          lastCommandTime = millis();
        }
        else if (lastMovementDirection == left)
        {
          movement = 3;
          lastCommandTime = millis();
        }
        else if (lastMovementDirection == right)
        {
          movement = 4;
          lastCommandTime = millis();
        }
      }
      irrecv.resume();
    }

    if (movement == 1 && millis() - lastCommandTime < 200)
    {
      Forward(0);
    }
    else if (movement == 2 && millis() - lastCommandTime < 200)
    {
      Backward(0);
    }
    else if (movement == 3 && millis() - lastCommandTime < 200)
    {
      Left(0);
    }
    else if (movement == 4 && millis() - lastCommandTime < 200)
    {
      Right(0);
    }
    else
    {
      Stop();
      movement = 0;
    }
  }
  else if (functionType == 2) //Obstacle avoidance mode
  {

    if (irrecv.decode(&results))
    {
      if (results.value == BACK)
      {
        functionType = 0;
        moveServo(90);
        Serial.println("Back - Ultrasonic");
        return;
      }
      irrecv.resume();
    }

    CheckDistance();

    if (distance >= MIN_DISTANCE && distance < MAX_DISTANCE)
    {
      Forward(0);
      moveServo(90);
    }
    else
    {
      Stop();
      Backward(400);
      Stop();


      CheckLeftSide();
      CheckDistance();
      
      if (distance > MAX_DISTANCE)
      {
        Left(300);
        Stop();
        return;
      }
      else
      {
        leftDistance = distance;
      }

      delay(1000);

      CheckRightSide();
      CheckDistance();

      if (distance > MAX_DISTANCE)
      {
        Right(300);
        Stop();
        return;
      }
      else
      {
        rightDistance = distance;
      }

      delay(1000);

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

      /* Serial.print("Left:");
      Serial.println(leftDistance);
      Serial.print("Right:");
      Serial.println(rightDistance); */
    }
  }
  else if (functionType == 3) //Line tracking mode
  {

    if (irrecv.decode(&results))
    {
      if (results.value == BACK)
      {
        functionType = 0;
        moveServo(90);
        Serial.println("Back - Line");
        return;
      }
      irrecv.resume();
    }

    SL = digitalRead(IR_LINE_LEFT);
    SR = digitalRead(IR_LINE_RIGHT);

    if (SL == LOW && SR==LOW)
    { 
      Forward(0);
    }        
    else
    {  
      if (SL == HIGH & SR == LOW)
      {  
        Right(0);
      }
      else if (SR == HIGH & SL == LOW)
      {  
        Left(0);  
      }
      else
      {    
        Stop();
      }
    }
  }

  /*if (!IS_IR)
  {
    CheckDistance();

    if (distance >= MIN_DISTANCE && distance < MAX_DISTANCE)
    {
      Forward(0);
      moveServo(90);
    }
    else
    {
      Stop();
      Backward(400);
      Stop();


      CheckLeftSide();
      CheckDistance();
      
      if (distance > MAX_DISTANCE)
      {
        Left(300);
        Stop();
        return;
      }
      else
      {
        leftDistance = distance;
      }


      
      delay(1000);

      CheckRightSide();
      CheckDistance();

      if (distance > MAX_DISTANCE)
      {
        Right(300);
        Stop();
        return;
      }
      else
      {
        rightDistance = distance;
      }

      delay(1000);

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

      Serial.print("Left:");
      Serial.println(leftDistance);
      Serial.print("Right:");
      Serial.println(rightDistance);
    }
  }
  else
  {

    IR movement
    if (irrecv.decode(&results)) {
      if (results.value == FORWARD) {
        Forward(0);
      } else if (results.value == BACKWARD) {
        Backward(0);
      } else if (results.value == LEFT) {
        Left(0);
      } else if (results.value == RIGHT) {
        Right(0);
      } else if (results.value == STOP) {
        Stop();
      } else {
        results.value = 0;
      }

      irrecv.resume();
    }
  }*/

  

}

#pragma region Obstacle_avoidance

void moveServo(int angle)
{
  int pulseWidth = map(angle, 0, 180, 1000, 2000);
  digitalWrite(SERVO, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(SERVO, LOW);
  delay(20);
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

#pragma endregion Obstacle_avoidance

#pragma region Movement

void Forward(int duration)
{
  analogWrite(SPEED1, 100);
  analogWrite(SPEED2, 100);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(duration);
}

void Backward(int duration)
{
  analogWrite(SPEED1, 100);
  analogWrite(SPEED2, 100);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(duration);
}

void Stop()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

int Left(int duration)
{
  analogWrite(SPEED1, 150);
  analogWrite(SPEED2, 150);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(duration);
}

int Right(int duration)
{
  analogWrite(SPEED1, 150);
  analogWrite(SPEED2, 150);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(duration);
}
#pragma endregion Movement
