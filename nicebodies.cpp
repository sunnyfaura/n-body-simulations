#include <iostream>
#include <stdio.h>
#include <vector>

#include "shader_helper.h"
#include "Polygon.h"
#include "header.h"

using namespace std;

bool verlet = false;

const double SPF = 1.0/60.0;
double G = 0.00001; 
const int MAX = 500;
const int CIRCLE_VERT = 20;
const double MASS_CONST = 1.0;
const double K_CONST = 5.0;



//xPos = 2 * xPos - xPrevPos + a * dt * dt;
//		xPrevPos = xPos;

#define MAT_SET_TRANSLATE(M, x, y) { M[6] = (x); M[7] = (y); }

struct Particle{
	double x, y, m;
	double xnew, ynew;
	double xprev, yprev;
	//double znew;
	double vx, vy;
	//double vz;x
};

Particle arr[MAX];

float fRand() {
	return (float)rand()/RAND_MAX;
	//return -1.0f + (float) rand()/ RAND_MAX/2;
}

int main() {
	glfwInit();

	glfwOpenWindow(600, 600, 8, 8, 8, 8, 0, 0, GLFW_WINDOW);

	glewExperimental = GL_TRUE;
	glewInit();

	Polygon p(0.01, 0, 0.1, CIRCLE_VERT, 0x0000FF00);

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
	double dir = 1;

	//generate more of these coordinates with random initial position
	int i = 0, j = 0;
	double dx = 0.0, dy = 0.0;// dz = 0.0;
	double ax = 0.0, ay = 0.0;// az = 0.0;
	double f = 0.0, invr = 0.0, invr3 = 0.0;


	//particle initialization
	for(; i < MAX; i++){
			arr[i].x = fRand() - 0.5;
			arr[i].y = fRand() - 0.5;
			arr[i].xprev = arr[i].x;
			arr[i].yprev = arr[i].y;
			arr[i].m = fRand();	
	}


	if (verlet)
		G = 0.00001;
	else
		G = 0.00001;

	while ( glfwGetWindowParam( GLFW_OPENED ) && !glfwGetKey(GLFW_KEY_ESC) ) {
		const double cur = glfwGetTime();
		const double drawElapsed = cur - prev;
		prev = cur;

		acc += drawElapsed;

		while ( acc >= SPF ) {
			acc -= SPF;
			//update
			t += SPF;

			for(i = 0; i < MAX; i++){
				ax = 0.0;
				ay = 0.0;
				//az = 0.0;
				for(j = 0; j < MAX; j++){
					dx = arr[j].x - arr[i].x;
					dy = arr[j].y - arr[i].y;
					//dz = arr[j].z - arr[i].z;

					invr = 1.0/sqrt(dx*dx + dy*dy + 0.5);					
					invr3 = invr * invr * invr;
					f = -(arr[j].m*invr3*G);

					//accumulate the acceleration from g attraction
					ax += f*dx;
					ay += f*dy;
					//az += f*dz;
				}
				//position update
				if ( !verlet ) {
					arr[i].xnew = arr[i].x + (t*arr[i].vx) + 0.5*t*t*ax;
					arr[i].ynew = arr[i].y + (t*arr[i].vy) + 0.5*t*t*ay;
				}
				else
				{	
					arr[i].xnew += ((2 * arr[i].xnew) - arr[i].xprev + (ax*SPF*SPF))*0.05;
					arr[i].ynew += ((2 * arr[i].ynew) - arr[i].yprev + (ay*SPF*SPF))*0.05;
				}
				
				//arr[i].znew = arr[i].z + (t*arr[i].vz) + 0.5*t*t*az;



				//velocity update
				arr[i].vx += t*ax;
				arr[i].vy += t*ay;
				//arr[i].vz += t*az;
			}

			for(i = 0; i < MAX; i++ ){

				if (verlet)
				{
					arr[i].xprev = arr[i].x;
					arr[i].yprev = arr[i].y;
				}	

				arr[i].x = arr[i].xnew;
				arr[i].y = arr[i].ynew;

				//arr[i].z = arr[i].znew;

				if(arr[i].x > 1) 
					arr[i].vx *= -1;
				if(arr[i].x < -1) 
					arr[i].vx *= -1;
				if(arr[i].y > 1) 
					arr[i].vy *= -1;
				if(arr[i].y < -1) 
					arr[i].vy *= -1;
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
