
// this file contains user input related methods and globals

#include "Header.h"	

camera cam0;
camera cam1;
camera *curCam = &cam0;
GLdouble phi;
GLdouble theta;
GLdouble currentCameraRotationSpeed = ORBIT_CAM_SPEED; // different camera styles have different speeds
GLfloat currentLightAttenuation = LIGHT_ATTENUATION; // the current light intensity value
movementState moveState = ::stop;
bool orbit = true; // camera orbit\pivot mode?
bool middleHeld; // are we holding the middle mouse button?
bool leftHeld; // are we holding the left mouse button? 
bool cam1Changed = true;
bool cam0Changed = true;
float prevX; // previous x screen coordinate value
float prevY; // previous y screen coordinate value
bool movingBody = true;
bool movingHead;
bool movingTail;
bool movingLight;

// the keyboard input callback function
void keyboard(GLubyte key, GLint xMouse, GLint yMouse) {

	switch (key) { // check key
	
	case 'c': // toggle between cameras

		if (curCam == &cam0)
			curCam = &cam1;

		else
			curCam = &cam0;

		break;
	
	case 'o': // toggle between orbit\pivot camera mode (main camera only)

		if (orbit) { // switching from orbit to pivot mode? 

			currentCameraRotationSpeed = PIVOT_CAM_SPEED;
			phi -= M_PI; // p0 is now the pivot point and so we need to "flip" by -180

		}

		else {

			currentCameraRotationSpeed = ORBIT_CAM_SPEED; // we use a different speed for orbit-style camera
			phi += M_PI; // ref is now the pivot point and so we need to "flip" by +180

		}

		orbit = !orbit; // we changed modes
		break;
	
	case 'w': // moving something forwards (or up if head\tail)

		moveState = ::forward;
		break;
	
	case 's': // moving something backwards (or down if head\tail)

		moveState = ::backward;
		break;
	
	case 'a': // moving\rotating something to the left

		moveState = ::left;
		break;
	
	case 'd': // moving\rotating something to the right

		moveState = ::right;
		break;
	
	case 'r': // moving light up

		moveState = ::up;
		break;
	
	case 'f': // moving light down

		moveState = ::down;
		break;
	
	case 'b': // controlling body now

		movingBody = true;
		movingTail = false;
		movingHead = false;
		movingLight = false;
		break;
	
	case 'h': // controlling head now

		movingHead = true;
		movingBody = false;
		movingTail = false;
		movingLight = false;
		break;
	
	case 't': // controlling tail now

		movingTail = true;
		movingBody = false;
		movingHead = false;
		movingLight = false;
		break;
	
	case 'l': // controlling light now

		movingLight = true;
		movingBody = false;
		movingTail = false;
		movingHead = false;
		break;

	case 'j': // decrease light intensity

		if (currentLightAttenuation > LIGHT_ATTENUATION_MAX) // not too little light
			break;

		currentLightAttenuation += LIGHT_ATTENUATION_INC;
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, currentLightAttenuation);
		break;

	case 'u': // increase light intensity

		if (currentLightAttenuation < LIGHT_ATTENUATION_MIN) // not too much light
			break;

		currentLightAttenuation -= LIGHT_ATTENUATION_INC;
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, currentLightAttenuation);
		break;

	}

}

// keyboard key released
void keyboardUp(GLubyte key, GLint xMouse, GLint yMouse) {

	moveState = ::stop; // we're not moving any models now

	if (movingBody){ // were we moving the body just before?

		// reset legs rotation for the sake of aesthetics (this is not necessary but it looks better I think)
		glPushMatrix();
		glGetDoublev(GL_MODELVIEW_MATRIX, elephantTransformationMatrixRB);
		glGetDoublev(GL_MODELVIEW_MATRIX, elephantTransformationMatrixRF);
		glGetDoublev(GL_MODELVIEW_MATRIX, elephantTransformationMatrixLB);
		glGetDoublev(GL_MODELVIEW_MATRIX, elephantTransformationMatrixLF);
		currentLegsRotation = 0; // start rotation from 0 with every new movement
		legsFlip = 1; // reset legs flipping bool to 1 
		glPopMatrix();

	}
	
}

// the mouse callback function
void mouseFunc(int button, int state, int x, int y) {

	if (curCam != &cam0)
		return;

	if (button == GLUT_MIDDLE_BUTTON) 
		middleHeld = state == GLUT_DOWN ? true : false;

	else if (button == GLUT_LEFT_BUTTON)
		leftHeld = state == GLUT_DOWN ? true : false;

	else if (button == MOUSEWHEEL_IN || button == MOUSEWHEEL_OUT) { // mouse wheel?

		vector n = vScalar(CAM_ZOOM_SPEED, vNormalize(vSub(cam0.ref, cam0.p0))); // scalar * normalized camera direction						
		if (button == MOUSEWHEEL_IN && vNorm(vSub(cam0.ref, cam0.p0)) > CAM_ZOOM_SPEED) // avoid crossing ref when moving p0
			cam0.p0 = vAdd(cam0.p0, n); 

		else if (button == MOUSEWHEEL_OUT)
			cam0.p0 = vSub(cam0.p0, n);

	}

	cam0Changed = true;

}

// the mouse motion callback function
void motion(int x, int y) {

	if (curCam != &cam0)
		return;

	GLdouble diffX = (x - prevX);
	GLdouble diffY = (y - prevY);

	prevX = x;
	prevY = y;

	vector n = vSub(cam0.p0, cam0.ref); // camera direction vector

	if (middleHeld) { // middle button is held so we're making orbit\pivot camera movement now

		if (fabs(diffX) < 20) // prevents the camera from jumping too hard at first
			theta += diffX * currentCameraRotationSpeed; // increment theta

		if (fabs(diffY) < 20) // prevents the camera from jumping too hard at first
			phi += diffY * currentCameraRotationSpeed * 2; // increment phi (screen is wide so increment more here)
			
		// always keep the angles in an appropriate range to prevent an eventual overflow
		GLdouble a = 2 * M_PI;

		if (phi > a)
			phi -= a;

		else if (phi < -a)
			phi += a;

		if (theta > a) 
			theta -= a;

		else if (theta < -a)
			theta += a;
			

		// we're orbiting now 
		if (orbit) {

			// orbit p0 around ref (orbit v as well to prevent camera flipping)
			cam0.p0 = vAdd(sphereToCart(vNorm(n), phi, theta), cam0.ref);
			cam0.v = sphereToCart(1, phi + M_PI / 2, theta);

		}

		// we're pivoting now 
		else {

			// orbit ref around p0 (orbit v as well to prevent camera flipping)
			cam0.ref = vAdd(sphereToCart(vNorm(n), phi, theta), cam0.p0);
			cam0.v = sphereToCart(1, phi - M_PI / 2, theta);

		}

	}

	else if (leftHeld) { // left button is held so we're making panning camera movement now

		vector u;
		vector temp;

		n = vNormalize(vSub(cam0.p0, cam0.ref)); // normalized camera direction
		u = vCross(n, cam0.v); // right vector for moving camera right\left


		if (fabs(diffX * PAN_CAM_SPEED) < 0.5) { // prevents the camera from jumping too hard at first

			// move camera right\left
			temp = vScalar(diffX * PAN_CAM_SPEED, u);
			cam0.p0 = vAdd(cam0.p0, temp);
			cam0.ref = vAdd(cam0.ref, temp);

		}

		if (fabs(diffY * PAN_CAM_SPEED) < 0.5) { // prevents the camera from jumping too hard at first

			// move camera down\up
			temp = vScalar(diffY * PAN_CAM_SPEED, cam0.v);
			cam0.p0 = vAdd(cam0.p0, temp);
			cam0.ref = vAdd(cam0.ref, temp);

		}

	}

	cam0Changed = true;

}
