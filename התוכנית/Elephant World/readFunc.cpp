
// this file contains file reading methods

#include "Header.h"

std::map<string, int> modelMap = {};
std::map<string, vector> emptyMap = {};
model *models = NULL;
size_t modelsSize = 0;

// this method reads model data from the .fbx file
void readFbx(const char *fileName) {

	int numCoordinates = 0;
	int numIndices = 0;
	int numNormals = 0;

	int *indices = new(std::nothrow) int[MAX_INDICES]; // temporary array for coordinate indices
	GLdouble *data = new(std::nothrow) GLdouble[MAX_DATA]; // temporary array for coordinates\normals	
	
	if (!indices || !data) {
		cout << "Memory allocation failed!\n";
		system("pause");
		exit(0);
	}

	size_t start, end;
	string line;
	ifstream input(fileName);

	if (!input.is_open()) {

		cout << "Failed to open " << fileName << "!\n";
		system("pause");
		exit(0);

	}

	// check how many mesh models exist in the file
	while (getline(input, line) && line.find("Geometry") == string::npos) 
		;
	getline(input, line, ':');
	getline(input, line);
	models = new(std::nothrow) model[modelsSize = stoi(line)]; // allocate an array for the mesh models
	if (!models) {

		cout << "Memory allocation failed!\n";
		system("pause");
		exit(0);

	}

	// get empty models info (we need these for camera stats and moving mesh models' locations\pivot points)
	while (getline(input, line)){
	
		if (line.find("\", \"Null") != string::npos) {
			
			// get the name
			start = line.find("::");
			end = line.find("\",");
			string name = line.substr(start + 2, end - start - 2);

			// get the location coordinates
			while (getline(input, line, ',') && (start = line.find("Lcl Translation")) == string::npos)
				;
			while (getline(input, line, ',') && !isdigit(line[0]) && line[0] != '-')
				;
			GLdouble x = stod(line);
			getline(input, line, ',');
			GLdouble y = stod(line);
			getline(input, line, ',');
			GLdouble z = stod(line);

			// update the map
			emptyMap.insert({ name, {x, y, z } });
				
		}
		
		else if (line.find("Mesh") != string::npos) // done with empty models
			break;

	}
	
	// get mesh model info
	for (int j = 0; j < modelsSize; j++){
		
		// get the name
		numCoordinates = numIndices = numNormals = 0;
		start = line.find("::");
		line = line.substr(start);
		end = line.find('"');
		string name = line.substr(2, end-2);

		// update the map
		modelMap.insert({ name, j });
		models[j].name = name;
		
		// get the coordinates
		while (getline(input, line, ' ') && (start = line.find("Vertices:")) == string::npos)
			;
		while (getline(input, line, ',') && (start = line.find("PolygonVertexIndex:")) == string::npos)
			data[numCoordinates++] = stod(line);
		data[numCoordinates++] = stod(line);

		// store the coordinates
		models[j].coordinates = new(std::nothrow) GLdouble[numCoordinates];
		if (!models[j].coordinates) {

			cout << "Memory allocation failed!\n";
			system("pause");
			exit(0);

		}
		models[j].coordinatesSize = numCoordinates;
		for (int i = 0; i < numCoordinates; i++) 
			models[j].coordinates[i] = data[i];
		
		// get the indices
 		line = line.substr(start + sizeof("PolygonVertexIndex:"));
		indices[numIndices++] = stod(line);
		while (getline(input, line, ',') && (start = line.find("Normals:")) == string::npos)
			indices[numIndices++] = stod(line);
		indices[numIndices++] = stod(line);

		// store the indices
		models[j].indices = new(std::nothrow) int[numIndices];
		if (!models[j].indices) {

			cout << "Memory allocation failed!\n";
			system("pause");
			exit(0);

		}
		models[j].indicesSize = numIndices;
		for (int i = 0; i < numIndices; i++)
			models[j].indices[i] = indices[i];

		// get the normals
		line = line.substr(start);
		line = line.substr(sizeof("Normals"));
		data[numNormals++] = stod(line);
		while (getline(input, line, ',') && (start = line.find("}")) == string::npos)
			data[numNormals++] = stod(line);
		data[numNormals++] = stod(line);
		
		// store the normals
		models[j].normals = new(std::nothrow) GLdouble[numNormals];
		if (!models[j].normals) {

			cout << "Memory allocation failed!\n";
			system("pause");
			exit(0);

		}
		models[j].normalsSize = numNormals;
		for (int i = 0; i < numNormals; i++)
			models[j].normals[i] = data[i];

	}	
	
	// delete temporary\auxiliary arrays
	delete [] indices;
	delete [] data;
	input.close();

}

// this method gets the models' material info
void readMaterials(const char *fileName) {
	
	string name;
	ifstream input(fileName);

	if (!input.is_open()) {

		cout << "Failed to open " << fileName << "!\n";
		system("pause");
		exit(0);

	}

	while (input >> name) {

		model *m = &models[modelMap[name]];

		input >> m->material.ambientDiffuse[0];
		input >> m->material.ambientDiffuse[1];
		input >> m->material.ambientDiffuse[2];

		input >> m->material.specular[0];
		input >> m->material.specular[1];
		input >> m->material.specular[2];

		input >> m->material.shininess;

	}

	input.close();

}

// this method reads and prints the help info
void printHelp(const char *fileName) {
	
	string line;
	ifstream input(fileName);

	if (!input.is_open()) {

		cout << "Failed to open " << fileName << "!\n";
		system("pause");
		exit(0);

	}

	while (getline(input, line))
		cout << line + "\n";

	input.close();

}