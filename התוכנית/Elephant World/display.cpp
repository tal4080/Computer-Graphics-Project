
// this file contains display related methods and globals

#include "Header.h"

GLfloat globalAmbient[] = { 0.1, 0.1, 0.1 }; // we start with a low intensity ambient light
GLfloat pointLight[4];
GLuint texNames[3];

// This method draws the textures when it's called from the display function
void drawTexture(int i) { // 'i' is an index to the model (a quad surface) that will contain the texture
	
	GLfloat coord[4][2] = { {0, 0}, {1, 0}, {1, 1}, {0, 1} }; // standard order texture mapping
	GLuint texNum = 0; // index to the texture's "name"

	// all the textures will be applied to the pictures in the scene
	if (models[i].name == "tex_picture2")
		texNum = 1;

	else if (models[i].name == "tex_picture3")
		texNum = 2;

	glBindTexture(GL_TEXTURE_2D, texNames[texNum]);
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
	for (int k = 0, n = 0; k < 4; k++, n += 3) { // use the .fbx indices to map to the appropriate coordinates\normals

		int idx = models[i].indices[k];
		if (k == 3)
			idx = -idx - 1;
		idx *= 3;
		glNormal3d(models[i].normals[n], models[i].normals[n + 1], models[i].normals[n + 2]); // set the normal
		glTexCoord2fv(coord[k]); // set the texture coordinate
		glVertex3d(models[i].coordinates[idx], models[i].coordinates[idx + 1], models[i].coordinates[idx + 2]); // plot the vertex

	}
	glEnd();

	glDisable(GL_TEXTURE_2D);

}

// generates\calculates the checkmate texture
void blueWaves() {

	int size = 128; // size of the texture
	int len = size * size * 3; // the total length of the array containing the texture
	
	GLfloat *t = new(std::nothrow) GLfloat[len]; // allocate a temporary array for the texture

	if (!t) {

		cout << "Memory allocation failed!\n";
		system("pause");
		exit(0);

	}

	// this part uses the cosine function to produce a wave\arc-like texture
	for (int i = 0, j = 0, k = 0; i < len; i += 3, j = i / (3 * size), k = (i / 3) % size) {

		t[i] = 0;
		t[i + 1] = 0;
		t[i + 2] = cos((j*j - k*k)/(4.0*size)); // blue

	}

	
	glBindTexture(GL_TEXTURE_2D, texNames[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGB, GL_FLOAT, t);

	delete [] t;

}

// generates\calculates the greenFlowers texture
void greenFlowers() {

	int size = 128;
	int len = size * size * 3;
	
	GLfloat *t = new(std::nothrow) GLfloat[len];

	if (!t) {

		cout << "Memory allocation failed!\n";
		system("pause");
		exit(0);

	}

	// this part uses the sine function to produce flower\petal-like texture
	for (int i = 0, j = 0, k = 0; i < len; i += 3, j = i / (3 * size), k = (i / 3) % size) {

		t[i] = 0;
		t[i + 1] = sin((j * k) / 4.0); // green
		t[i + 2] = 0;

	}

	
	glBindTexture(GL_TEXTURE_2D, texNames[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGB, GL_FLOAT, t);

	delete [] t;

}

// generates\calculates the checkmate texture
void redCheckmate() {

	int size = 8;
	int len = size * size * 3;
	bool b = 1;

	GLfloat *t = new(std::nothrow) GLfloat[len];

	if (!t) {

		cout << "Memory allocation failed!\n";
		system("pause");
		exit(0);

	}

	// alternates between red and black squares to produce a checkmate-like texture
	for (int i = 0, j = 0, k = 0; i < len; i += 3, j = i / (3 * size), k = (i / 3) % size) {
		b = !b;
		if (k == 0)
			b = !b;
		t[i] = b / 2.0;
		t[i + 1] = 0;
		t[i + 2] = 0;
	}


	glBindTexture(GL_TEXTURE_2D, texNames[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGB, GL_FLOAT, t);

	delete[] t;

}

// this method draws regular models (non-textured)
void drawModel(int i) {

	// use the .fbx indices to map to the appropriate coordinates\normals (a negative index marks a polygon's last vertex)
	for (int j = 0, k = 0, n = 0; j < models[i].indicesSize; j++, k++) {

		if (models[i].indices[j] < 0) { // reached a negative index so start plotting the polygon  

			glBegin(GL_POLYGON);
			for (; k >= 0; k--, n += 3) {

				int idx = models[i].indices[j - k];
				if (k == 0)
					idx = -idx - 1;
				idx *= 3;
				glNormal3d(models[i].normals[n], models[i].normals[n + 1], models[i].normals[n + 2]);
				glVertex3d(models[i].coordinates[idx], models[i].coordinates[idx + 1], models[i].coordinates[idx + 2]);

			}
			glEnd();

		}

	}

}

// the display function
void display() {

	// clear color/depth buffers 
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	// update transformation matrices for all moving models
	glPushMatrix();
	if (moveState != ::stop) { // something moved?

		if (movingLight) // light moved?
			transformLight();

		else if (moveState != ::up && moveState != ::down){

			// body and head transformations alter cam1
			if (movingBody) { // body moved?

				transformBody();
				transformLegs();
				cam1Changed = true;

			}

			else if (movingHead) {// head moved?
				transformHead();
				cam1Changed = true;
			}

			else if (movingTail) // tail moved?
				transformTail();

		}

	}
	glPopMatrix(); // done updating the transformation matrices

	// set the camera first
	glPushMatrix();
	if (curCam == &cam0) {

		if (cam0Changed) { // reset camera stats? 

			gluLookAt(curCam->p0.x, curCam->p0.y, curCam->p0.z, curCam->ref.x, curCam->ref.y, curCam->ref.z, curCam->v.x, curCam->v.y, curCam->v.z);
			glGetDoublev(GL_MODELVIEW_MATRIX, cam0TransformationMatrix);
			cam0Changed = false;

		}

		else
			glMultMatrixd(cam0TransformationMatrix);
			
	}
	
	else {

		if (cam1Changed) {

			gluLookAt(curCam->p0.x, curCam->p0.y, curCam->p0.z, curCam->ref.x, curCam->ref.y, curCam->ref.z, curCam->v.x, curCam->v.y, curCam->v.z);
			glGetDoublev(GL_MODELVIEW_MATRIX, cam1TransformationMatrix);
			cam1Changed = false;

		}

		else
			glMultMatrixd(cam1TransformationMatrix);

	}
	
	// transform the light
	glPushMatrix();
	glMultMatrixd(lightTransformMatrix);
	glLightfv(GL_LIGHT0, GL_POSITION, pointLight);
	glPopMatrix();
	
	// scan the model array and apply the appropriate transformations 
	for (int i = 0; i < modelsSize; i++) {
	
		bool isElephant = models[i].name.find("e_") != string::npos; // an elephant body part?
		bool isLight = models[i].name.find("lamp_") != string::npos; // a model representing the light?
		bool isTexture = models[i].name.find("tex_") != string::npos; // a texture quad? (one of the pictures)
		
		// start transforming
		glPushMatrix();
		if (isLight) // transform the model representing the light
			glMultMatrixd(lightTransformMatrix);

		else if (isElephant) { // transform the elephant

			glMultMatrixd(elephantTransformMatrixBody);

			if (models[i].name == "e_head" || models[i].name == "e_eyes") // head and eyes move together as the head is transformed
				glMultMatrixd(elephantTransformMatrixHead);

			else if (models[i].name == "e_tail") // transform the tail
				glMultMatrixd(elephantTransformMatrixTail);

			// transform all the legs as the elephant moves
			else if (models[i].name == "e_legRB")
				glMultMatrixd(elephantTransformationMatrixRB);

			else if (models[i].name == "e_legRF")
				glMultMatrixd(elephantTransformationMatrixRF);

			else if (models[i].name == "e_legLB")
				glMultMatrixd(elephantTransformationMatrixLB);

			else if (models[i].name == "e_legLF")
				glMultMatrixd(elephantTransformationMatrixLF);

		}
	
		// set the model's material
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, models[i].material.ambientDiffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, models[i].material.specular);
		glMaterialf(GL_FRONT, GL_SHININESS, models[i].material.shininess);

		if (isTexture) // draw texture if needed
			drawTexture(i);
		else if (curCam == &cam0 || !isElephant) // else draw the model normally (if the current camera is cam1 then there's no need to display the elephant so don't draw it)
			drawModel(i);
		glPopMatrix();
		

	}
	glPopMatrix(); // back to identity
	glFlush();	

}

// redisplay timer
void timer(int i) {

	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, timer, 0);

}

// we use fullscreen
void reshape(int w, int h) {

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat) w / h, 0.001, 1000); // reapply the projection matrix
	glMatrixMode(GL_MODELVIEW);

}
