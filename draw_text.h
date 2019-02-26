#ifndef DRAW_TEXT_H
#define DRAW_TEXT_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <string>
#include <string.h>
#include <stddef.h>

// assumes a window of 1000 by 1000
void draw_text(
		const int x_pos,
		const int y_pos,
    const float scale,
    const float r,
    const float g,
    const float b,
		const char* text)
{
	//const float scale = 0.25f;

	glColor3f(r, g, b);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, 1000, 0, 1000);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();
			glTranslatef(x_pos, y_pos, 0.0f);
			glScalef(scale, scale, 1.0f);
    		size_t len = strlen(text);
    		for (size_t i=0;i<len;i++)
       			glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
		glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void draw_text(
		const int x_pos,
		const int y_pos,
    const float scale,
    const float r,
    const float g,
    const float b,
		std::string str)
{
	//const float scale = 0.25f;
	glColor3f(r, g, b);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, 1000, 0, 1000);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();
			glTranslatef(x_pos, y_pos, 0.0f);
			glScalef(scale, scale, 1.0f);
    		size_t len = str.length();
    		for (size_t i=0;i<len;i++)
       			glutStrokeCharacter(GLUT_STROKE_ROMAN, str.at(i));
		glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

#endif
