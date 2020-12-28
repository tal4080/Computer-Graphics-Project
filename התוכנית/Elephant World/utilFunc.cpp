
// this file contains utility methods that are used throughout the program. all the methods here are self-explanatory

#include "Header.h"

vector sphereToCart(GLdouble r, GLdouble phi, GLdouble theta) {

	return { r * cos(phi) * cos(theta), r * sin(phi), r * cos(phi) * sin(theta) };

}

vector vAdd(vector v1, vector v2) {

	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };

}

vector vSub(vector v1, vector v2) {

	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };

}

vector vScalar(GLdouble t, vector v) {

	return { t * v.x, t * v.y, t * v.z };

}

GLdouble vDot(vector v1, vector v2) {

	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

}

vector vCross(vector vL, vector vR) {

	return	{	vL.y * vR.z - vL.z * vR.y,
				vL.z * vR.x - vL.x * vR.z,
				vL.x * vR.y - vL.y * vR.x	};

}

GLdouble vNorm(vector v) {

	return sqrt(vDot(v, v));

}

vector vNormalize(vector v) {

	GLdouble n = vNorm(v);
	return vScalar(1 / n, v);

}

GLdouble degToRad(GLdouble deg) {

	return M_PI * deg / 180;

}

vector vRotate(vector n, GLdouble a, bool rx, bool ry, bool rz) {

	GLdouble x = n.x;
	GLdouble y = n.y;
	GLdouble z = n.z;

	if (rx) {

		y = cos(a) * n.y - sin(a) * n.z;
		z = cos(a) * n.z + sin(a) * n.y;

	}
	
	else if (ry) {

		x = cos(a) * n.x + sin(a) * n.z;
		z = cos(a) * n.z - sin(a) * n.x;

	}
	
	else if (rz) {

		x = cos(a) * n.x - sin(a) * n.y;
		y = cos(a) * n.z + sin(a) * n.x;

	}

	return { x, y, z };

}