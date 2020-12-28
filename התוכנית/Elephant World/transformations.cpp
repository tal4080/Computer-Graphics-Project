
// this file handles\updates the transformation matrices of all the models

#include "Header.h"

vector startHeadPivot;
vector currentHeadPivot;
vector elephantStartPosition;
vector elephantStartDirection;
vector elephantCurrentPosition;
vector elephantCurrentDirection;
matrix elephantTransformMatrixBody;
matrix elephantTransformMatrixCam1;
matrix elephantTransformMatrixHead;
matrix elephantTransformMatrixTail;
matrix elephantTransformationMatrixLB;
matrix elephantTransformationMatrixLF;
matrix elephantTransformationMatrixRB;
matrix elephantTransformationMatrixRF;
matrix lightTransformMatrix;
matrix cam1TransformationMatrix;
matrix cam0TransformationMatrix;
GLdouble elephantRotationInRadians = degToRad(ELEPHANT_ROTATION_SPEED);
GLdouble headRotationInRadians = degToRad(HEAD_ROTATION_SPEED);
GLdouble tailRotationInRadians = degToRad(TAIL_ROTATION_SPEED);
GLdouble currentHeadUpAngle; // we limit head rotations for aesthetical reasons  
GLdouble currentTailUpAngle; // we limit tail rotations for aesthetical reasons  
GLdouble currentHeadSideAngle; // we limit head rotations for aesthetical reasons  
GLdouble currentTailSideAngle; // we limit tail rotations for aesthetical reasons  
GLdouble currentLegsRotation;
GLdouble bodyRotation; // this is used to get the body and head transformations to coincide in elephant view mode (cam1)
int legsFlip = 1; // this is used to flip legs' rotation direction to simulate walking

// overload for method transformLegs() 
void transformLegs(vector p, GLdouble m[16], int flip) {

	glLoadMatrixd(m); // load the appropriate matrix for updating

	// rotate the leg around it's pivot point
	glTranslated(p.x, p.y, p.z);
	glRotated(flip * LEG_ROTATION_SPEED, 1, 0, 0);
	glTranslated(-p.x, -p.y, -p.z);

	glGetDoublev(GL_MODELVIEW_MATRIX, m); // update the matrix

}
// updates all the legs' rotations
void transformLegs() {

	if (currentLegsRotation > LEGS_MAX_ROTATION) { // reached max rotation so flip rotation direction

		legsFlip = -legsFlip;
		currentLegsRotation = -LEGS_MAX_ROTATION;

	}
	currentLegsRotation += LEG_ROTATION_SPEED; // update the current rotation value

	// rotate all the legs (left and right legs rotate in different directions to make it look like the elephant is walking - so cute :) )
	transformLegs(emptyMap["e_legRB_pivot"], elephantTransformationMatrixRB, legsFlip);
	transformLegs(emptyMap["e_legRF_pivot"], elephantTransformationMatrixRF, legsFlip);
	transformLegs(emptyMap["e_legLB_pivot"], elephantTransformationMatrixLB, -legsFlip);
	transformLegs(emptyMap["e_legLF_pivot"], elephantTransformationMatrixLF, -legsFlip);

}

// updates the body transformation matrix
void transformBody() {

	// this makes sure that we move in the correct direction
	int flip = -1;
	if (moveState == ::forward || moveState == ::left)
		flip = -flip;

	glLoadMatrixd(elephantTransformMatrixBody); // load the appropriate matrix

	if (moveState == ::forward || moveState == ::backward) {

		vector d = vScalar(flip * ELEPHANT_MOVE_SPEED, elephantStartDirection);
		
		glTranslated(d.x, d.y, d.z);

		d = vScalar(flip * ELEPHANT_MOVE_SPEED, elephantCurrentDirection); // now we need to get cam1 to coincide with the movement done to the elephant in cam0

		cam1.p0 = vAdd(cam1.p0, d); // move p0
		cam1.ref = vAdd(cam1.ref, d); // move ref
		elephantCurrentPosition = vAdd(elephantCurrentPosition, d); // update the elephant's direction
		currentHeadPivot = vAdd(currentHeadPivot, d); // update the elephant's head pivot point

	}

	if (moveState == ::left || moveState == ::right) {

		vector p = elephantStartPosition;

		// rotate the elephant
		glTranslated(p.x, p.y, p.z);
		glRotated(flip * ELEPHANT_ROTATION_SPEED, 0, 1, 0);
		glTranslated(-p.x, -p.y, -p.z);

		p = elephantCurrentPosition; // now we need to get cam1 to coincide with the rotation done to the elephant in cam0

		// rotate p0 around the body's position
		cam1.p0 = vSub(cam1.p0, p);
		cam1.p0 = vRotate(cam1.p0, flip * elephantRotationInRadians, 0, 1, 0);
		cam1.p0 = vAdd(cam1.p0, p);

		// rotate ref around the body's position
		cam1.ref = vSub(cam1.ref, p);
		cam1.ref = vRotate(cam1.ref, flip * elephantRotationInRadians, 0, 1, 0);
		cam1.ref = vAdd(cam1.ref, p);

		// rotate the head pivot point around the body's position
		currentHeadPivot = vSub(currentHeadPivot, p);
		currentHeadPivot = vRotate(currentHeadPivot, flip * elephantRotationInRadians, 0, 1, 0);
		currentHeadPivot = vAdd(currentHeadPivot, p);

		// update the elephant's direction
		elephantCurrentDirection = vRotate(elephantCurrentDirection, flip * elephantRotationInRadians, 0, 1, 0);

		// update body rotation (as we mentioned before, this is used in the head rotation method)
		bodyRotation += flip * elephantRotationInRadians;

	}

	// update the matrix
	glGetDoublev(GL_MODELVIEW_MATRIX, elephantTransformMatrixBody);

}

// updates the head transformation matrix
void transformHead() {

	// which axis are we rotating around?
	bool rotateX = false;
	bool rotateY = false;
	bool rotateZ = false;
	GLdouble flip = 1;
	GLdouble a = HEAD_ROTATION_SPEED;
	vector p = startHeadPivot;

	if (moveState == ::forward) {

		if (currentHeadUpAngle > HEAD_MAX_UP_ROTATION) // do not rotate to much - it looks bad 
			return;
		currentHeadUpAngle += HEAD_ROTATION_SPEED; // update to make sure we do not rotate too much
		rotateX = true;

	}

	else if (moveState == ::backward) {

		if (currentHeadUpAngle < HEAD_MIN_DOWN_ROTATION) 
			return;
		currentHeadUpAngle -= HEAD_ROTATION_SPEED;
		flip = -flip;
		rotateX = true;

	}

	else if (moveState == ::left) {

		if (currentHeadSideAngle > HEAD_MAX_SIDE_ROTATION)
			return;
		currentHeadSideAngle += HEAD_ROTATION_SPEED;
		rotateY = true;

	}

	else {

		if (currentHeadSideAngle < -HEAD_MAX_SIDE_ROTATION)
			return;
		currentHeadSideAngle -= HEAD_ROTATION_SPEED;
		flip = -flip;
		rotateY = true;

	}

	glLoadMatrixd(elephantTransformMatrixHead); // load the appropriate matrix
	
	// perform the rotation
	glTranslated(p.x, p.y, p.z);
	glRotated(flip * a, rotateX, rotateY, rotateZ);
	glTranslated(-p.x, -p.y, -p.z);

	// update the matrix
	glGetDoublev(GL_MODELVIEW_MATRIX, elephantTransformMatrixHead);

	// now we need to update cam1
	p = currentHeadPivot;

	GLdouble r = bodyRotation;

	// we rotate cam1 while accounting for body rotations to get things to coincide
	cam1.ref = vSub(cam1.ref, p);
	cam1.ref = vRotate(cam1.ref, -r, 0, !rotateY, 0); // if rotateY == true then there's no need to account for body rotations
	cam1.ref = vRotate(cam1.ref, flip * headRotationInRadians, rotateX, rotateY, rotateZ);
	cam1.ref = vRotate(cam1.ref, r, 0, !rotateY, 0); // if rotateY == true then there's no need to account for body rotations
	cam1.ref = vAdd(cam1.ref, p);

	// same for p0
	cam1.p0 = vSub(cam1.p0, p);
	cam1.p0 = vRotate(cam1.p0, -r, 0, !rotateY, 0);
	cam1.p0 = vRotate(cam1.p0, flip * headRotationInRadians, rotateX, rotateY, rotateZ);
	cam1.p0 = vRotate(cam1.p0, r, 0, !rotateY, 0);
	cam1.p0 = vAdd(cam1.p0, p);

}

// updates the tail transformation matrix
void transformTail() {

	bool rotateX = false;
	bool rotateY = false;
	bool rotateZ = false;
	GLdouble flip = 1;
	GLdouble a = TAIL_ROTATION_SPEED;
	vector p = emptyMap["e_tail_pivot"];

	// this part is similar to the head rotations
	if (moveState == ::forward) {

		if (currentTailUpAngle > TAIL_MAX_UP_ROTATION)
			return;
		currentTailUpAngle += TAIL_ROTATION_SPEED;
		flip = -flip;
		rotateX = true;

	}

	else if (moveState == ::backward) {

		if (currentTailUpAngle < TAIL_MIN_DOWN_ROTATION)
			return;
		currentTailUpAngle -= TAIL_ROTATION_SPEED;
		rotateX = true;

	}

	else if (moveState == ::left) {

		if (currentTailSideAngle < -TAIL_MAX_SIDE_ROTATION)
			return;
		currentTailSideAngle -= TAIL_ROTATION_SPEED;
		flip = -flip;
		rotateZ = true;

	}

	else {

		if (currentTailSideAngle > TAIL_MAX_SIDE_ROTATION)
			return;
		currentTailSideAngle += TAIL_ROTATION_SPEED;
		rotateZ = true;

	}

	// we do not need to update cam1 stats for the tail of course so just update the matrix
	glLoadMatrixd(elephantTransformMatrixTail);
	glTranslated(p.x, p.y, p.z);
	glRotated(flip * a, rotateX, rotateY, rotateZ);
	glTranslated(-p.x, -p.y, -p.z);
	glGetDoublev(GL_MODELVIEW_MATRIX, elephantTransformMatrixTail);

}

// updates the light transformation matrix
void transformLight() {

	// the transformations here are straightforward

	glLoadMatrixd(lightTransformMatrix);

	if (moveState == ::forward)
		glTranslated(0, 0, LIGHT_MOVE_SPEED);

	else if (moveState == ::backward)
		glTranslated(0, 0, -LIGHT_MOVE_SPEED);

	else if (moveState == ::left)
		glTranslated(LIGHT_MOVE_SPEED, 0, 0);

	else if (moveState == ::right)
		glTranslated(-LIGHT_MOVE_SPEED, 0, 0);

	else if (moveState == ::up)
		glTranslated(0, LIGHT_MOVE_SPEED, 0);

	else if (moveState == ::down)
		glTranslated(0, -LIGHT_MOVE_SPEED, 0);

	glGetDoublev(GL_MODELVIEW_MATRIX, lightTransformMatrix);

}
