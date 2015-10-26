/*
 * Parallel02_01.cpp
 *
 *  Created on: Oct 25, 2015
 *      Author: aditi
 */
/*
 * Parallel05.cpp
 *
 *  Created on: Oct 25, 2015
 *      Author: aditi
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <sys/time.h>
#include <omp.h>
#include <cmath>


using namespace std;

const int NUM_OF_SECTORS = 16;
const int NUM_OF_SPEED = 5;
const int NUM_OF_MAX_THREADS = 4;
const float DELTA_BUCKET = ((float)360/(float)NUM_OF_SECTORS); //equals to 22.5 degrees
string sId = "H0024";


struct measurements{
	float windSpd;
	float windDir;
};

int calcSpeedsBin(float winSpd);

int calcDirectBin(float winDir);

measurements getMeas(string str);

void aggData(string fileName, int wr[][NUM_OF_SPEED]);

vector<string> readFileList(string filepath);

int main(){
	struct timeval start, end;
	double delta;

	gettimeofday(&start, NULL);

	cout << "Hello world!! \n" << endl;

	int wr[NUM_OF_SECTORS][NUM_OF_SPEED]= {0};

	omp_set_num_threads(NUM_OF_MAX_THREADS);

	string fileListpath = "/Users/sandyarathidas/Documents/CMPE275_Sandy/Project1/mesonet1/files.txt";
	string path = "/Users/sandyarathidas/Documents/CMPE275_Sandy/Project1/mesonet1/";

	vector<string> vectorOfFilePaths = readFileList(fileListpath);

	#pragma omp parallel
	{
		int local_wr[NUM_OF_SECTORS][NUM_OF_SPEED] = {0};
		#pragma omp for nowait
		for(int i=0; i<vectorOfFilePaths.size(); i++){
			aggData(path + vectorOfFilePaths[i],local_wr);
		}

		for(int m=0; m< NUM_OF_SECTORS; m++){
				for(int n=0; n< NUM_OF_SPEED; n++){
					#pragma omp atomic
					wr[m][n] += local_wr[m][n];
				}
		}
	}


	cout<<"***** printing 2D array for windrose algorithm *****" << endl << endl;
	//#pragma omp parallel for
	for(int m=0; m< NUM_OF_SECTORS; m++){
		for(int n=0; n< NUM_OF_SPEED; n++){
			cout << wr[m][n] << "               ";
		}
		cout << endl;
	}

	gettimeofday(&end, NULL);
	delta = (end.tv_sec  - start.tv_sec) +
	         ((end.tv_usec - start.tv_usec) / 1.e6);

	cout<< endl;
	printf("%.6lf seconds elapsed\n", delta);
}

int calcSpeedsBin(float winSpd) {
	if (winSpd == 0)
			return 0;
		else if (winSpd > 0 and winSpd <= 5)
			return 1;
		else if (winSpd > 5 and winSpd <= 15)
			return 2;
		else if (winSpd > 15 and winSpd <= 25)
			return 3;
		else
			return 4;
}

int calcDirectBin(float winDir) {
	/*
	 * Degrees        |   Bucket No.
	 * 0 - 22.5       |     0
	 * 22.5 - 45      |     1
	 * 45 - 67.5      |     2
	 * 67.5 - 90      |     3
	 * 90 - 112.5     |     4
	 * 112.5 - 135    |     5
	 * 135 - 157.5    |     6
	 * 157.5 - 180    |     7
	 * 180 - 202.5    |     8
	 * 202.5 - 225    |     9
	 * 225 - 247.5    |    10
	 * 247.5 - 270    |    11
	 * 270 - 292.5    |    12
	 * 292.5 - 315    |    13
	 * 315 - 337.5    |    14
	 * 337.5 - 360    |    15
	 */

	while(winDir>360){
		winDir -= 360;  // for handling direction degrees > 360, to bring in the reange [0,360]
	}

	if(winDir == 360)
		winDir = 0;		// for coverting 360 into 0

	return (int)(winDir / DELTA_BUCKET);
}

measurements getMeas(string line){
	int j=0;
	string rowData[6], token;
	istringstream lineStream(line);
	measurements meas;

	j = 0;

	while (getline(lineStream, token, ',')) {
			rowData[j++] = token;
	}

	meas.windDir = strtof(rowData[5].c_str(), NULL);
	meas.windSpd = strtof(rowData[4].c_str(), NULL);

	lineStream.clear();

	return meas;
}

void aggData(string fileName, int wr[][NUM_OF_SPEED]){

	ifstream inputFile(fileName);
	string line;
	measurements m1;
	int d,s;

	while (getline(inputFile, line)) {

			size_t found = line.find(sId);

			if(found!= string::npos){
				m1 = getMeas(line);
				//if(m1.stationId == sId){
				s = calcSpeedsBin(m1.windSpd);
				d = calcDirectBin(m1.windDir);

				if(d< NUM_OF_SECTORS and s < NUM_OF_SPEED and d >= 0 and s>=0){
					wr[d][s]++;
				}
				//}
			}
	}

	inputFile.close();

}

vector<string> readFileList(string filepath) {
	vector<string> list;
	ifstream inputfile(filepath);
	string line;
	while (getline(inputfile, line)) {
		list.push_back(line);
	}
	inputfile.close();
	return list;
}

















