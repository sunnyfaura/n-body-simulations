#include <iostream>
#include <stdio.h>
#include <vector>

#include "shader_helper.h"
#include "Polygon.h"
#include "header.h"

using namespace std;

const double SPF = 1.0/60.0;
const double bounceSpeed = 0.01;
const int MAX = 50;
const int CIRCLE_VERT = 20;
const double MASS_CONST = 1.0;
const double K_CONST = 5.0;

#define MAT_SET_TRANSLATE(M, x, y) { M[6] = (x); M[7] = (y); }

/**
GRAVITATIONAL FORCE = k ( m1 , m2 ) / r * r
r is the distance between the bodies (IS A VECTOR) 


**/

struct Particle{
	double x, y, currPosX, currPosY,
		   mass, accelX, accelY,
		   force;

};

Particle arr[MAX];

float fRand() {
	return (float)rand()/RAND_MAX;
	//return -1.0f + (GLfloat) rand()/ ((GLfloat) RAND_MAX/(2));
}
1
int main() {
	glfwInit();

	glfwOpenWindow(600, 600, 8, 8, 8, 8, 0, 0, GLFW_WINDOW);

	glewExperimental = GL_TRUE;
	glewInit();

	//generate more circles
	

	Polygon p(0.05, 0, 0.1, CIRCLE_VERT, 0xFFFF00FF);

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
	int i = 0;
	for(; i < MAX; i++){
			arr[i].x = fRand() - 0.5;
			arr[i].y = fRand() - 0.5;
			arr[i].mass = fRand() - 0.5;	
	}
	
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

		double unitX, unitY, distX, distY, magDist;
		while ( acc >= SPF ) {
			acc -= SPF;
			
			//update
			t += SPF;

			for(i = 0; i < MAX - 1; ++i){
				
				//arr[i].x =(sin((arr[i].x))*bounceSpeed*dir);// * SPF;
				//arr[i].y =(sin((arr[i].y))*bounceSpeed*dir);// * bounceSpeed);
				
#pragma region STUFF
				/**		
				if ( arr[i].x > 1 ) {
					//arr[i].x = 1;
					dir *= -1;
				} else if ( arr[i].x < -1 ) {
					//arr[i].x = -1;
					dir *= -1;
				}
				if ( arr[i].y > 1 ) {
					//arr[i].y  = 1;
					 dir *= -1;
				} else if ( arr[i].y < -1 ) {
					//arr[i].y = -1;
					 dir *= -1;
				}

				if ( arr[i].x < 0.00000000000001 ) {
					//arr[i].x = 1;
					dir *= -1;
				} 
				if ( arr[i].y < 0.00000000000001 ) {
					//arr[i].y  = 1;
					 dir *= -1;
				}
				**/
#pragma endregion

				for (int j = 1 ; j < MAX; j++)
				{ 
					
					distX = arr[i].currPosX - arr[j].currPosX;
					distY = arr[i].currPosY - arr[j].currPosY;
					magDist = (distX * distX) + (distY * distY);
					unitX = distX / sqrt(magDist);
					unitY = distY / sqrt(magDist);
					
					arr[i].force = arr[j].force = ((-K_CONST * arr[i].mass * arr[j].mass) / magDist);
					
					arr[i].accelX = (arr[i].force/arr[i].mass) ;
					arr[i].accelY = (arr[i].force/arr[i].mass) ;

					arr[j].accelX = (arr[j].force/arr[j].mass) ;
					arr[j].accelY = (arr[j].force/arr[j].mass) ;

					arr[i].x += arr[i].accelX * SPF;
					arr[i].y += arr[i].accelY * SPF;
					
					arr[j].x += arr[j].accelX * SPF;
					arr[j].y += arr[j].accelY * SPF;
					//arr[i].forceY = ((-K_CONST * arr[i].mass * arr[j].mass) / magDist);

				}
				
			}
		}

		//draw
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);	

		for(i = 0; i < MAX; i++){
			MAT_SET_TRANSLATE(mat, arr[i].x, arr[i].y);
			//glUniformMatrix3fv(U_TRANSFORM, 1, GL_FALSE, mat);
			p.draw(U_TRANSFORM,mat);
		}
		glfwSwapBuffers();			
	}	

	glfwTerminate();
	return 0;
}
