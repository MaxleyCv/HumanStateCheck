#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Adafruit_ADXL345_U.h>

float g1, g2, g3;

sensors_vec_t acceleration G;

enum Case
{
  stand, 
  walk,
  intenswalk,
  
}

int MainCase = 0;


int r = 8, g = 9, b = 10;


Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);


float VectLength(float a1, float a2, float a3)
{
  return sqrt(pow(a1,2) + pow(a2,2) +pow(a3,2));
}

bool CheckEquality(float a, float b, float deviation)
{
  if(abs(a-b) <= deviation) return true;
  else return false;
}

void CheckCase(float Min, float Max)
{
  if(CheckEquality(Min, 9.8, 1.5)&&CheckEquality(Max, 11, 1.5)) MainCase = 2;
  else if(CheckEquality(Min, 9, 2.5)&&CheckEquality(Max, 14, 3.5)) MainCase = 1;
  else if(CheckEquality(Min, 7, 3)&&CheckEquality(Max, 22, 5.5)) MainCase = 3;
  else if(CheckEquality(Min, 4, 2) && CheckEquality(Max, 25, 6)) MainCase = 4;
  else if(CheckEquality(Min, 0, 2) && CheckEquality(Max, 3, 3)) MainCase = 5;
}

void CheckBase()
{
  unsigned long TimeStart = millis();
  int minAcceleration, maxAcceleration;
  sensors_event_t ac;
  accel.getEvent(&ac);
  maxAcceleration = VectLength(ac.acceleration.x, ac.acceleration.y, ac.acceleration.z);
  minAcceleration = VectLength(ac.acceleration.x, ac.acceleration.y, ac.acceleration.z);
  while(millis()-TimeStart<=800)
  {
    sensors_event_t acc;
    accel.getEvent(&acc);
    float a = VectLength(acc.acceleration.x, acc.acceleration.y, acc.acceleration.z);
    if(a < minAcceleration) minAcceleration = a;
    else if(a > maxAcceleration) maxAcceleration = a;
  }
  CheckCase(minAcceleration, maxAcceleration);
}

bool CheckStand()
{
  sensors_event_t acc;
  accel.getEvent(&acc);
  float a1, a2, a3;
  a1 = acc.acceleration.x;
  a2 = acc.acceleration.y;
  a3 = acc.acceleration.z;
  float a = VectLength(a1,a2,a3);
  float g = VectLength(g1,g2,g3);
  float q = VectLength(a1+g1,a2+g2,a3+g3);
  double alfa = acos(double((pow(q,2)-pow(g,2)-pow(a,2))/(2*a*g)));
  if(alfa > 3.14/4&&alfa<3.14/4*3) 
      return false;
  else 
      return true;
}

void setup() 
{
  pinMode(r,OUTPUT);
  pinMode(g,OUTPUT);
  pinMode(b,OUTPUT);
  Serial.begin(9600);
  accel.begin();
  sensors_event_t acc;
  accel.getEvent(&acc);
  g1 = acc.acceleration.x;
  g2 = acc.acceleration.y;
  g3 = acc.acceleration.z;
}

void loop() 
{
  int val[3];
  CheckBase();
  switch(MainCase)
  {
    case 2:
    {
      if(CheckStand()) {
        val[0] = 128;
        val[1] = 0;              //Standing
        val[2] = 128;
        }
      else {
        val[0] = 128;
        val[1] = 128;              // Lying
        val[2] = 0;
        }
      break;
    }
    case 1:
    {
        val[0] = 0;
        val[1] = 255;           // Walking
        val[2] = 0;
      break;
    }
    case 3:
    {
        val[0] = 0;
        val[1] = 0;           //Jumping
        val[2] = 255;
      break;
    }
    case 4:
    {
        val[0] = 0;
        val[1] = 0;        // Running
        val[2] = 255;
      break;
    }
    case 5:
    {
        val[0] = 255;
        val[1] = 0;         // Falling
        val[2] = 0;
        break;
    }
  }
  analogWrite(r, val[0]);
  analogWrite(g, val[1]);
  analogWrite(b, val[2]);
  
}
