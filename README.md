# ZumoMazeRobot
The code takes snippets and learns from several examples in the existing Zumo32U4 Library, these are: BorderDetect, FaceTowardsOpponent, LineAndProximitySensors
To execute the program:

1. Download the RoboticsAssignmentFinal.ino from the repository
2. Install Arduino IDE with the most up to date version
3. In the Arduino IDE, Select Open Files and locate the .ino File
4. Install the required libraries
5. Connect the Zumo32U4 Robot to your PC via USB-C cable and select upload
The Zumo should now contain the software and can be unplugged from the PC

Scenario 1:
Place the Robot on the Maze and press the A Button to begin the procces.
The Robot should run after a countdown and continue on the white background and avoid any black lines in search of a house while traversing the maze through left/right/random turns,
when it detects a house the robot will beep and turn on the yellow LED and prepare to turn "away" from the house. 
Then, the proccess will continue and the object will attempt to look for the next house

Scenairio 2:
The robot will continue to traverse the maze, displayed on the LCD Screen will be an integer that outputs a user friendly message with the count of the houses visited, this is done in the code with every object detected by the proximity sensors being counted as it goes along and it will output the total houses visited so far.

Scenario 3:
No map of the maze was attempted in my code.
