/******************************************************************
 *
 * RotatingCube.c
 *
 * Description: This example demonstrates a colored, rotating
 * cube in shader-based OpenGL. The use of transformation
 * matrices, perspective projection, and indexed triangle sets
 * are shown.
 *
 * Computer Graphics Proseminar SS 2014
 *
 * Interactive Graphics and Simulation Group
 * Institute of Computer Science
 * University of Innsbruck
 *
 *******************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

/* OpenGL includes */
#include <GL/glew.h>
#include <GL/freeglut.h>

/* Local includes */
#include "LoadShader.h"   /* Provides loading function for shader code */
#include "Matrix.h" 

/*----------------------------------------------------------------*/
/* Define Attribute Pointer Names */
#define ATT_PYRAMID 0
#define ATT_CUBE 1
#define ATT_STAR 2
#define ATT_DIAMOND 3
 

/*----------------------------------------------------------------*/

/* Define handle to a vertex buffer object */
GLuint VBOS;
GLuint VBOP;
GLuint VBOD;
GLuint VBOH1;
GLuint VBOH2;
GLuint VBOF;

/* Define handle to a color buffer object */
GLuint CBOS;
GLuint CBOP;
GLuint CBOD;
GLuint CBOH;
GLuint CBOF;

/* Define handle to an index buffer object */
GLuint IBOS;
GLuint IBOP;
GLuint IBOD;
GLuint IBOH;
GLuint IBOF;


/* Indices to vertex attributes; in this case positon and color */
enum DataID {
	vPosition = 0, vColor = 1
};

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

GLuint ShaderProgramD;
GLuint ShaderProgramS;
GLuint ShaderProgramP;

float ProjectionMatrix[16]; /* Perspective projection matrix */
float ViewMatrix[16]; /* Camera view matrix */
float ModelMatrixD[16]; /* Model matrices */
float ModelMatrixP[16];
float ModelMatrixS[16];
float ModelMatrixHanger1[16];
float ModelMatrixHanger2[16];

float ModelMatrixFloor[16];

/* Set Keyboard access variables */
float camera_disp = -20.0;
float rotate_speed = 1.0;

bool rotateXD = true; // DIAMOND
bool rotateYD = true;
bool rotateZD = true;

bool rotateXP = false; // PYRAMID
bool rotateYP = true;
bool rotateZP = false;

bool rotateXS = false; // STAR
bool rotateYS = true;
bool rotateZS = false;
  
bool isAnim = true;

/* Transformation matrices for initial position */
float TranslateOrigin[16];
float TranslateDown[16];
float RotateX[16];
float RotateZ[16];
float InitialTransformD[16];
float InitialTransformP[16];
float InitialTransformS[16];
float InitialTransformH1[16];
float InitialTransformH2[16];

/* bar hanger */
GLfloat vertex_buffer_data_hanger_top[] = { /* 8 cube vertices */
    -5.0, -0.1,  0.2,
     5.0, -0.1,  0.2,
     5.0,  0.1,  0.2,
    -5.0,  0.1,  0.2,
    -5.0, -0.1, -0.2,
     5.0, -0.1, -0.2,
     5.0,  0.1, -0.2,
    -5.0,  0.1, -0.2, 
    
    5.2, -0.3, -0.4,
    5.2, -0.3, 0.4,
    5.2, -0.1, -0.4,
    5.2, -0.1, 0.4,
    4.8, -0.3, -0.4,
    4.8, -0.3, 0.4,
    4.8, -0.1, -0.4,
    4.8, -0.1, 0.4, 
    
    
    -5.2, -0.3, -0.4,
    -5.2, -0.3, 0.4,
    -5.2, -0.1, -0.4,
    -5.2, -0.1, 0.4,
    -4.8, -0.3, -0.4,
    -4.8, -0.3, 0.4,
    -4.8, -0.1, -0.4,
    -4.8, -0.1, 0.4,      
};  

GLfloat vertex_buffer_data_hanger_middle[] = { /* 8 cube vertices */
    -2.5, -0.1,  0.1,
     2.5, -0.1,  0.1,
     2.5,  0.1,  0.1,
    -2.5,  0.1,  0.1,
    -2.5, -0.1, -0.1,
     2.5, -0.1, -0.1,
     2.5,  0.1, -0.1,
    -2.5,  0.1, -0.1,    
    
    2.7, -0.3, -0.2,
    2.7, -0.3, 0.2,
    2.7, -0.1, -0.2,
    2.7, -0.1, 0.2,
    2.3, -0.3, -0.2,
    2.3, -0.3, 0.2,
    2.3, -0.1, -0.2,
    2.3, -0.1, 0.2,
    
    -2.7, -0.3, -0.2,
    -2.7, -0.3, 0.2,
    -2.7, -0.1, -0.2,
    -2.7, -0.1, 0.2,
    -2.3, -0.3, -0.2,
    -2.3, -0.3, 0.2,
    -2.3, -0.1, -0.2,
    -2.3, -0.1, 0.2,
};

GLfloat color_buffer_data_hanger[] = { /* RGB color values for bar vertices */
	1.0, 1.0, 1.0,
	0.8, 0.8, 0.8,
	1.0, 1.0, 1.0,
	0.8, 0.8, 0.8,
	1.0, 1.0, 1.0,
	0.8, 0.8, 0.8,
	1.0, 1.0, 1.0,
	0.8, 0.8, 0.8,
};

GLushort index_buffer_data_hanger[] = { /* Indices of 6*2 triangles */
    0, 1, 2,
    2, 3, 0,
    1, 5, 6,
    6, 2, 1,
    7, 6, 5,
    5, 4, 7,
    4, 0, 3,
    3, 7, 4,
    4, 5, 1,
    1, 0, 4,
    3, 2, 6,
    6, 7, 3,
    
    8, 9, 10,
    9, 10, 11,
    8, 10, 12,
    10,12, 14,
    9,11, 13,
    11,13,15,
    8,9,12,
    9,12,13,
    10,11,14,
    11,14,15,
    12, 13, 14,
    13, 14, 15,
    
    16,17,18,
    17,18,19,
    16,18,20,
    18,20,22,
    17,19,21,
    19,21,23,
    16,17,20,
    17,20,21,
    18,19,22,
    19,22,23,
    20,21,22,
    21,22,23,      
};




const GLuint numVertices_diamond = 10*3;
GLfloat vertex_buffer_data_diamond[] = { /* 8 cube vertices */
/* tips */
0,0,2,
0,0,-2,

/* hexagon */
-1,0,0,
-0.333,1,0,
0.333,1,0,
1,0,0,
0.333,-1,0,
-0.333,-1,0,
};

GLfloat color_buffer_data_diamond[] = { /* RGB color values for vertices */
1.0, 0.0, 0.0, 
0.0, 1.0, 0.0, 
0.0, 0.0, 1.0, 
1.0, 0.0, 0.0, 
0.0, 1.0, 0.0, 
0.0, 0.0, 1.0, 
1.0, 0.0, 0.0, 
0.0, 1.0, 0.0, 
};

GLushort index_buffer_data_diamond[] = { /* Indices of 6*2 triangles */
/* triangles to top */
0,2,3,
0,3,4,
0,4,5,
0,5,6,
0,6,7,
0,2,7,
/* triangles to bottom */
1,2,3,
1,3,4,
1,4,5,
1,5,6,
1,6,7,
1,2,7,
 };



const GLuint numVertices_pyramid = 6 * 3;
GLfloat vertex_buffer_data_pyramid[] = { /* 5 pyramid vertices */
-0.5, -0.5, -0.5, 
0.5, -0.5, -0.5, 
0.5, -0.5, 0.5, 
-0.5, -0.5, 0.5,
0.0, 0.5, 0.0,
};

GLfloat color_buffer_data_pyramid[] = { /* RGB color values for vertices */
0.6, 0.2, 0.0, 
0.6, 0.2, 0.0, 
0.6, 0.2, 0.0, 
0.6, 0.2, 0.0, 
1.0, 0.0, 0.0, 
};

GLushort index_buffer_data_pyramid[] = { /* Indices of 4+2 triangles */
0, 1, 2, 
0, 2, 3, 
0, 1, 4, 
1, 2, 4, 
2, 3, 4, 
3, 0, 4, };

const GLuint numVertices_star = 24 * 3;
GLfloat vertex_buffer_data_star[] = { /* 8 + 6 pyramid vertices */
	/* Cube Vertices */
	-0.25, -0.25, 0.25, 
	0.25, -0.25, 0.25, 
	0.25, 0.25, 0.25, 
	-0.25, 0.25, 0.25,
	-0.25, -0.25, -0.25, 
	0.25, -0.25, -0.25, 
	0.25, 0.25, -0.25, 
	-0.25, 0.25,-0.25,

	/* Tip Vertices */
	-1.0, 0.0, 0.0, 	// Left
	1.0, 0.0, 0.0, 		// Right
	0.0, -1.0, 0.0,		// Down
	0.0, 1.0, 0.0,		// Up
	0.0, 0.0, -1.0,		// Back
	0.0, 0.0, 1.0,		// Front
};

GLfloat color_buffer_data_star[] = { /* RGB color values for vertices */
	1.0, 1.0, 0.0, 
	0.5, 0.5, 0.0, 
	1.0, 1.0, 0.0, 
	0.5, 0.5, 0.0, 
	1.0, 1.0, 0.0, 
	0.5, 0.5, 0.0,  
	1.0, 1.0, 0.0, 
	0.5, 0.5, 0.0, 
	1.0, 1.0, 0.0, 
	0.5, 0.5, 0.0, 
	1.0, 1.0, 0.0, 
	0.5, 0.5, 0.0, 
	1.0, 1.0, 0.0, 
	0.5, 0.5, 0.0, 
};

GLushort index_buffer_data_star[] = { /* Indices of 6*4 triangles */
/* Cube Indices */ 
/*0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1, 7, 6, 5, 5, 4, 7, 4, 0, 3, 3, 7, 4, 4, 5, 1,
		1, 0, 4, 3, 2, 6, 6, 7, 3,
*/
		/* Tip Indices */
		8, 0, 3, 
		8, 3, 7, 
		8, 0, 4, 
		8, 7, 4,

		9, 1, 5, 
		9, 5, 6, 
		9, 6, 2, 
		9, 1, 2,

		10, 0, 1, 
		10, 1, 5, 
		10, 5, 4, 
		10, 4, 0,

		11, 3, 2, 
		11, 2, 6, 
		11, 6, 7, 
		11, 3, 7,

		12, 4, 5, 
		12, 5, 6, 
		12, 6, 7, 
		12, 4, 7,

		13, 0, 1, 
		13, 1, 2, 
		13, 2, 3, 
		13, 0, 3, 
};

const GLuint numVertices_floor = 4 * 3;
GLfloat vertex_buffer_data_floor[] = { /* 4 plane vertices */
10, -5, 10,
-10, -5, 10,
10, -5, -10,
-10, -5, -10,
};

GLfloat color_buffer_data_floor[] = { /* RGB color values for vertices */
0.5, 0.5, 0.5, 
0.1, 0.1, 0.1, 
0.1, 0.1, 0.1, 
1.0, 1.0, 1.0, 
};

GLushort index_buffer_data_floor[] = { /* Indices of 2 triangles */
0, 1, 2, 
1, 2, 3, };

/*----------------------------------------------------------------*/

void associateShader() {
	/* Associate program with shader matrices */
	GLint projectionUniform = glGetUniformLocation(ShaderProgramD,
			"ProjectionMatrix");
	if (projectionUniform == -1) {
		fprintf(stderr, "Could not bind uniform ProjectionMatrix\n");
		exit(-1);
	}
	glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, ProjectionMatrix);

	GLint ViewUniform = glGetUniformLocation(ShaderProgramD, "ViewMatrix");
	if (ViewUniform == -1) {
		fprintf(stderr, "Could not bind uniform ViewMatrix\n");
		exit(-1);
	}
	glUniformMatrix4fv(ViewUniform, 1, GL_TRUE, ViewMatrix);

	/* Clear window; color specified in 'Initialize()' */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void drawFloor() {
	GLint ModelUniform = glGetUniformLocation(ShaderProgramD, "ModelMatrix");
	if (ModelUniform == -1) {
		fprintf(stderr, "Could not bind uniform ModelMatrix\n");
		exit(-1);
	}
	glUniformMatrix4fv(ModelUniform, 1, GL_TRUE, ModelMatrixFloor);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	glEnableVertexAttribArray(vPosition);
	glBindBuffer(GL_ARRAY_BUFFER, VBOF);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(vColor);
	glBindBuffer(GL_ARRAY_BUFFER, CBOF);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOF);
	GLint size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	//glLoadIdentity();
	
	/* Issue draw command, using indexed triangle list */
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	
	/* Disable attributes */
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);
	glPopMatrix();
}

void drawPyramid() {
	GLint ModelUniform = glGetUniformLocation(ShaderProgramD, "ModelMatrix");
	if (ModelUniform == -1) {
		fprintf(stderr, "Could not bind uniform ModelMatrix\n");
		exit(-1);
	}
	glUniformMatrix4fv(ModelUniform, 1, GL_TRUE, ModelMatrixP);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	glEnableVertexAttribArray(vPosition);
	glBindBuffer(GL_ARRAY_BUFFER, VBOP);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(vColor);
	glBindBuffer(GL_ARRAY_BUFFER, CBOP);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOP);
	GLint size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	//glLoadIdentity();
	
	/* Issue draw command, using indexed triangle list */
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	
	/* Disable attributes */
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);
	glPopMatrix();
}

void drawStar() {
	GLint ModelUniform = glGetUniformLocation(ShaderProgramD, "ModelMatrix");
	if (ModelUniform == -1) {
		fprintf(stderr, "Could not bind uniform ModelMatrix\n");
		exit(-1);
	}
	glUniformMatrix4fv(ModelUniform, 1, GL_TRUE, ModelMatrixS);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	glEnableVertexAttribArray(vPosition);
	glBindBuffer(GL_ARRAY_BUFFER, VBOS);	
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(vColor);
	glBindBuffer(GL_ARRAY_BUFFER, CBOS);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOS);
	GLint size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
	/* Issue draw command, using indexed triangle list */
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	/* Disable attributes */
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);
	glPopMatrix();
}

void drawDiamond() {	
	GLint ModelUniform = glGetUniformLocation(ShaderProgramD, "ModelMatrix");
	if (ModelUniform == -1) {
		fprintf(stderr, "Could not bind uniform ModelMatrix\n");
		exit(-1);
	}
	glUniformMatrix4fv(ModelUniform, 1, GL_TRUE, ModelMatrixD);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
			
	glEnableVertexAttribArray(vPosition);
	glBindBuffer(GL_ARRAY_BUFFER, VBOD);	
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(vColor);
	glBindBuffer(GL_ARRAY_BUFFER, CBOD);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOD);
	GLint size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	/* Issue draw command, using indexed triangle list */
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	/* Disable attributes */
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);
	
	glPopMatrix();
}

void drawHanger() {	
	GLint ModelUniform = glGetUniformLocation(ShaderProgramD, "ModelMatrix");
	if (ModelUniform == -1) {
		fprintf(stderr, "Could not bind uniform ModelMatrix\n");
		exit(-1);
	}
	glUniformMatrix4fv(ModelUniform, 1, GL_TRUE, ModelMatrixHanger1);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
			
	glEnableVertexAttribArray(vPosition);
	glBindBuffer(GL_ARRAY_BUFFER, VBOH1);	
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(vColor);
	glBindBuffer(GL_ARRAY_BUFFER, CBOH);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOH);
	GLint size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	/* Issue draw command, using indexed triangle list */
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	/* Disable attributes */
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);
	
	glPopMatrix();
}

void drawHanger2() {	
	GLint ModelUniform = glGetUniformLocation(ShaderProgramD, "ModelMatrix");
	if (ModelUniform == -1) {
		fprintf(stderr, "Could not bind uniform ModelMatrix\n");
		exit(-1);
	}
	glUniformMatrix4fv(ModelUniform, 1, GL_TRUE, ModelMatrixHanger2);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
			
	glEnableVertexAttribArray(vPosition);
	glBindBuffer(GL_ARRAY_BUFFER, VBOH2);	
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(vColor);
	glBindBuffer(GL_ARRAY_BUFFER, CBOH);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOH);
	GLint size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	/* Issue draw command, using indexed triangle list */
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	/* Disable attributes */
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);
	
	glPopMatrix();
}



/******************************************************************
 *
 * Display
 *
 * This function is called when the content of the window needs to be
 * drawn/redrawn. It has been specified through 'glutDisplayFunc()';
 * Enable vertex attributes, create binding between C program and
 * attribute name in shader
 *
 *******************************************************************/

void Display() {
	associateShader();
	/* Clear window; color specified in 'Initialize()' */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
     
	drawPyramid();
	drawStar();
	drawDiamond();
	drawHanger();
	drawHanger2();
	drawFloor();

	/* Swap between front and back buffer */
	glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y)   
{
    switch( key ) 
    {
	case '1':
		rotate_speed = 5.0;
		break;
	case '2':
		rotate_speed = 2.0;
		break;
	case '3':
		rotate_speed = 1.0;
		break;
	case '4':
		rotate_speed = 0.5;
		break;
	case '5':
		rotate_speed = 0.1;
		break;
	case 'a': // ALL ROTATIONS
		rotateXD = true;
		rotateYD = true;
		rotateZD = true;
		
		rotateXP = true;
		rotateYP = true;
		rotateZP = true;
		
		rotateXS = true;
		rotateYS = true;
		rotateZS = true;
		break;
	case 'e':
		camera_disp -= 1;
		break;
	case 'i':
		rotate_speed *= -1;
		break;
	case 's': // STANDARD
		rotateXD = true; // DIAMOND
		rotateYD = true;
		rotateZD = true;;

		rotateXP = false; // PYRAMID
		rotateYP = true;
		rotateZP = false;

		rotateXS = false; // STAR
		rotateYS = true;
		rotateZS = false;
		
		rotate_speed = 1.0; //Rotation speed to standard
		break; 
	case 'w':
		camera_disp += 1;
		break;
	/* Toggle animation */
	case '0':
		if (isAnim)
			isAnim = false;		
		else
			isAnim = true;
		break;
	case 'q': case 'Q':  
	    exit(0);    
		break;
    }

    glutPostRedisplay();
}

/******************************************************************
 *
 * OnIdled
 *
 *
 *
 *******************************************************************/

void OnIdle() {
	if(isAnim) {
		float cur_angle;
		float RotationMatrixAnimX[16];
		float RotationMatrixAnimY[16];
		float RotationMatrixAnimZ[16];
		float TranslationMatrix[16];
		float TranslationMatrix2[16];
		float FullTransformMatrix[16];
		float RotationCameraMatrixAnim[16];
		float InitialCameraTransform[16];

		/* DIAMOND */
		cur_angle = (glutGet(GLUT_ELAPSED_TIME) / (400.0*rotate_speed)) * (180 / M_PI);	
		if(rotateXD) SetRotationX(cur_angle, RotationMatrixAnimX); else SetRotationX(0, RotationMatrixAnimX);
		if(rotateYD) SetRotationY(cur_angle, RotationMatrixAnimY); else SetRotationY(0, RotationMatrixAnimY);
		if(rotateZD) SetRotationZ(-cur_angle, RotationMatrixAnimZ); else SetRotationZ(0, RotationMatrixAnimZ);
		SetTranslation(-5, -1,0 , TranslationMatrix);	
		/* apply transformations */
		MultiplyMatrix(TranslationMatrix, RotationMatrixAnimX, FullTransformMatrix);
		MultiplyMatrix(FullTransformMatrix, RotationMatrixAnimY, FullTransformMatrix);
		MultiplyMatrix(FullTransformMatrix, RotationMatrixAnimZ, FullTransformMatrix);
		MultiplyMatrix(FullTransformMatrix, InitialTransformD, ModelMatrixD);
		
		/* Pyramid */
		cur_angle = (glutGet(GLUT_ELAPSED_TIME) / (500.0*rotate_speed)) * (180 / M_PI);	
		if(rotateXP) SetRotationX(cur_angle, RotationMatrixAnimX); else SetRotationX(0, RotationMatrixAnimX);
		if(rotateYP) SetRotationY(cur_angle, RotationMatrixAnimY); else SetRotationY(0, RotationMatrixAnimY);
		if(rotateZP) SetRotationZ(cur_angle, RotationMatrixAnimZ); else SetRotationZ(0, RotationMatrixAnimZ);
		SetTranslation(7.5, -2.5,0 , TranslationMatrix);	
		/* apply transformations */
		MultiplyMatrix(TranslationMatrix, RotationMatrixAnimX, FullTransformMatrix);
		MultiplyMatrix(FullTransformMatrix, RotationMatrixAnimY, FullTransformMatrix);
		MultiplyMatrix(FullTransformMatrix, RotationMatrixAnimZ, FullTransformMatrix);
		MultiplyMatrix(FullTransformMatrix, InitialTransformP, ModelMatrixP);
		
		/* Star */
		cur_angle = (glutGet(GLUT_ELAPSED_TIME) / (800.0*rotate_speed)) * (180 / M_PI);	
		if(rotateXS) SetRotationX(-cur_angle, RotationMatrixAnimX); else SetRotationX(0, RotationMatrixAnimX);
		if(rotateYS) SetRotationY(-cur_angle, RotationMatrixAnimY); else SetRotationY(0, RotationMatrixAnimY);
		if(rotateZS) SetRotationZ(-cur_angle, RotationMatrixAnimZ); else SetRotationZ(0, RotationMatrixAnimZ);
		SetTranslation(2.5, -2.5,0 , TranslationMatrix);;	
		/* apply transformations */
		MultiplyMatrix(TranslationMatrix, RotationMatrixAnimX, FullTransformMatrix);
		MultiplyMatrix(FullTransformMatrix, RotationMatrixAnimY, FullTransformMatrix);
		MultiplyMatrix(FullTransformMatrix, RotationMatrixAnimZ, FullTransformMatrix);
		MultiplyMatrix(FullTransformMatrix, InitialTransformS, ModelMatrixS);
		
		/* Hanger1 */
		SetTranslation(0,3,0, ModelMatrixHanger1);
		
		/* Hanger2 */
		SetTranslation(5,0,0, ModelMatrixHanger2);
		
		// Rotate everything around the X axis
		cur_angle = (glutGet(GLUT_ELAPSED_TIME) / (600.0*rotate_speed)) * (180 / M_PI);
		SetRotationY(-cur_angle, RotationMatrixAnimX);
		SetTranslation(-5,0,0, TranslationMatrix);
		SetTranslation(5,0,0, TranslationMatrix2);
		
		MultiplyMatrix(TranslationMatrix2, RotationMatrixAnimX, FullTransformMatrix);
		MultiplyMatrix(FullTransformMatrix, TranslationMatrix, FullTransformMatrix);
		
		MultiplyMatrix(FullTransformMatrix, ModelMatrixHanger2, ModelMatrixHanger2);
		MultiplyMatrix(FullTransformMatrix, ModelMatrixS, ModelMatrixS);
		MultiplyMatrix(FullTransformMatrix, ModelMatrixP, ModelMatrixP);
		 
		
		SetRotationY((glutGet(GLUT_ELAPSED_TIME) / (1000.0*rotate_speed)) * (180 / M_PI), RotationCameraMatrixAnim);
		SetTranslation(0.0, 0.0, camera_disp, InitialCameraTransform);
		MultiplyMatrix(InitialCameraTransform, RotationCameraMatrixAnim, ViewMatrix);
		
		/* Request redrawing forof window content */
		glutPostRedisplay();
	}
}

/******************************************************************
 *
 * reshape
 *
 *
 *
 *******************************************************************/
 
void reshape (int w, int h)
{
	//this way we tell opengl that the rendering window has changed
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}


/******************************************************************
 *
 * SetupDataBuffers
 *
 * Create buffer objects and load data into buffers
 *
 *******************************************************************/

	void SetupDataBuffers() {
	/* STAR */
	glGenBuffers(1, &VBOS);
	glBindBuffer(GL_ARRAY_BUFFER, VBOS);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data_star),
			vertex_buffer_data_star, GL_STREAM_DRAW);

	glGenBuffers(1, &IBOS);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOS);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data_star),
			index_buffer_data_star, GL_STATIC_DRAW);

	glGenBuffers(1, &CBOS);
	glBindBuffer(GL_ARRAY_BUFFER, CBOS);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data_star),
			color_buffer_data_star, GL_STATIC_DRAW);			
			
	/* PYRAMID */
	glGenBuffers(1, &VBOP);
	glBindBuffer(GL_ARRAY_BUFFER, VBOP);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data_pyramid),
			vertex_buffer_data_pyramid, GL_STREAM_DRAW);

	glGenBuffers(1, &IBOP);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOP);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data_pyramid),
			index_buffer_data_pyramid, GL_STATIC_DRAW);

	glGenBuffers(1, &CBOP);
	glBindBuffer(GL_ARRAY_BUFFER, CBOP);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data_pyramid),
			color_buffer_data_pyramid, GL_STATIC_DRAW);

	/* DIAMOND */
	glGenBuffers(1, &VBOD);
	glBindBuffer(GL_ARRAY_BUFFER, VBOD);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data_diamond),
			vertex_buffer_data_diamond, GL_STREAM_DRAW);

	glGenBuffers(1, &IBOD);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOD);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data_diamond),
			index_buffer_data_diamond, GL_STATIC_DRAW);

	glGenBuffers(1, &CBOD);
	glBindBuffer(GL_ARRAY_BUFFER, CBOD);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data_diamond),
			color_buffer_data_diamond, GL_STATIC_DRAW);
			
	/* HANGER HIGH */
	glGenBuffers(1, &VBOH1);
	glBindBuffer(GL_ARRAY_BUFFER, VBOH1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data_hanger_top),
			vertex_buffer_data_hanger_top, GL_STREAM_DRAW);
			
	/* HANGER LOW */
	glGenBuffers(1, &VBOH2);
	glBindBuffer(GL_ARRAY_BUFFER, VBOH2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data_hanger_middle),
			vertex_buffer_data_hanger_middle, GL_STREAM_DRAW);

	glGenBuffers(1, &IBOH);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOH);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data_hanger),
			index_buffer_data_hanger, GL_STATIC_DRAW);

	glGenBuffers(1, &CBOH);
	glBindBuffer(GL_ARRAY_BUFFER, CBOH);
	glBufferData(GL_ARRAY_BUFFER, sizeof(index_buffer_data_hanger),
			color_buffer_data_hanger, GL_STATIC_DRAW);
			
			
    /* FLOOR */
	glGenBuffers(1, &VBOF);
	glBindBuffer(GL_ARRAY_BUFFER, VBOF);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data_floor),
			vertex_buffer_data_floor, GL_STREAM_DRAW);

	glGenBuffers(1, &IBOF);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOF);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data_floor),
			index_buffer_data_floor, GL_STATIC_DRAW);

	glGenBuffers(1, &CBOF);
	glBindBuffer(GL_ARRAY_BUFFER, CBOF);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data_floor),
			color_buffer_data_floor, GL_STATIC_DRAW);
			
}

/******************************************************************
 *
 * AddShader
 *
 * This function creates and adds individual shaders
 *
 *******************************************************************/

void AddShader(GLuint ShaderProgram, const char* ShaderCode, GLenum ShaderType) {
	/* Create shader object */
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}

	/* Associate shader source code string with shader object */
	glShaderSource(ShaderObj, 1, &ShaderCode, NULL);

	GLint success = 0;
	GLchar InfoLog[1024];

	/* Compile shader source code */
	glCompileShader(ShaderObj);
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType,
				InfoLog);
		exit(1);
	}

	/* Associate shader with shader program */
	glAttachShader(ShaderProgram, ShaderObj);
}

/******************************************************************
 *
 * CreateShaderProgram
 *
 * This function creates the shader program; vertex and fragment
 * shaders are loaded and linked into program; final shader program
 * is put into the rendering pipeline
 *
 *******************************************************************/

void CreateShaderProgram() {
	/* Allocate shader object */
	ShaderProgramD = glCreateProgram();

	if (ShaderProgramD == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	/* Load shader code from file */
	VertexShaderString = LoadShader("vertexshader.vs");
	FragmentShaderString = LoadShader("fragmentshader.fs");

	/* Separately add vertex and fragment shader to program */
	AddShader(ShaderProgramD, VertexShaderString, GL_VERTEX_SHADER);
	AddShader(ShaderProgramD, FragmentShaderString, GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024];

	/* Link shader code into executable shader program */
	glLinkProgram(ShaderProgramD);

	/* Check results of linking step */
	glGetProgramiv(ShaderProgramD, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgramD, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	/* Check if shader program can be executed */
	glValidateProgram(ShaderProgramD);
	glGetProgramiv(ShaderProgramD, GL_VALIDATE_STATUS, &Success);

	if (!Success) {
		glGetProgramInfoLog(ShaderProgramD, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	/* Put linked shader program into drawing pipeline */
	glUseProgram(ShaderProgramD);
}

/******************************************************************
 *
 * Initialize
 *
 * This function is called to initialize rendering elements, setup
 * vertex buffer objects, and to setup the vertex and fragment shader
 *
 *******************************************************************/

void Initialize(void) {
	/* Set background (clear) color to blue */
	glClearColor(0.0, 0.0, 0.4, 0.0);

	/* Enable depth testing */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	/* Setup vertex, color, and index buffer objects */
	SetupDataBuffers();

	/* Setup shaders and shader program */
	CreateShaderProgram();

	/* Initialize matrices */
	SetIdentityMatrix(ProjectionMatrix);
	SetIdentityMatrix(ViewMatrix);
	SetIdentityMatrix(ModelMatrixD);
	SetIdentityMatrix(ModelMatrixP);
	SetIdentityMatrix(ModelMatrixS);
	SetIdentityMatrix(ModelMatrixHanger1);
	SetIdentityMatrix(ModelMatrixHanger2);
	SetIdentityMatrix(ModelMatrixFloor);

	/* Set projection transform */
	float fovy = 45.0;
	float aspect = 1.0;
	float nearPlane = 1.0;
	float farPlane = 50.0;
	SetPerspectiveMatrix(fovy, aspect, nearPlane, farPlane, ProjectionMatrix);

	/* Set viewing transform */
		SetTranslation(0.0, 0.0, camera_disp, ViewMatrix);

	/* INITIAL TRANSFORM DIAMOND */
		SetIdentityMatrix(InitialTransformD);
	
	/* INITIAL TRANSFORM STAR */
		SetIdentityMatrix(InitialTransformS);
	
	/* INITIAL TRANSFORM PYRAMID */
		SetIdentityMatrix(InitialTransformP);	
		
	/* INITIAL TRANSFORM HANGER */
		SetIdentityMatrix(InitialTransformH1);
}

/******************************************************************
 *
 * main
 *
 * Main function to setup GLUT, GLEW, and enter rendering loop
 *
 *******************************************************************/

int main(int argc, char** argv) {
	/* Initialize GLUT; set double buffered window and RGBA color model */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(400, 400);
	glutCreateWindow("Mobile - Marxer/Spiegl");

	/* Initialize GL extension wrangler */
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	/* Setup scene and rendering parameters */
	Initialize();

	/* Specify callback functions;enter GLUT event processing loop,
	 * handing control over to GLUT */
	glutIdleFunc(OnIdle);
	glutDisplayFunc(Display);
    glutKeyboardFunc(Keyboard); 
	glutMainLoop();

	/* ISO C requires main to return int */
	return 0;
}
