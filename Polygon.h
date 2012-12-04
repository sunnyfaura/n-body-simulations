#ifndef POLYGON_H__
#define POLYGON_H__

#ifdef __APPLE__
	#define glGenVertexArrays(a, b) glGenVertexArraysAPPLE(a, b)
	#define glBindVertexArray(c) glBindVertexArrayAPPLE(c)
	#define glDeleteVertexArrays(n, arrays) glDeleteVertexArraysAPPLE( n, arrays);
#else

#endif

#include <cmath>
#include "header.h"
//#include <OpenGL/glext.h>

class Polygon {
	GLuint vbo, vao;
	const GLuint totalVertices;

	struct Vtx {
		GLfloat x, y;
		GLuint color;
	};

	Polygon(const Polygon &);
	Polygon& operator=(const Polygon &);
public:
	enum { ATTR_POS, ATTR_COLOR };

	Polygon(float radius, int vertices, GLuint color, float initialAngle = 0) : totalVertices(vertices + 2) {
		glGenBuffers(1, &vbo);
		glGenVertexArrays(1, &vao);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(Vtx) * totalVertices, 0, GL_STATIC_DRAW);
		Vtx *v = (Vtx*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

		v[0].x = v[0].y = 0;
		v[0].color = color;

		const float increment = 2 * M_PI /vertices;
		for ( int i = 1; i < vertices + 2; ++i ) {
			const float t = initialAngle + increment * (i - 1);
			v[i].color = color;
			v[i].x = cos(t) * radius;
			v[i].y = sin(t) * radius;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);

		glEnableVertexAttribArray(ATTR_POS);
		glVertexAttribPointer(ATTR_POS, 2, GL_FLOAT, GL_FALSE, sizeof(Vtx), 0);
		glEnableVertexAttribArray(ATTR_COLOR);
		glVertexAttribPointer(ATTR_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vtx), (void*)(2 * sizeof(GLfloat)));
	}

	~Polygon() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}
	
	void draw() {
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, totalVertices);
	}
};

#endif