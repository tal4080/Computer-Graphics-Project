
//////////////////////////
// Author: Tal Rave		//
// I.D. 301276986		//
// Version: 02/08/2019	//
//////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// This program renders a 3d scene using openGL (an elephant in a porcelain store).	//
//////////////////////////////////////////////////////////////////////////////////////

#pragma once
#define _USE_MATH_DEFINES
#include <windows.h>
#include <GL/glut.h>
#include<fstream>
#include<iostream>
#include <string>
#include <map>
#include <cmath>

using namespace std;

#define WIN_WIDTH 1000 // initial window width
#define WIN_HEIGHT 1000 // initial window height
#define SKY_BLUE 0.5, 0.8, 1, 0 // background color
#define MAX_INDICES 10000 // max memory allocation when reading .fbx file (indices)
#define MAX_DATA 30000 // max memory allocation when reading .fbx file (normals\vertices) 
#define ORBIT_CAM_SPEED 0.005 // orbit style camera speed
#define PIVOT_CAM_SPEED 0.0015 // pivot style camera speed 
#define PAN_CAM_SPEED 0.02 // pan style camera speed
#define CAM_ZOOM_SPEED 0.5 // camera zoom in\out speed
#define MOUSEWHEEL_IN 3 // mouse wheel button code
#define MOUSEWHEEL_OUT 4 // mouse wheel button code
#define ELEPHANT_MOVE_SPEED 0.01 // movement speed when controlling the elephant
#define ELEPHANT_ROTATION_SPEED 2 // rotation speed when controlling the elephant
#define HEAD_ROTATION_SPEED 2 // rotation speed when controlling the elephant's head
#define TAIL_ROTATION_SPEED 2 // rotation speed when controlling the elephant's tail
#define HEAD_MAX_UP_ROTATION 80 // a limit on head up rotation
#define HEAD_MIN_DOWN_ROTATION -50 // a limit on head down rotation
#define HEAD_MAX_SIDE_ROTATION 80 // a limit on head side rotation
#define TAIL_MAX_UP_ROTATION 150 // a limit on tail up rotation
#define TAIL_MIN_DOWN_ROTATION 0 // a limit on tail down rotation
#define TAIL_MAX_SIDE_ROTATION 120 // a limit on tail side rotation
#define LIGHT_MOVE_SPEED 0.1 // movement speed when controlling the point light
#define LIGHT_ATTENUATION 0.05 // initial attenuation value
#define LIGHT_ATTENUATION_INC 0.01 // attenuation increment amount per click
#define LIGHT_ATTENUATION_MIN 0.00001 // min attenuation value (max light intensity)
#define LIGHT_ATTENUATION_MAX 1 // max attenuation value (min light intensity)
#define LEG_ROTATION_SPEED 5 // rotation speed of legs when moving the elephant
#define LEGS_MAX_ROTATION 30 // max leg rotation before flipping the direction of rotation (to simulate walking)
#define FPS 60 // how many times to call redisplay per second 


enum movementState {stop, forward, backward, left, right, up, down}; // what kind of movement is happening now


typedef struct {GLdouble x, y, z;} vector; // represents points\vectors in 3d

typedef GLdouble matrix[16]; // 3d transformation matrix

typedef struct {vector p0, ref, v;} camera; // p0 = view point, ref = reference point, v = up vector 

typedef GLfloat color[3]; // rgb color (no alpha)

typedef struct {color ambientDiffuse; 
				color specular; 
				GLfloat shininess;} material; // material struct for models

typedef struct {string name;
				material material;
				int *indices;
				GLdouble *coordinates, *normals;
				size_t coordinatesSize, indicesSize, normalsSize;} model; // model struct containing all the info that's needed to manipulate imported meshes 


extern std::map<string, int> modelMap; // maps model names to their indices in the model array so that we can refer to them by name 
extern std::map<string, vector> emptyMap; // map empty object names to their location (we need these in order to determine camera stats and locations\pivot points for moving models)
extern model *models; // the array of mesh models
extern size_t modelsSize; // size of the above array
extern camera cam0; // main camera
extern camera cam1; // elephant eyes camera
extern camera *curCam; // the current active camera
extern vector startHeadPivot; // the head's pivot point
extern vector currentHeadPivot; // the head's pivot point after transforming cam1
extern vector elephantStartPosition;  // the elephant's (body) position\pivot point
extern vector elephantCurrentPosition; // the elephant's (body) position\pivot point after transforming cam1
extern vector elephantStartDirection;  // the elephant's (body) direction
extern vector elephantCurrentDirection; // the elephant's (body) direction after transforming cam1
extern GLfloat globalAmbient[3]; // the ambient light 
extern GLfloat pointLight[4]; // the point light
extern GLdouble phi; // phi angle for main camera orbit\pivot 
extern GLdouble theta; // theta angle for main camera orbit\pivot
extern movementState moveState; // what kind of movement is happening now
extern matrix elephantTransformMatrixBody; // body transformation matrix
extern matrix elephantTransformMatrixHead; // head transformation matrix
extern matrix elephantTransformMatrixTail; // tail transformation matrix
extern matrix elephantTransformationMatrixLB; // left back leg transformation matrix
extern matrix elephantTransformationMatrixLF; // left front leg transformation matrix
extern matrix elephantTransformationMatrixRB; // right back leg transformation matrix
extern matrix elephantTransformationMatrixRF; // right front leg transformation matrix
extern matrix lightTransformMatrix; // light transformation matrix
extern matrix cam1TransformationMatrix; // cam1 transformation matrix
extern matrix cam0TransformationMatrix; // cam0 transformation matrix
extern GLdouble currentLegsRotation; // the current legs rotation when moving the elephant (we flip the rotation direction to simulate walking)
extern GLuint texNames[3]; // store "names" for textures so that we do not have to regenerate them
extern bool movingBody; // are we controlling the body now?
extern bool movingHead; // are we controlling the head now?
extern bool movingTail; // are we controlling the tail now?
extern bool movingLight; // are we controlling the point light now?
extern bool cam1Changed; // did cam1 stats change?
extern bool cam0Changed; // did cam0 stats change?
extern bool fullScreen; // do we start in full screen?
extern bool lightOn; // is the point light on?
extern int legsFlip; // we reached max rotation for legs so flip the rotation direction (again, to simulate walking)


void readFbx(const char *fileName); // this method reads\parses the .fbx file to get the models' info (vertices, indices, normals, empty objects)
void readMaterials(const char *fileName); // this method reads the materials.txt file to get the materials info for the models (diffuse, specular, shininess)
void printHelp(const char *fileName); // this method prints the help.txt file to provide the user with the controls info

vector vAdd(vector v1, vector v2); //  vector addition
vector vSub(vector v1, vector v2); // vector subtraction
vector vScalar(GLdouble t, vector v); // multiply a vector by a number
GLdouble vDot(vector v1, vector v2); // vector dot product 
vector vCross(vector vL, vector vR); // vector cross product
GLdouble vNorm(vector v); // vector norm
vector vNormalize(vector v); // normalize vector
vector sphereToCart(GLdouble r, GLdouble phi, GLdouble theta); // spherical coordinates to cartesian coordinates
GLdouble degToRad(GLdouble deg); // degrees to radians
vector vRotate(vector n, GLdouble a, bool rx, bool ry, bool rz); // rotates a vector\point

void transformLegs(); // this method transforms the elephant's legs while updating the appropriate matrix
void transformHead(); // this method transforms the elephant's head while updating the appropriate matrix
void transformTail(); // this method transforms the elephant's tail while updating the appropriate matrix
void transformBody(); // this method transforms the elephant's body while updating the appropriate matrix
void transformLight(); // this method transforms the light while updating the appropriate matrix

void display(); // the display callback function
void reshape(int w, int h); // the reshape callback function
void timer(int i); // timer for redisplay

void keyboard(GLubyte key, GLint xMouse, GLint yMouse); // the keyboard callback function
void keyboardUp(GLubyte key, GLint xMouse, GLint yMouse); // the keyboardUp callback function
void mouseFunc(int button, int state, int x, int y); // the mouse callback function
void motion(int x, int y); // the motion callback function  

void menuFunc(GLint selected); // the menu callback function

void blueWaves(); // generates a texture
void greenFlowers(); // generates a texture
void redCheckmate(); // generates a texture