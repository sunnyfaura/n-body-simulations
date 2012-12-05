#include <iostream>
#include <stdio.h>
#include <vector>

#include "shader_helper.h"
#include "Polygon.h"
#include "header.h"

using namespace std;

const double SPF = 1.0/60.0;
const int MAX = 50;
const int CIRCLE_VERT = 20;
const double MASS_CONST = 0.001;
const double K_CONST = 0.01;
const double RAD_CONST = 0.05;

#define MAT_SET_TRANSLATE(M, x, y) { M[6] = (x); M[7] = (y); }

struct Particle{
	double x, y,
		   mass, xN, yN,
		   vx, vy;
};

Particle arr[MAX];

float fRand() {
	//return (float)rand()/RAND_MAX;
	return -1.0f + (GLfloat) rand()/ ((GLfloat) RAND_MAX/(2));
}

int main() {
	glfwInit();
	glfwOpenWindow(600, 600, 8, 8, 8, 8, 0, 0, GLFW_WINDOW);

	glewExperimental = GL_TRUE;
	glewInit();

	//generate more circles
	Polygon p(RAD_CONST, 0, 0.1, CIRCLE_VERT, 0xFFFF00FF);

	GLuint mainProgram;

	AttribBinding bindings[] = {
		{"p", Polygon::ATTR_POS},
		{"color", Polygon::ATTR_COLOR}
	};

	loadShaders(mainProgram, "simple.vsh", "simple.fsh", bindings, 2);
	const GLuint U_TRANSFORM = glGetUniformLocation(mainProgram, "transform");
	glUseProgram(mainProgram);

	GLfloat mat[] = {
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	};
	
	double t = 0;

	double prev = glfwGetTime();
	double acc = 0;

	//generate more of these coordinates with random initial position
	int i = 0;
	int j = 0;
	for(; i < MAX; i++){
			arr[i].x = fRand();
			arr[i].y = fRand();
			arr[i].mass = MASS_CONST; //fRand();
			arr[i].vx = 0.5;
			arr[i].vy = 0.5;
	}
	

	double aX = 0.0, aY = 0.0, dx = 0.0, dy = 0.0, inv = 0.0, inv3 = 0.0, force = 0.0, dir = 1;

	while ( glfwGetWindowParam( GLFW_OPENED ) && !glfwGetKey(GLFW_KEY_ESC) ) {
		const double cur = glfwGetTime();
		const double drawElapsed = cur - prev;
		prev = cur;

		acc += drawElapsed;
		
		//the idea here is that the x and y coordinates
		//follow the newtonian law of gravity
		//where F = ma = (k*m1*m2)/r^2
		//once a circle follows these rules
		//the uniform will pass a matrix to the vertex shader

		//Integrate the fnx

		//Repeat for each circle:
		//generate x and y coordinates by numerical approx
		//similar to Reimann's Sum

		while ( acc >= SPF ) {
			acc -= SPF;
			t += SPF; //UPDATE

			for(i = 0; i < MAX; i++)
			{
				aX = 0.0;
				aY = 0.0;

				for (j = 0; j < MAX; j++)
				{
					dx = arr[j].x - arr[i].x;
					dy = arr[j].y - arr[i].y;

					inv = 1.0/sqrt(dx*dx + dy*dy + RAD_CONST);
					inv3 = inv * inv * inv;

					force = arr[j].mass * inv3;
					aX += force*dx;
					aY += force*dy;
				}

				arr[i].xN = arr[i].x + (t*arr[i].vx) + (0.5*t*t*aX);
				arr[i].yN = arr[i].y + (t*arr[i].vy) + (0.5*t*t*aY);
				arr[i].vx += t*aX;
				arr[i].vy += t*aY;
			}

			for (i = 0; i < MAX; i++)
			{
				arr[i].x = arr[i].xN;
				arr[i].y = arr[i].yN;
			}

		}
			
			for(i = 0; i < MAX; i++)
			{
				if ( arr[i].x > 1 ) {
					arr[i].x *= 1;
					dir = -1;
				} else if ( arr[i].x < -1 ) {
					arr[i].x *= -1;
					dir = 1;
				}

				if ( arr[i].y > 1 ) {
					arr[i].y *= 1;
					dir = -1;
				} else if ( arr[i].y < -1 ) {
					arr[i].y *= -1;
					dir = 1;
				}
			}


		//draw
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);	

		for(i = 0; i < MAX; i++){
			MAT_SET_TRANSLATE(mat, arr[i].x, arr[i].y);
			glUniformMatrix3fv(U_TRANSFORM, 1, GL_FALSE, mat);
			p.draw(U_TRANSFORM,mat);
		}
		glfwSwapBuffers();			
	}	

	glfwTerminate();
	return 0;
}
