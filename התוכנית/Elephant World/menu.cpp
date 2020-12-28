
// this file contains menu related methods

#include "Header.h"

bool fullScreen;
bool lightOn;

// the menu callback function
void menuFunc(GLint selected) {

	if (selected == 1 || selected == 2) {

		system("cls");
		SetForegroundWindow(GetConsoleWindow());

		if (selected == 1)
			printHelp("help.txt");

		else if (selected == 2) { // ask for new values to update ambient light

			cout << "The current values for ambient light are:\n"
				<< globalAmbient[0] << "\t" << globalAmbient[1] << "\t" << globalAmbient[2] << "\n"
				<< "Please enter new values for ambient light: \n";

			GLfloat temp[3];

			cin >> temp[0] >> temp[1] >> temp[2];

			// check input correctness
			if (!cin.fail()) {

				cout << "Done!\n";
				memcpy(&globalAmbient, &temp, sizeof(temp));
				glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

			}

			else {

				cout << "Bad input!\n";
				cin.clear();

			}

			cin.ignore(INT_MAX, '\n');

		}

	}

	else if (selected == 3) {

		if (fullScreen)
			glutReshapeWindow(WIN_WIDTH, WIN_HEIGHT);

		else
			glutFullScreen();

		fullScreen = !fullScreen;

	}

	else if (selected == 4) {

		if (lightOn)
			glDisable(GL_LIGHT0);

		else
			glEnable(GL_LIGHT0);

		lightOn = !lightOn;

	}
		
	else
		exit(0);

}