#include <Wire.h>
#include <Zumo32U4.h>

// Defining the line sensor threshold to detect black lines and speed / delay of motors 
#define QTR_THRESHOLD     1000 
#define REVERSE_SPEED     100  
#define TURN_SPEED        100
#define FORWARD_SPEED     100
#define REVERSE_DURATION  200  
#define TURN_DURATION     200  

const uint8_t sensorThreshold = 6;

unsigned long lastTurnTime = 0; 
const unsigned long turnInterval = 30000;

// Counter to prevent the robot from being stuck in a left / right loop
int leftCount = 0;
int rightCount = 0;
int foundObject = 0;

Zumo32U4OLED display;
Zumo32U4ButtonA buttonA;
Zumo32U4Buzzer buzzer;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;

// Defines 3 line sensors: left, middle, right
#define NUM_SENSORS 3
unsigned int lineSensorValues[NUM_SENSORS];

// Buzzer and button function that awaits button A to be pressed before playing a countdown
void waitForButtonAndCountDown()
{
  ledYellow(1);
  display.clear();
  display.print(F("Press A"));
  buttonA.waitForButton();
  ledYellow(0);

  for (int i = 0; i < 3; i++)
  {
    delay(1000);
    buzzer.playNote(NOTE_G(3), 200, 15);
  }

  delay(1000);
  buzzer.playNote(NOTE_G(4), 500, 15);
  delay(1000);
}

// Drive function that makes the robot drive forwards
void drive()
{
  motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
}

// Turn right function that passes a parameter as an int that is used to increase the turn duration in the conditions set below
void turnRight(int i)
{
  motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
  delay(REVERSE_DURATION);
  motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
  delay(TURN_DURATION * i);
}

// Same as right function but for left turn
void turnLeft(int i)
{
  motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
  delay(REVERSE_DURATION);
  motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
  delay(TURN_DURATION * i);
}

// Stop function which is called when an object is detected, makes the robot stop and essentially attempt to spin around and avoid the object
void stop()
{
  motors.setSpeeds(0, 0);
  delay(1000);
  motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
  delay(REVERSE_DURATION);
  motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
  delay(TURN_DURATION * 10);
}

// random turn function that uses the current time and last turn time (predefined as 0 when the program executes) to perform a random left or right turn every 30 seconds
void randomTurn()
{
  unsigned long currentTime = millis(); 

  if (currentTime - lastTurnTime >= turnInterval)
  {
    int turnDirection = random(0, 2); 
    if (turnDirection == 0)
    {
      motors.setSpeeds(-TURN_SPEED, TURN_SPEED); 
      delay(TURN_DURATION * 4);
    } 

    else
    {
      motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
      delay(TURN_DURATION * 4);
    }

    lastTurnTime = currentTime;
  }
}

void setup()
{
  //configures the three proximity and line sensors and calls the wait for button function
  lineSensors.initThreeSensors();
  proxSensors.initThreeSensors();
  waitForButtonAndCountDown();
}

void loop()
{
  if (buttonA.isPressed())
  {
    motors.setSpeeds(0, 0);
    buttonA.waitForRelease();
    waitForButtonAndCountDown();
  }
  
  //calls the random turn function to execute in the loop
  randomTurn();

  lineSensors.read(lineSensorValues);

  //Condition if the leftmost sensor detects the threshold as over 1000 so black, and the count is under 3 for the turn it calls the turn right function with the paramater for turn duration as 1
  if (lineSensorValues[0] > QTR_THRESHOLD && rightCount <= 3)
  {
    turnRight(1);
    rightCount++;
  }

  //Condition if the righttmost sensor detects the threshold as over 1000 so black, and the count is under 3 for the turn it calls the turn left function with the paramater for turn duration as 1
  else if (lineSensorValues[NUM_SENSORS - 1] > QTR_THRESHOLD && leftCount <= 3)
  {
    turnLeft(1);
    leftCount++;
  }

  // As the counter is over 3 turns, it will make a greater turn as identified by a higher paramater than previously
  else if (lineSensorValues[0] > QTR_THRESHOLD && rightCount >= 3)
  {
    turnRight(5);
    rightCount = 0;
  }

  //same as above
  else if (lineSensorValues[NUM_SENSORS - 1] > QTR_THRESHOLD && leftCount >= 3)
  {
    turnLeft(5);
    leftCount = 0;
  }

  // in the case of the left and right sensors not detecting anything, the middle sensor is used to turn left and prevent the robot from overlapping any black lines
  else if (lineSensorValues[1] > QTR_THRESHOLD)
  {
    turnLeft(1);
  }

  else
  {
    drive();
  }

  // reads from the proximity sensors and if the values are higher than the threshold, it indicates an object is seen
  proxSensors.read();
  uint8_t leftValue = proxSensors.countsFrontWithLeftLeds();
  uint8_t rightValue = proxSensors.countsFrontWithRightLeds();

  bool objectSeen = leftValue >= sensorThreshold || rightValue >= sensorThreshold;

  // if object is seen, it flashes the yellow led and outputs the amount of houses that have been visited to the LCD Screen before executing the stop function
  if (objectSeen)
  {
    foundObject++;
    ledYellow(1);
    buzzer.playNote(NOTE_A(4), 2000, 15);
    display.print("You have found: ");
    display.print(foundObject);
    display.print(" houses");
    stop();
  }

  else
  {
    // as nothing is detected anymore, the led is turned off and the robot starts to drive again repeating the loop
    display.clear();
    ledYellow(0);
    drive();
  }
}


