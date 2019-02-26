#ifdef __APPLE__
#include <GLUT/glut.h> 
#else
#include <GL/glut.h> 
#endif

#include <cmath>
#include<string>
#include<iostream>
#include<sstream>
#include <stdio.h> 
#include <time.h>
#include <unistd.h>
#include "draw_text.h" 
#include "clock_hands.h" 
#include "load_and_bind_texture.h"

// window size
const int window_size_x = 1000;
const int window_size_y = 1000;
const int window_centre_x = window_size_x/2;
const int window_centre_y = window_size_y/2;

// analog clock circle
const float radius = 250.0f; // clock circle radius
const float pi = atan(1)*4; // define pi
const size_t steps = 64; // how many steps once round the circle
const float theta_step = 2*pi/float(steps); // angle step in radians

const float number_step = pi/6;
const float number_radius = radius-30.0f;
float number_xoffset = -20.0f;
float number_yoffset = -50.0f;

int mode = 1; // program mode
int sw_mode = 1; // stopwatch mode
bool roman = false; // roman numerals toggle

//calendar
int month_change = 0;

//stopwatch
std::string lastTime;
struct timespec start, current, stop;
int display_hour;
int display_minute;
int display_second;
int display_mil_second;
unsigned long pauseTime = 0;
unsigned long pauseAdd = 0;

//texture
unsigned int g_the_tex = 0; // texture handle
unsigned int texture_handle = load_and_bind_texture("./images/wood-grain.png");
std::string hand_colour = "black";

// redraw callback
void display()
{
  glClear(GL_COLOR_BUFFER_BIT); 
	
  if (mode < 5){ // not on 'help' page
  //instructions
    glDisable(GL_LINE_SMOOTH);
    draw_text(10, 950, 0.2,1,1,1, "1: Analog clock");
    draw_text(10, 900, 0.2,1,1,1, "2: Digital clock");
    draw_text(10, 850, 0.2,1,1,1, "3: Stopwatch");
    draw_text(700, 950, 0.25,1,0,0, "Press 5 for help");
    glEnable(GL_LINE_SMOOTH);
  }  
  
  if (mode == 1) { // analog clock
    
    if (texture_handle !=0 ) { // texture applied 
      glColor3f(1.0f, 1.0f, 1.0f); // white (background)
    } else {
      glColor3f(0.64f, 0.55f, 0.27f); // green
    }
    
   	// enable texturing
  	glEnable(GL_TEXTURE_2D); 

	  // select which texure to render
  	glBindTexture(GL_TEXTURE_2D, g_the_tex); 
    
    float theta = 0.0f;
    
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();
      glLoadIdentity();
      glTranslatef(window_centre_x, window_centre_y, 0.0f);
      
      // draw clock circle
      glBegin(GL_POLYGON); // connected vertices
          
    	  // however many times once round the circle
    	  for (size_t i=0;i<steps;i++, theta+=theta_step) {
    		  float x = radius * cos(theta);
    		  float y = radius * sin(theta);
          glVertex2f(x, y);
          glTexCoord2f(cos(theta)*0.5+0.5, sin(theta)*0.5+0.5); // texture coordinates [0,1]
        }
      glColor3f(0.6,0.16,0.16);
    	glEnd();
     
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
   
    // draw numbers on clock
    // drawn anticlockwise from 3 o'clock
    
    std::string clock_numbers[12] = {"3", "2", "1", "12", "11", "10", "9", "8", "7", "6", "5", "4"};
    std::string clock_numerals[12] = {"III", "II", "I", "XII", "XI", "X", "IX", "VIII", "VII", "VI", "V", "IV"};
    
    float number_angle = 0.0f;
    
    for (int i = 0; i< 12; i++) {
      float x = (radius-30) * cos(number_angle);
      float y = (radius-30) * sin(number_angle);
      if (mode == 1 && !roman) {
        draw_text(x+window_centre_x-10, y+window_centre_y-10,
        0.25, 1, 1, 1, clock_numbers[i]);
      } else if (mode == 1 && roman) {
        glDisable(GL_LINE_SMOOTH);
        draw_text(x+window_centre_x-10, y+window_centre_y-10,
        0.25, 1, 1, 1, clock_numerals[i]);
      }
      number_angle += number_step;
    }
    
    glEnable(GL_LINE_SMOOTH);
    
    // add hands based on system time
    draw_hands(window_centre_x, window_centre_y, hand_colour);
    
    draw_text(400, 150, 0.25,1,1,1, date_string());
       
    
  } else if (mode == 2) { // digital clock and calendar
  
    glEnable(GL_LINE_SMOOTH);

    //title
    draw_text(350, 700, 0.75, 1, 1, 1, "Time");
                    
    time_t rawtime;
    time (&rawtime); // current time
    struct tm * timeinfo = localtime(&rawtime);    
    char * time_array = ctime(&rawtime);    
        
    // digital clock
    std::string d_clock;    
    for (int i = 0; i<24; i++) { // clear other time string chars
      if (i >= 11 && i <=18) {
        d_clock.append(1, time_array[i]);
      }
    }
    draw_text(300, 500, 0.75, 1, 1, 1, d_clock);

    timeinfo->tm_mon += month_change; // change month from current time
    mktime(timeinfo); // update time information
    
    // draw calendar
    float calendar_x = 325.0;
    float calendar_y = 350.0;            
        
    int days_from_first = 0;
    int diff_from_weekday = 0;
    int first_weekday_of_month = 0;
    int days_in_month = 0;
    
    float first_day_offset;
    
    int day_of_month = timeinfo->tm_mday;
        
    std::string day_of_week_str;
    int day_of_week = timeinfo->tm_wday;
    if (day_of_week ==0) {
      day_of_week = 7;
    }
    
    
    // record month
    std::string month_strings[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    std::string month_str = month_strings[timeinfo->tm_mon];
    
    //record year
    int year = timeinfo->tm_year + 1900;
    std::ostringstream year_stream;
    year_stream << year;
    std::string year_str(year_stream.str());
    
    //calculate days in month
    if (month_str.compare("Jan")==0
     || month_str.compare("Mar")==0
     || month_str.compare("May")==0
     || month_str.compare("Jul")==0
     || month_str.compare("Aug")==0
     || month_str.compare("Oct")==0
     || month_str.compare("Dec")==0) {
      days_in_month = 31;
    } else if (month_str.compare("Apr")==0
     || month_str.compare("Jun")==0
     || month_str.compare("Sep")==0
     || month_str.compare("Nov")==0) {
      days_in_month = 30;
    } else if (month_str.compare("Feb")==0) {
      if (year % 4 == 0) { // leap year
        days_in_month = 29; 
      } else {
        days_in_month = 28;
      }
    }
    
    days_from_first = day_of_month - 1;
    diff_from_weekday = days_from_first % 7;
    first_weekday_of_month = day_of_week - diff_from_weekday;
    if (first_weekday_of_month <= 0) {
    	first_weekday_of_month += 7;
    }
    
    
    // print calendar headings
    glDisable(GL_LINE_SMOOTH);
    draw_text(calendar_x+100, calendar_y+50, 0.25, 1, 1, 1, month_str);
    draw_text(calendar_x+175, calendar_y+50, 0.25, 1, 1, 1, year_str);
    draw_text(calendar_x, calendar_y, 0.2, 1, 1, 1, "Mo Tu We Th Fr Sa Su");
    
    // offset first day of month on cal display
    first_day_offset = 10 +(first_weekday_of_month-1)*50;
    int xoffset = first_day_offset;
    int yoffset = -40;
    
    // print first row of days of month
    int counter = 1;
    for (int i = first_weekday_of_month; i<=7; i++) {
      std::ostringstream number;
      number << counter;
      std::string num_string = number.str();
 	    if (counter == day_of_month) { // print current day red
        draw_text(calendar_x + xoffset, calendar_y + yoffset, 0.2, 1, 0, 0, num_string);
      } else {
        draw_text(calendar_x + xoffset, calendar_y + yoffset, 0.2, 1, 1, 1, num_string);
      }
      xoffset += 50; // shift for next num in row
      counter++;
    }
    
    int num_in_first_row = counter - 1;
    
    // draw other rows
    yoffset -= 50;
    xoffset = 10; // start from Mon for next row
    for (int i = 1; i <= days_in_month - num_in_first_row; i++) {
      std::ostringstream number;
      number << counter;
      std::string num_string = number.str();
      if (counter == day_of_month && month_change ==0) { // print  today red
        draw_text(calendar_x + xoffset, calendar_y + yoffset, 0.2, 1, 0, 0, num_string);
      } else {
        draw_text(calendar_x + xoffset, calendar_y + yoffset, 0.2, 1, 1, 1, num_string);
      }
      xoffset += 50; 
      counter++;
      if (i%7 ==0) { // end of week/ next row
        yoffset -= 50;
        xoffset = 0;
      }
    }
    
    glEnable(GL_LINE_SMOOTH);
  
  } else if (mode == 3) { // stopwatch
    
    //Heading
    draw_text(350, 900, 0.5, 1,1,1, "Stopwatch");
    draw_text(230, 600, 0.3, 1,1,0, "Hs  : Mins : Secs : Mils");
    
    //instructions
    glDisable(GL_LINE_SMOOTH);
    draw_text(800, 450, 0.2,1,1,1, "'s': start/stop");
    draw_text(800, 400, 0.2,1,1,1, "'r': reset");
    glEnable(GL_LINE_SMOOTH);
    
    if (sw_mode ==1) { // start the stopwatch

      clock_gettime(CLOCK_MONOTONIC_RAW, &start);
      sw_mode = 2; // run
    
    } else if (sw_mode == 2) { // stopwatch running
    
      clock_gettime(CLOCK_MONOTONIC_RAW, &current);
      
      // difference in sec between current and start times
      unsigned long long elapsed_microseconds = 
      (current.tv_sec * 1000000 + current.tv_nsec/1000) - (start.tv_sec * 1000000 + start.tv_nsec/1000) - pauseTime;
      unsigned long elapsed_milliseconds = elapsed_microseconds/1000.0f;
      float elapsed_seconds = elapsed_milliseconds/1000.0f;
      float elapsed_minutes = elapsed_seconds/60.0f;
      float elapsed_hours = elapsed_minutes/60.0f;
          
      
      // only display up to 60 mins or secs etc. 
      display_mil_second = int(floor(elapsed_milliseconds))%1000;
      display_second = int(floor(elapsed_seconds))%60;
      display_minute = int(floor(elapsed_minutes))%60;
      display_hour = int(floor(elapsed_hours));
      
      std::ostringstream display_stream;
      display_stream << display_hour << " : " << display_minute << " : " << display_second << " : " << display_mil_second ;
      std::string display_string(display_stream.str());
      lastTime.assign(display_string); // store the latest elapsed time
      draw_text(230, 500, 0.5, 1, 1, 1, display_string); // print digital time
            
    
    } else if (sw_mode == 3) { //stopped
      
      // store difference between now and when last running (time paused in millisecs)
      clock_gettime(CLOCK_MONOTONIC_RAW, &stop);
      pauseAdd = (stop.tv_sec * 1000000 + stop.tv_nsec/1000) - (current.tv_sec * 1000000 + current.tv_nsec/1000);      
      
      draw_text(230, 500, 0.5, 1, 0, 0, lastTime); // draw latest elapsed time
      draw_text(475, 400, 0.2, 1, 0, 0, "Paused"); 
      
    } else if (sw_mode == 4) { //reset
    
      sw_mode = 1; // start
      draw_text(230, 500, 0.5, 1, 1, 1, "0 : 0 : 0");
    }
  } else if (mode ==5) { // help page 
    
    glDisable(GL_LINE_SMOOTH);
    draw_text(10, 900, 0.5,1,1,1, "1: Analog clock");
    draw_text(50, 850, 0.3,1,1,1, "Numbering");
    draw_text(70, 800, 0.3,1,1,1, "- Press r for roman numerals");
    draw_text(70, 750, 0.3,1,1,1, "- Press a for arabic numbering");
    draw_text(50, 700, 0.3,1,1,1, "Clock face");
    draw_text(70, 650, 0.3,1,1,1, "A: Magical swirl");
    draw_text(70, 600, 0.3,1,1,1, "B: Star wars");
    draw_text(70, 550, 0.3,1,1,1, "C: Mountains");
    draw_text(70, 500, 0.3,1,1,1, "D: Plain light blue");
    draw_text(10, 400, 0.5,1,1,1, "2: Digital clock and calendar");
    draw_text(70, 350, 0.3,1,1,1, "- Use left/right arrow keys to change month");
    draw_text(10, 250, 0.5,1,1,1, "3: Stopwatch"); 
    draw_text(20, 200, 0.3,1,1,1, "- Press s to start/stop");  
    draw_text(20, 150, 0.3,1,1,1, "- Press r to reset");
    draw_text(10, 75, 0.5,1,1,1, "q: Quit");   
    glEnable(GL_LINE_SMOOTH);
    
  }
 
  glutSwapBuffers(); // swap the backbuffer with the front
}


void init() {

  glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, window_size_x, 0, window_size_y); 
  glPopMatrix();
  
  glEnable(GL_LINE_SMOOTH);
  g_the_tex = texture_handle;
}

void keyboard(unsigned char key, int, int) {
	std::cerr << "\t you pressed the " << key << " key" << std::endl;

	switch (key) {
		case 'q': exit(1); // quit!
    case '1': mode = 1; break;
    case '2': mode = 2; month_change = 0; break;
    case '3': mode = 3; break; 
    case '5': mode = 5; break;
    case 's': if (mode ==3) { // stopwatch
                if (sw_mode == 2) { // if sw is running
                  sw_mode = 3; // pause
                } else if (sw_mode == 3) { // if sw currently paused
                  pauseTime += pauseAdd; // add pause time to total pause time
                  sw_mode = 2; // resume
                }
              }
              break;
    case 'r': if (mode ==3) { // reset stopwatch
                sw_mode = 4;
                pauseTime = 0; 
              } else if (mode==1) { // if on analog clock mode
                roman = true; // roman numerals
              }  break;
    case 'a': if (mode==1) {
                roman = false;
              } break;
    case 'A': if (mode==1) {
                texture_handle = load_and_bind_texture("./images/blue-swirl.png");
                hand_colour.assign("white");
                init();
              } break;
    case 'B': if (mode==1) {
                texture_handle = load_and_bind_texture("./images/star_wars_logo.png");
                hand_colour.assign("green"); 
                init();
              } break;          
    case 'C': if (mode==1) {
                texture_handle = load_and_bind_texture("./images/mountain.png");
                hand_colour.assign("white");
                init();
              } break;
    case 'D': if (mode==1) {
                texture_handle = 0; // no texture
                hand_colour.assign("black");
                init();
              } break;          

	}

	glutPostRedisplay(); // force a redraw
}

void special(int key, int, int)
{
	// handle special keys
	switch (key)
	{
		case GLUT_KEY_LEFT: if (mode==2){ // show previous month on calendar
                          month_change -= 1;                          
                        } break;
		case GLUT_KEY_RIGHT: if (mode==2){
                           month_change += 1;
                         } break;
	}

	glutPostRedisplay(); // force a redraw
}

void idle()
{
	glutPostRedisplay();
}

void visibility(int vis) {
	if (vis==GLUT_VISIBLE) {
		std::cerr << "\t visible" << std::endl;
		glutIdleFunc(idle);
	} else {
 
		std::cerr << "\t NOT visible" << std::endl;
		glutIdleFunc(NULL);
	}
}



int main(int argc, char* argv[])
{
  
  
	glutInit(&argc, argv); 
	
	// set the GL display mode: double buffering and RGBA colour mode
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA); // flags bitwise OR'd together
 
  	
	glutInitWindowSize(512, 512); // window size
	glutInitWindowPosition(50, 50); // where on screen

  glutCreateWindow("Clock"); 

	glutDisplayFunc(display); // set display callback
 
  glClearColor(0.47, 0.7, 0.65, 0.0); // clear colour 
	
	glutKeyboardFunc(keyboard); 
  glutSpecialFunc(special); 
	glutVisibilityFunc(visibility);
 
  init();
	glutMainLoop(); // go into the main loop and wait for window events...

	return 0; 
}