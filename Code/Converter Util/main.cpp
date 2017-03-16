/*------------------------------------------------------------------------------
obj to aobj converter
aobj is a custom model format that makes it easier to load vertices and indexes
into a 3d rendering code that uses indexed triangle lists to render.

NOTE!
This program only supports obj files that index using triangles and/or quads.

MIT License

Copyright (c) 2017 Andreas Palmqvist

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <locale>
#include <forward_list>

using namespace std;

#define PATH_SIZE 64

enum Report {
	ERROR_FILE_NOT_FOUND, ERROR_UNCOMPATIBLE_OBJ, ERROR_MTL_NOT_FOUND, ERROR_UNCOMPATIBLE_MTL, ERROR_NO_MTL, SUCCESS
};

enum Primitive {
	UNKNOWN = 0, TRIANGLE = 3, QUAD = 4
};

struct ObjCoord {
	float x, y, z;
};

struct ObjTexCoord {
	float u, v;
};

struct Vertex {
	float x, y, z;
	float nx, ny, nz;
	float u, v;
};

struct AOBJHeader {
	unsigned int vertexAmount;
	unsigned int indexAmount;
	wchar_t colorMapFilename[64];
	float refcR, refcG, refcB;
	float gloss;
};

class VertexMap {
	private:
		struct Entry {
			string face;
			int index;
		};

		forward_list<Entry>** mMap;
		unsigned int mListAmount;

		unsigned int getIndex(string face) const {
			unsigned int endIndex;
			endIndex = face.find_first_of('/');

			return stoi(face.substr(0, endIndex)) - 1;
		}

	public:
		VertexMap(unsigned int listAmount) {
			mMap = new forward_list<Entry>*[listAmount];
			mListAmount = listAmount;

			for (int i = 0; i < mListAmount; i++)
				mMap[i] = nullptr;
		}

		~VertexMap() {
			for (int i = 0; i < mListAmount; i++) {
				if (mMap[i] != nullptr)
					delete mMap[i];
			}
			delete[] mMap;
		}

		void addEntry(string face, unsigned int index) {
			unsigned int vertIndex = getIndex(face);
			Entry entry;
			entry.face = face;
			entry.index = index;

			if (mMap[vertIndex] == nullptr) {
				mMap[vertIndex] = new forward_list<Entry>;
			}
			mMap[vertIndex]->push_front(entry);
		}

		bool entryInMap(string face) const {
			unsigned int vertIndex = getIndex(face);
			bool ret = false;

			if (mMap[vertIndex] != nullptr) {
				for (auto itr = mMap[vertIndex]->begin(); itr != mMap[vertIndex]->end(); ++itr) {
					if (itr->face.compare(face) == 0) {
						ret = true;
						break;
					}
				}
			}

			return ret;
		}

		unsigned int findEntryIndex(string face) {
			unsigned int vertIndex = getIndex(face);
			unsigned int ret = 0;

			if (mMap[vertIndex - 1] != nullptr) {
				for (auto itr = mMap[vertIndex]->begin(); itr != mMap[vertIndex]->end(); ++itr) {
					if (itr->face.compare(face) == 0) {
						ret = itr->index;
						break;
					}
				}
			}

			return ret;
		}
};

Report convertObjFile(string filename);
string interpretReport(Report report);
Primitive whatPrimitive(string line);
void stringToWchar(wchar_t* dest, string* source);

int main() {
	string filename;
	cout << "OBJ to AOBJ file converter (c) 2017 Andreas Palmqvist under the MIT Licence." << endl << endl;

	while (true) {
		cout << "(Leave the field empty and press enter to terminate the program)" << endl;
		cout << "Enter the name of the file to convert > ";
		getline(cin, filename);
		if (filename == "")
			break;

		cout << endl;
		cout << interpretReport(convertObjFile(filename)) << endl << endl;
	}

	return 0;
}

Report convertObjFile(string filename) {
	unsigned int mtlErrorCheck = 0;
	bool mtlLocated = false;
	
	string inputFilename, outputFilename;
	inputFilename = filename;
	
	vector<ObjCoord>    objVertices;
	vector<ObjCoord>    objNormals;
	vector<ObjTexCoord> objTexCoords;

	vector<Vertex>       vertices;
	vector<unsigned int> primitives;
	vector<unsigned int> indices;

	AOBJHeader header;
	VertexMap* vertexMap = nullptr;
	unsigned int currentIndex = 0;

	cout << "-Trying to open " << inputFilename << "..." << endl << endl;
	ifstream input;
	input.open(inputFilename, ios::in);
	if (!input.is_open())
		return ERROR_FILE_NOT_FOUND;

	string line, tempStr;

	cout << "-Reading data..." << endl << endl;
	while (getline(input, line)) {
		istringstream inputString(line);
		// Material filereading
		if (line.substr(0, 6) == "mtllib") {
			string mtlLine;
			string mtlFilename;
			inputString >> tempStr >> mtlFilename;

			ifstream mtlInput;
			mtlInput.open(mtlFilename, ios::in);
			if (!mtlInput.is_open())
				return ERROR_MTL_NOT_FOUND;

			while (getline(mtlInput, mtlLine)) {
				istringstream mtlInputString(mtlLine);
				if (mtlLine.substr(0, 2) == "Ns") {
					mtlInputString >> tempStr >> header.gloss;
					mtlErrorCheck++;
				}
				else if (mtlLine.substr(0, 2) == "Ks") {
					mtlInputString >> tempStr >> header.refcR >> header.refcG >> header.refcB;
					mtlErrorCheck++;
				}
				else if (mtlLine.substr(0, 6) == "map_Kd") {
					string colorMapFilenameStr;
					mtlInputString >> tempStr >> colorMapFilenameStr;
					stringToWchar(header.colorMapFilename, &colorMapFilenameStr);
					mtlErrorCheck++;
				}
			}

			if (mtlErrorCheck != 3)
				return ERROR_UNCOMPATIBLE_MTL;
			mtlLocated = true;
		}
		// Vertex reading
		else if (line.substr(0, 2) == "v ") {
			ObjCoord objVertex;
			inputString >> tempStr >> objVertex.x >> objVertex.y >> objVertex.z;
			objVertices.push_back(objVertex);
		}
		// Normal reading
		else if (line.substr(0, 2) == "vn") {
			ObjCoord objNormal;
			inputString >> tempStr >> objNormal.x >> objNormal.y >> objNormal.z;
			objNormals.push_back(objNormal);
		}
		// Texcoord reading
		else if (line.substr(0, 2) == "vt") {
			ObjTexCoord objTexCoord;
			inputString >> tempStr >> objTexCoord.u >> objTexCoord.v;
			objTexCoords.push_back(objTexCoord);
		}
		// Face reading
		else if (line.substr(0, 2) == "f ") {
			unsigned int tempPriIndices[4] = {0};
			if (vertexMap == nullptr) {
				cout << "-Interpreting data..." << endl << endl;
				vertexMap = new VertexMap(objVertices.size());
			}

			int primitive = whatPrimitive(line);
			if (primitive == UNKNOWN) {
				delete vertexMap;
				return ERROR_UNCOMPATIBLE_OBJ;
			}

			string* faceArr = new string[primitive];
			if (primitive == TRIANGLE) {
				inputString >> tempStr >> faceArr[0] >> faceArr[1] >> faceArr[2];
				primitives.push_back(TRIANGLE);
			}
			else if (primitive == QUAD) {
				inputString >> tempStr >> faceArr[0] >> faceArr[1] >> faceArr[2] >> faceArr[3];
				primitives.push_back(QUAD);
			}

			for (int i = 0; i < primitive; i++) {
				if (vertexMap->entryInMap(faceArr[i])) {
					tempPriIndices[i] = vertexMap->findEntryIndex(faceArr[i]);
				}
				else {
					unsigned int vectorIndices[3] = { 0 };
					unsigned int startCut, endCut;
					startCut = endCut = 0;

					for (int j = 0; j < 3; j++) {
						endCut = faceArr[i].find_first_of('/', startCut);
						vectorIndices[j] = stoi(faceArr[i].substr(startCut, (endCut)-startCut));
						startCut = endCut + 1;
					}

					Vertex vertex;
					vertex.x = objVertices.at(vectorIndices[0] - 1).x;
					vertex.y = objVertices.at(vectorIndices[0] - 1).y;
					vertex.z = objVertices.at(vectorIndices[0] - 1).z;

					vertex.nx = objNormals.at(vectorIndices[2] - 1).x;
					vertex.ny = objNormals.at(vectorIndices[2] - 1).y;
					vertex.nz = objNormals.at(vectorIndices[2] - 1).z;

					vertex.u = objTexCoords.at(vectorIndices[1] - 1).u;
					vertex.v = objTexCoords.at(vectorIndices[1] - 1).v;

					vertexMap->addEntry(faceArr[i], currentIndex);

					vertices.push_back(vertex);
					tempPriIndices[i] = currentIndex++;
				}
			}
			if (primitive == TRIANGLE) {
				indices.push_back(tempPriIndices[0]);
				indices.push_back(tempPriIndices[1]);
				indices.push_back(tempPriIndices[2]);
			}
			else if (primitive == QUAD) {
				indices.push_back(tempPriIndices[0]);
				indices.push_back(tempPriIndices[1]);
				indices.push_back(tempPriIndices[2]);
				indices.push_back(tempPriIndices[0]);
				indices.push_back(tempPriIndices[2]);
				indices.push_back(tempPriIndices[3]);
			}

			delete[] faceArr;
		}
	}

	if (!mtlLocated)
		return ERROR_NO_MTL;

	input.close();
	delete vertexMap;

	outputFilename = inputFilename;
	outputFilename.insert(outputFilename.find_last_of('o'), "a");

	cout << "Writing converted data to " << outputFilename << "..." << endl;
	ofstream output;
	output.open(outputFilename, ios::out | ios::binary);

	header.vertexAmount = vertices.size();
	header.indexAmount = indices.size();

	output.write((char*)&header, sizeof(AOBJHeader));
	for (int i = 0; i < vertices.size(); i++)
		output.write((char*)&vertices.at(i), sizeof(Vertex));
	for (int i = 0; i < indices.size(); i++)
		output.write((char*)&indices.at(i), sizeof(unsigned int));

	output.close();

	return SUCCESS;
}

string interpretReport(Report report) {
	switch (report) {
		case ERROR_FILE_NOT_FOUND:
			return "ERROR! File not found.\nEither it is not located in the program folder or you misspelled its name. Be sure to add \".obj\" at the end.";
			break;
		case ERROR_UNCOMPATIBLE_OBJ:
			return "ERROR! The obj file is not compatible with this program.\nThis program only supports obj files that index using either triangles or quads. It also must contain v, vt and vn!";
			break;
		case ERROR_MTL_NOT_FOUND:
			return "ERROR! Mtl not found. \nCheck if the file is referenced correctly in the obj file.";
			break;
		case ERROR_UNCOMPATIBLE_MTL:
			return "ERROR! Uncompatible mtl file. \nCan be many reasons.";
			break;
		case ERROR_NO_MTL:
			return "ERROR! No mtl reference in obj file. \nAdd one.";
			break;
		case SUCCESS:
			return "SUCCESS! The file was successfully converted.";
			break;
	}
}

Primitive whatPrimitive(string line) {
	int count = 0;
	for (int i = 0; i < line.size(); i++) {
		if ((line.at(i) >= 48 && line.at(i) <= 57))
			if (line.at(i - 1) == ' ') {
				count++;
			}
	}

	if (count == TRIANGLE || count == QUAD)
		return (Primitive) count;
	else
		return UNKNOWN;
}

void stringToWchar(wchar_t* dest, string* source) {
	wstring wstr(source->begin(), source->end());
	
	const wchar_t* wcstr = wstr.c_str();
	wcscpy(dest, wcstr);
}