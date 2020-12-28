
// this file contains the main method of the program

#include "Header.h"	

int main(int argc, char** argv) {

	// read mesh and material info from the appropriate files:
	readFbx("elephant.fbx");
	readMaterials("materials.txt");
	
	// glut init
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Elephant World");
	glClearColor(SKY_BLUE);
	
	// callbacks
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(motion);
	glutMouseFunc(mouseFunc);
	glutKeyboardUpFunc(keyboardUp);
	glutReshapeFunc(reshape);
	glutTimerFunc(1000 / FPS, timer, 0);

	// menu
	glutCreateMenu(menuFunc);
	glutAddMenuEntry("Help", 1);
	glutAddMenuEntry("Adjust ambient light", 2);
	glutAddMenuEntry("Full screen on\\off", 3);
	glutAddMenuEntry("Point light on\\off", 4);
	glutAddMenuEntry("Quit", 5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// lighting
	glShadeModel(GL_SMOOTH);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
	// set initial point light position
	pointLight[0] = emptyMap["lamp_position"].x; 
	pointLight[1] = emptyMap["lamp_position"].y;
	pointLight[2] = emptyMap["lamp_position"].z;
	pointLight[3] = 1;
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, LIGHT_ATTENUATION);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	lightOn = true;

	// visibility detection
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// projection
	glMatrixMode(GL_PROJECTION);
	gluPerspective(45, WIN_WIDTH / WIN_HEIGHT, 0.001, 1000); // we want to see very near and very far
	
	// set cameras
	cam0 = { emptyMap["cam0_p0"], emptyMap["cam0_ref"], { 0, 1, 0 } };
	cam1 = { emptyMap["cam1_p0"], emptyMap["cam1_ref"], {0, 1, 0} };
	vector n = vNormalize(vSub(cam0.p0, cam0.ref)); 
	// set initial values for phi and theta to prevent the camera from jumping at first
	phi = asin(n.y);
	theta = -acos(n.x / cos(phi));
	
	// set elephant position and direction 
	startHeadPivot = currentHeadPivot = emptyMap["e_head_pivot"];
	elephantStartPosition = elephantCurrentPosition = emptyMap["e_body_pivot"];
	n = vSub(cam1.ref, cam1.p0);
	elephantStartDirection = elephantCurrentDirection = vNormalize({n.x, 0, n.z});
	
	// set transformation matrices
	glMatrixMode(GL_MODELVIEW);
	glGetDoublev(GL_MODELVIEW_MATRIX, elephantTransformMatrixBody);
	glGetDoublev(GL_MODELVIEW_MATRIX, elephantTransformMatrixHead);
	glGetDoublev(GL_MODELVIEW_MATRIX, elephantTransformMatrixTail);
	glGetDoublev(GL_MODELVIEW_MATRIX, elephantTransformationMatrixLB);
	glGetDoublev(GL_MODELVIEW_MATRIX, elephantTransformationMatrixLF);
	glGetDoublev(GL_MODELVIEW_MATRIX, elephantTransformationMatrixRB);
	glGetDoublev(GL_MODELVIEW_MATRIX, elephantTransformationMatrixRF);
	glGetDoublev(GL_MODELVIEW_MATRIX, lightTransformMatrix);

	// load\generate textures
	glGenTextures(3, texNames);
	blueWaves();
	greenFlowers();
	redCheckmate();

	// start
	glutFullScreen();
	fullScreen = true;
	glutMainLoop();
	return 0;

}