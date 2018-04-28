#include <SPI.h>
#include <Wire.h>
#include <math.h>
#include <hcsr04.h>
/*
* Ultrasonic sensor program for controlling windows program
* Using Arduino and also Python library pyautogui
*/

/*
* I/O setup for Sensor
*/
const int trigger_ultraleft = 4; // trigger pin for 1st Ultrasonic sensors
const int echo_ultraleft = 3; // Echo pin 

/*
 *  setup global variable for speed
 */

HCSR04 ultra_left;
unsigned short distance_left;

void setup() {
  // put your setup code here, to run once:
  ultra_left.init(trigger_ultraleft, echo_ultraleft, 5); //setup pin for ultrasonic sensor
  Serial.begin(9600);
}

/*/
 * read distance function
 * setup the maxium ultransonic value to 40 to avoid data distorsion
 * using the readAvgDisctanceInCm from the hcsr04.h to get the value
 */
unsigned short f_distance(unsigned short readvalue)
{
  return (readvalue> 40) ? 40 : readvalue;
}

void loop() {
  // put your main code here, to run repeatedly:
  
  unsigned short dist_temp; //use temp variable to confirm the control mode 
                            //ultrasonic sensor can only work under the control mode

  Serial.println(ultra_left.readAvgDisctanceInCm(3));
  distance_left = f_distance(ultra_left.readAvgDisctanceInCm(3));
  if(distance_left >= 10 && distance_left <= 15)
  {
    delay(200);
    dist_temp = f_distance(ultra_left.readAvgDisctanceInCm(3));
    if( fabs(dist_temp - distance_left) <= 2) // in the control mode
    {
      Serial.println("control mode");
      distance_left = dist_temp;
      while(f_distance(ultra_left.readAvgDisctanceInCm(3)) < 35)
      {
        distance_left = f_distance(ultra_left.readAvgDisctanceInCm(3));
        if( distance_left < 9)
        {
          Serial.println("Volumedown"); 
          delay(250);
        }
        if( distance_left > 18)
        {
          Serial.println("Volumeup");
          delay(250);
        }
      }
    }
    
  }
}
