#ifndef CLOCK_HANDS_H
#define CLOCK_HANDS_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <string.h>
#include <stddef.h>
#include<iostream>

const float hour_hand_length = 150.0;
const float minute_hand_length = 210.0;

int charToInt(char ch) {
	int i = ch -'0';
	return i;
}
// draws clock hands, and day of month indicator
void draw_hands(float centre_x, float centre_y, std::string colour) {
  
  float r = 0; // red
  float g = 0; // green
  float b = 0; // blue
  
  if (colour.compare("black") ==0) {
    r = 0; g = 0; b = 0;
  } else if (colour.compare("white") ==0) {
    r = 1; g = 1; b = 1;
  } else if (colour.compare("red") ==0) {
    r = 1; g = 0; b = 0;
  } else if (colour.compare("green") ==0) {
    r = 0; g = 1; b = 0;
  } else if (colour.compare("blue") ==0) {
    r = 0; g = 0; b = 1;
  }

  const float PI = 3.141592654;
  const float number_step  = PI/6; // angle between numbers on the clock
  
  time_t rawtime;
  time (&rawtime); // current time
  struct tm * timeinfo  = localtime(&rawtime);
  char * time = ctime(&rawtime);

  mktime(timeinfo);
  
  // extract info from tm struct
  int hour = timeinfo->tm_hour;
	int minute = timeinfo->tm_min;
  int seconds = timeinfo->tm_sec;

	// convert to 12h
	if (hour>=12) {
		hour -= 12;
	}

  // calculate hand end coordinates
	float fraction_past_hour = (float) minute/60.0;
	float hour_hand_angle = (hour + fraction_past_hour)*number_step;
	float minute_hand_angle = ((float)minute)/5.0 * number_step;
 
  // rotation matrix defines anticlockwise rotation; (2pi - angle) required 
  // rotation relative to origin
  float hour_hand_x = 0*cos(2*PI-hour_hand_angle) - hour_hand_length*sin(2*PI-hour_hand_angle); 
  float hour_hand_y = hour_hand_length*cos(2*PI-hour_hand_angle) + 0* sin(2*PI-hour_hand_angle);  
  float minute_hand_x = 0*cos(2*PI-minute_hand_angle) - minute_hand_length*sin(2*PI-minute_hand_angle);
  float minute_hand_y = minute_hand_length*cos(2*PI-minute_hand_angle) + 0*sin(2*PI-minute_hand_angle);
  
  // add centre coordinate to hand endpoints
  hour_hand_x += centre_x;
  hour_hand_y += centre_y;
  minute_hand_x += centre_x;
  minute_hand_y += centre_y;
  
  glMatrixMode (GL_MODELVIEW);
  glPushMatrix ();
  
  glColor3f(r, g, b);
  
  glBegin(GL_LINES);
    glVertex2f(centre_x, centre_y); // starting point says the same
    glPointSize(10); // increase enpoint size
    glVertex2f(hour_hand_x, hour_hand_y);
    glVertex2f(centre_x, centre_y); // starting point stays the same 
    glPointSize(10);
    glVertex2f(minute_hand_x, minute_hand_y);
  glEnd();
  
  glPointSize(10); // increase enpoint size
  
  glBegin(GL_POINTS);
    glColor3f(r, g, b);
    glVertex2f(hour_hand_x, hour_hand_y);
    glVertex2f(minute_hand_x, minute_hand_y);
  glEnd();
  
  glPopMatrix();
  
  glPointSize(1.0);
  
  // draw day of month
  std::ostringstream day_of_month_stream;
  day_of_month_stream << timeinfo->tm_mday;
  std::string day_of_month_str(day_of_month_stream.str());
  draw_text(625, centre_y,0.25, r, g, b, day_of_month_str);
  

}

#endif

std::string date_string() {
  std::string month_strings[12] = {"January", "Feburary", "March",
   "April", "May", "June", "July", "August", "September", "October", "November", "December"};
  time_t rawtime;
  time (&rawtime); // current time
  char * time = ctime(&rawtime);
  struct tm* timeinfo = localtime(&rawtime);
  std::ostringstream date_stream;
  date_stream << timeinfo->tm_mday << " " << month_strings[timeinfo->tm_mon]
  << " " << timeinfo->tm_year+1900;
  std::string date(date_stream.str());
  
  return date;
}