#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <stdlib.h>
#include <math.h>

using namespace std;

const double epsilon = .000000001;
double deltaZ;
double deltaY;
double deltaX;
double currentZ;

struct vertex{
	double X;
	double Y;
	double Z;
	float color;
};

void march(float vert[], double X, double Y,  ofstream &tris);
void drawTris(vertex current, vertex v[], float vert[], ofstream &tris);
void drawQuad(vertex v[], ofstream &tris);
void drawHex(vertex v[], ofstream &tris);
void toFile(vertex midpoints[],  ofstream &tris);
void findNeighbors(int neighbors[], int i);
void vertexToCoord(float vert[], vertex v, int i, double X, double Y);


int main(int argc, char *argv[])
{
	float vertices[8];
	double left;
	double bottom;
	double front;


	int size;
	string readStr;
	char readcStr[32];
	char filename[24];

	//read in size from 1.dat
	ifstream slice ("slices/1.dat");
	if(!slice.is_open()) cout << "1.dat not read\n";
	////ifstream slice2 ("slices/2.dat");
	ofstream tris ("tris.txt");

	//ifstream* slicei1 = &slice1;
	//ifstream* slicei2 = &slice2;
	//ifstream* tmp;

	getline(slice,readStr);
	size = atoi(readStr.c_str());

	getline(slice,readStr);
	front = atof(readStr.c_str());

	getline(slice,readStr);
	left = atof(readStr.c_str());

	getline(slice,readStr);
	bottom = atof(readStr.c_str());

	getline(slice,readStr);
	deltaX = atof(readStr.c_str());

	getline(slice,readStr);
	deltaY = atof(readStr.c_str());

	getline(slice,readStr);
	deltaZ = atof(readStr.c_str());


	float **array1;//[size][size];
	array1 = new float *[size];
	float **array2;//[size][size];
	array2 = new float *[size];
	for(int i = 0; i < size; i++) {
		array1[i] = new float[size];
		array2[i] = new float[size];
	}
	//float array3[size][size];

	for(int j = 0; j < size; j++) {
		for(int k = 0; k < size; k++) {
			slice.getline(readcStr,32,' ');
			array1[j][k] = (float)atof(readcStr);
		}
	}

	slice.close();
	slice.clear();
	//slice.open("slices/2.dat");
	sprintf(filename,"slices/%d.dat",2);
	slice.open(filename);
	//    if(slice.is_open())
	//	cout << filename << " opened\n" ;
	//    else
	//	cout << filename << "file not opening\n";

	//this loop reads in and operates on each slice
	for(int i = 0; i < size; i++){
		currentZ = front + i * deltaZ;
		for(int j = 0; j < size; j++) {
			for(int k = 0; k < size; k++) {
				slice.getline(readcStr,32,' ');
				//cout << readcStr << endl;
				array2[j][k] = (float)atof(readcStr);
			}
		}

		for(int j = 0; j < size - 1; j++) {
			//left 4 vertices
			vertices[0] = array1[j][1];
			vertices[1] = array1[j][0];
			vertices[4] = array2[j][1];
			vertices[5] = array2[j][0];

			for(int k = 0; k < size - 1; k++) {
				//right 4 vertices
				vertices[3] = array1[j+1][k+1];
				vertices[2] = array1[j+1][k];
				vertices[7] = array2[j+1][k+1];
				vertices[6] = array2[j+1][k];

				march(vertices,left + j * deltaX, bottom + k * deltaY, tris);

				//moving right
				vertices[0] = vertices[3];
				vertices[1] = vertices[2];
				vertices[4] = vertices[7];
				vertices[5] = vertices[6];

			}
		}
		//move slice i+1 to i, load in next slice as i+1
		slice.close();
		slice.clear();
		cout << " loop " << i << ": ";
		if(slice.is_open()) cout << "fail!";	
		sprintf(filename,"slices/%d.dat",i+3);
		slice.open(filename);
		if(slice.is_open())
			cout << filename << " opened" << endl;
		else
			cout << filename << " not opening" << endl;
	}

}

void march(float vert[], double X, double Y,  ofstream &tris) {
	//X and Y are the coordinates v[0]
	//Z is global, as are the deltas which are used to calcuate the other vertices

	int count = 0;//count is the number of vertices
	int i = 0;//i is the index of the first vertex in the cube
	int neighbors[3];//the index of the neighbors of the first vertex
	//vertex current; current.X = current.Y =  current.Z =  current.color = 0;//the coordinates of the first vertex
	vertex current;
	current.X = X;
	current.Y = Y;
	current.Z = currentZ;
	current.color = vert[0];
	vertex v[3]; //where the 3 vertices are stored before their midpoints are calculated

	for(i = 0; i < 8;i++){
		if(vert[i] != -1)
			count++;
	}

	if(count == 0 || count == 8){
		//case 0: No triangles in this cube, draw nothing
		//cout << "Count = 0\n";

	} else if(count == 1 || count == 7){

		cout << "Count = 1\n";
		//case 1
		i = 0;
		if(count == 1){
			while(vert[i] == -1)
				i++;
		}
		else {
			while(vert[i] != -1)
				i++;
		}
		findNeighbors(neighbors, i);

		vertexToCoord(vert, current,i,X,Y);

		for( int c = 0; c < 3; c++)
			vertexToCoord(vert, v[c],neighbors[c],X,Y);
		drawTris(current, v, vert ,tris);

	} else if(count == 2 || count == 6){
		cout << "Count = 2\n";
		int neighbors2[3];
		vertex current2, squareVerts[4];
		int i2 = 0;

		i = 0;

		if(count == 2){
			while(vert[i] == -1)
				i++;
			i2 = i;
			while(vert[i2] == -1)
				i2++;
		}
		else {
			while(vert[i] != -1)
				i++;
			i2 = i;
			while(vert[i2] != -1)
				i2++;
		}
		findNeighbors(neighbors,i);
		findNeighbors(neighbors2,i2);
		vertexToCoord(vert, current,i,X,Y);
		vertexToCoord(vert, current2,i2,X,Y);

		int neighborCount = 0;
		for(int c = 0; c < 3; c++) {
			if(neighbors[c] == i2) {

				//case 2
				//for the 2 neighbors of i that aren't i2
				// find midpoints between them and current
				vertexToCoord(vert, v[0],neighbors[(c+1)%3],X,Y);
				vertexToCoord(vert, v[1],neighbors[(c+2)%3],X,Y);

				for(int d = 0; d < 2; d++){
					squareVerts[d].X = (current.X + v[d].X) / 2;
					squareVerts[d].Y = (current.Y + v[d].Y) / 2;
					squareVerts[d].Z = (current.Z + v[d].Z) / 2;
				}
				for(int d = 0; d < 3; d++) {
					vertexToCoord(vert, v[d],neighbors2[d],X,Y);
				}

				for(int d = 2; d < 4; d++){
					squareVerts[d].X = (current2.X + v[d].X) / 2;
					squareVerts[d].Y = (current2.Y + v[d].Y) / 2;
					squareVerts[d].Z = (current2.Z + v[d].Z) / 2;
				}
				drawQuad(squareVerts, tris);
				//drawTris(squareVerts[0],squareVerts[1],squareVerts[2],tris);
				//vertex tempVert[3];
				//tempVert[0] = squareVerts[0]; tempVert[1] = squareVerts[1]; tempVert[2] = squareVerts[2];
				//toFile(tempVert,tris);
				////drawSquare(current, v, vert, tris);
				//if(squareVerts[0].X == squareVerts[2].X ||
				//	squareVerts[0].Y == squareVerts[2].Y ||
				//	squareVerts[0].Z == squareVerts[2].Z) {
				//		tempVert[0] = squareVerts[1]; tempVert[1] = squareVerts[2]; tempVert[2] = squareVerts[3];
				//		toFile(tempVert,tris);
				//}
				//else {
				//	tempVert[0] = squareVerts[0]; tempVert[1] = squareVerts[2]; tempVert[2] = squareVerts[3];
				//	toFile(tempVert,tris);
				//}

				c = 3;
				neighborCount = -1;
				break;
			}
			for(int d = 0; d < 3; d++){
				if(neighbors[c] == neighbors2[d])
					neighborCount++;
			}
		}
		if (neighborCount != -1){
			//case3 and case4 also involves only drawing 2 instances of class 1
			//This was originally 2 separate cases determined by the number of shared neighbors
			for( int c = 0; c < 3; c++) 
				vertexToCoord(vert, v[c],neighbors[c],X,Y);
			drawTris(current, v, vert, tris);

			for( int c = 0; c < 3; c++)
				vertexToCoord(vert, v[c],neighbors2[c],X,Y);
			drawTris(current, v, vert,tris);
		}
	}
	else if(count == 3 || count == 5){
		cout << "Count = 3\n";
		int index[] = {0,0,0};//stores the value of the index in vert of the vertices.
		int neighbors[3][3];//for each corresponding vertex in index, it's neighbors are put in here.
		//vertex current[3];
		vertex midpoints[3];//The midpoints that form the triangles to be written.
		vertex v[3][3];//where the 3 vertices are stored before their midpoints are calculated, for each vertex

		i = 0;
		for(int j = 0; j < 3; j++) {
			while(vert[i] == -1){
				i++;
			}
			index[j] = i;
		}

		for(int j = 0; j < 3; j++) {
			findNeighbors(neighbors[j], index[j]);
			vertexToCoord(vert, current, index[j],X,Y);
		}

		int sumCount = 0;
		int iterCount = 0;
		//This uses the number of neighbors to create a unique number for each case
		for(int j = 0; j < 3; j++) {//for each vertex in neighbors
			iterCount = 0;
			for(int k = 0; k < 3; k++) {//go through it's list of neighbors
				for(int l = 0; l < 3; l++) {//check against the list of (filled) vertices
					if(neighbors[j][k] == index[l]) {//if one of the neighbors of this vertex is a filled vertex
						sumCount+= pow(10.0,iterCount);
						iterCount++;
					}
				}
			}
		}

		switch(sumCount) {
		case 13: //case 5
			break;
		case 2://case 6

			//draw triangle

			//draw square

			break;
		case 0:	//case7 (no neighbors)

			for(int j = 0; j < 3; j++) { //for each vertex
				for(int k = 0; k < 3; k++) {  //for each neighbor
					vertexToCoord(vert, v[j][k],neighbors[j][k],X,Y);
				}
			}

			for(int j = 0; j < 3; j++) {//for each triangle
				//for(int k = 0; k < 3; k++) { //for each vertex
				drawTris(current, v[j], vert,tris);
			}
			break;
		}

	} else if(count == 4){
		cout << "Count = 4\n";
		int index[] = {0,0,0,0};
		int neighbors[4][3];
		vertex midpoints[4], v[4][3]; //current[4]
		int k = 0;
		int flag = 0;
		int midIndex = 0;
		
		int neighborAddress = -1;//index[neighborAddress];
		int secondIndex[2];//for second square

		i = 0;
		for(int j = 0; j < 4; j++) {
			while(vert[i] == -1){
				i++;
			}
			index[j] = i;
		}

		for(int j = 0; j < 4; j++) {
			findNeighbors(neighbors[j], index[j]);
			vertexToCoord(vert, current, index[j], X ,Y);
		}

		int sumCount = 0;
		int iterCount = 0;
		//This uses the number of neighbors to create a unique number for each case
		for(int j = 0; j < 4; j++) {
			iterCount = 0;
			for(int k = 0; k < 3; k++) {
				for(int l = 0; l < 4; l++) {
					if(neighbors[j][k] == index[l]) {
						sumCount+= pow(10.0,iterCount);
						iterCount++;
					}
				}
			}
		}

		switch(sumCount) {
		case 44:
			//case 8
			int l;
			//if each node has exactly2 neighbors that are vertices, degree sequence {2,2,2,2}
			for(int j = 0; j < 4; j++) {//going through each vertex
				for(int k = 0; k < 3; k++) { //for each neighbor
					for(l = 0; l < 4;l++) { //for each vertex
						if(neighbors[j][k] == index[l]) {//if neighbor is a vertex
							break;
						}
					}
					if(l == 4) {//if this neighbor is not a vertex
						vertexToCoord(vert, v[0][j],neighbors[j][k],X,Y);
						break;
					}
				}
			}
			//I think this was correct
			for( int i = 0; i < 4; i++) {
				//cycle through currents
				vertexToCoord(vert, current, index[i],X,Y);//double Check this
 				for(int c = 0; c < 4; c++) {
					midpoints[c].X = (current.X + v[0][c].X) / 2;
					midpoints[c].Y = (current.Y + v[0][c].Y) / 2;
					midpoints[c].Z = (current.Z + v[0][c].Z) / 2;
					midpoints[c].color = vert[index[i]];//DCthis
				}
			}

			drawQuad(midpoints,tris);
			break;
		case 114:
			//case 9
			//determine if it has one point that has the other 3 vertices as it's neighbors

			//drawHex
			//take first midpoint, find other 2 midpoints that have either same X, Y or Z coordinates(these will be used as the corners of the square.
			//

			break;
		case 4:
			//case 10
			//Notes: Only using the first array in v, or as I need it.
			//vertex midpoints[4];
			
			vertex N1;N1.X = -1;
			//cycle through vert until a vertex has been found.
			//int neighborAddress = -1;//index[neighborAddress];
			//int secondIndex[2];//for second square
			vertexToCoord(vert, current, index[0],X,Y);
			for(int j = 0; j < 3; j++) {//for each neighbor of the first vertex
				for( k = 0; k < 4; k++) {
					if(neighbors[0][j] == index[k]) {//if neighbor is an included vertex
						neighborAddress = k;
						//Now to setup for the second square
						for(int l = 1; l < 4; l++) {
							if(index[l] != neighbors[0][j]) {//index[j]) {
								secondIndex[0] = l;//index[l];
								break;
							}
						}
						for(int l = 1; l < 4; l++) {
							if(index[l] != index[j] && index[l] != secondIndex[0]) {
								secondIndex[1] = l;//index[l];
								break;
							}
						}
						//break;
					}
				}
				if(k == 4){//this neighbor is not an included vertex
					flag = 0;
					if(N1.X != -1)
						flag = 1;
					vertexToCoord(vert, N1,neighbors[0][j],X,Y);
					midpoints[flag].X = (current.X + N1.X) / 2;
					midpoints[flag].Y = (current.Y + N1.Y) / 2;
					midpoints[flag].Z = (current.Z + N1.Z) / 2;
					midpoints[flag].color = vert[index[0]];//Need better way to get color
				}
			}

			//Now to add the 2 midpoints for the second vertex of the first square.
			N1.X = -1;
			flag = 0;
			vertexToCoord(vert, current, index[neighborAddress],X,Y);
			for(int j = 0; j < 3; j++) {//for each neighbor
				if(neighbors[neighborAddress][j] != index[0]) {
					vertexToCoord(vert, N1,neighbors[0][j],X,Y);

					if(j == 2) {
						midpoints[2+((flag+1)%2)].X = (current.X + N1.X) / 2;
						midpoints[2+((flag+1)%2)].Y = (current.Y + N1.Y) / 2;
						midpoints[2+((flag+1)%2)].Z = (current.Z + N1.Z) / 2;
						midpoints[2+((flag+1)%2)].color = vert[index[neighborAddress]];//Need better way to get color
					} else {
						flag = j;
						midpoints[2+j].X = (current.X + N1.X) / 2;
						midpoints[2+j].Y = (current.Y + N1.Y) / 2;
						midpoints[2+j].Z = (current.Z + N1.Z) / 2;
						midpoints[2+j].color = vert[index[neighborAddress]];//Need better way to get color
					}
				}
			}
			drawQuad(midpoints, tris);

			midIndex = 0;
			//second square
			for(int j = 0; j < 2; j++) {
				vertexToCoord(vert, current, index[secondIndex[j]],X,Y);
				for(int k = 0; k < 3; k++) { //for each neighbor 
					flag = 0;
					if(neighbors[secondIndex[j]][k] != secondIndex[(j+1)%2]) {
						vertexToCoord(vert, N1, neighbors[secondIndex[j]][k],X,Y);
						midpoints[midIndex].X = (current.X + N1.X) / 2;
						midpoints[midIndex].Y = (current.Y + N1.Y) / 2;
						midpoints[midIndex].Z = (current.Z + N1.Z) / 2;
						midpoints[midIndex].color = vert[index[secondIndex[j]]];//Need better way to get color
						midIndex++;
					}
				}
			}
			break;
		case 13:
			//case 12
			//detect one singleton and a path on 3

			break;
		case 0:
			//case 13
			//if none of the other vertices are neighbors for the

			for(int j = 0; j < 3; j++) { //for each vertex
				for(int k = 0; k < 3; k++) {  //for each neighbor
					vertexToCoord(vert, v[j][k],neighbors[j][k],X,Y);
				}
			}

			for(int j = 0; j < 4; j++) {//for each triangle
				drawTris(current, v[j], vert, tris);
			}
			break;
		case 24:
			//case 11/14

			break;
		}
	}
	else
		cout << "No cases selected." << endl;
}

void drawTris(vertex current, vertex v[], float vert[], ofstream &tris){
	vertex midpoints[3];
	for(int i = 0; i < 3; i++) { //for each vertex
		midpoints[i].X = (current.X + v[i].X) / 2;
		midpoints[i].Y = (current.Y + v[i].Y) / 2;
		midpoints[i].Z = (current.Z + v[i].Z) / 2;
		midpoints[i].color = (current.color + v[i].color) / 2;
	}
	toFile(midpoints, tris);	
}

void drawQuad(vertex v[], ofstream &tris){
	vertex triangle[3];
	for(int i = 0; i < 3; i++)
		triangle[i] = v[i];
	toFile(triangle, tris);

	//Assuming that all points are on one plane
	vertex slope1, slope2;
	slope1.X = abs(v[0].X - v[3].X);
	slope1.Y = abs(v[0].Y - v[3].Y);
	slope1.Z = abs(v[0].Z - v[3].Z);
	slope2.X = abs(v[1].X - v[2].X);
	slope2.Y = abs(v[1].Y - v[2].Y);
	slope2.Z = abs(v[1].Z - v[2].Z);
	//If Line 1-4 is perpendicular to line 2-3, then case 2 occurs

	//simpler method: if 1-4 is parallel to 2-3, then case 1 occurs
	if(slope1.X - slope2.X < epsilon && slope1.Y - slope2.Y < epsilon  && slope1.Z - slope2.Z < epsilon ){
		triangle[0] = v[0];
		triangle[1] = v[2];
		triangle[2] = v[3];
	} else {
		triangle[0] = v[1];
		triangle[1] = v[2];
		triangle[2] = v[3];
	}
	toFile(triangle,tris);

}

void drawHex(vertex v[], ofstream &tris) {
	//drawHex
	//take first midpoint, find other 2 midpoints that have either same X, Y or Z coordinates(these will be used as the corners of the square.
	//
}

void toFile(vertex midpoints[],  ofstream &tris) {
	//This prevents the triangles that have 2 or more points = 0 from being written.
	int count = 0;
	for(int i = 0; i < 3; i++) {
		if(abs(midpoints[i].X - midpoints[(i+1)%3].X) < epsilon && abs(midpoints[i].Y - midpoints[(i+1)%3].Y) < epsilon && abs(midpoints[i].Z - midpoints[(i+1)%3].Z) < epsilon)
			count++;
	}
	if(count > 1) {
		//cout << "Triangle has no area\n";
		return;
	}
	for(int i = 0; i < 3; i++) {
		tris << midpoints[i].X << " " << midpoints[i].Y << " " << midpoints[i].Z << " " << midpoints[i].color << endl;
	}
}

void findNeighbors(int neighbors[], int i) {
	//assume
	//find coords of current vertex
	switch ( i )
	{
	case 0:
		neighbors[0] = 3;
		neighbors[1] = 1;
		neighbors[2] = 4;
		break;
	case 1:
		neighbors[0] = 2;
		neighbors[1] = 0;
		neighbors[2] = 6;
		break;
	case 2:
		neighbors[0] = 2;
		neighbors[1] = 3;
		neighbors[2] = 6;
		break;
	case 3:
		neighbors[0] = 0;
		neighbors[1] = 2;
		neighbors[2] = 7;
		break;
	case 4:
		neighbors[0] = 7;
		neighbors[1] = 5;
		neighbors[2] = 0;
		break;
	case 5:
		neighbors[0] = 6;
		neighbors[1] = 4;
		neighbors[2] = 1;
		break;
	case 6:
		neighbors[0] = 5;
		neighbors[1] = 7;
		neighbors[2] = 2;
		break;
	case 7:
		neighbors[0] = 4;
		neighbors[1] = 6;
		neighbors[2] = 3;
		break;
	}
}

void vertexToCoord(float vert[], vertex v, int i, double X, double Y){
	///given the coords lower left vertex, calculate any of the other 7 vertices.
	if(i == 0 || i == 1 || i == 0 || i == 5)
		v.X = X;
	else
		v.X = X + deltaX;

	if(i == 1 || i == 2 || i == 5|| i == 6)
		v.Y = Y;
	else
		v.Y = Y + deltaY;

	if(i < 4)
		v.Z = currentZ;
	else
		v.Z = currentZ + deltaZ;

	v.color = vert[i];

}
