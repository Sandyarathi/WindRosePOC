/*
 * SerialModified.cpp
 *
 *  Created on: Oct 27, 2015
 *      Author: sandyarathidas
 */

#include<unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <sys/time.h>
#include <array>
using namespace std;

const int NUM_OF_SECTORS = 16;
const int SPEED_BUCKETS = 5;
const float DELTA_BUCKET = ((float)360/(float)NUM_OF_SECTORS); //equals to 22.5 degrees

int calcSpeedBin(float windSpd) {
	if (windSpd == 0)
		return 0;
	else if (windSpd > 0 and windSpd <= 5)
		return 1;
	else if (windSpd > 5 and windSpd <= 15)
		return 2;
	else if (windSpd > 15 and windSpd <= 25)
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

struct MesoData {

	string stationId;
	string observationDate;
	double latitude;
	double longitude;
	double windSpeed;
	double windDirection;
};

MesoData getMeas(string line){
	int j=0;
	string rowData[6], token;
	istringstream lineStream(line);
	MesoData meas;
	j = 0;
	while (getline(lineStream, token, ',')) {
			rowData[j++] = token;
	}
	meas.windDirection = strtof(rowData[5].c_str(), NULL);
	meas.windSpeed = strtof(rowData[4].c_str(), NULL);
	lineStream.clear();
	return meas;
}


void readData(string sId,string fileName, int wr[][SPEED_BUCKETS]){
	//string path ="/Users/sandyarathidas/Documents/CMPE275_Sandy/Project1/mesonet1/";
	string path = "../Data/";
	ifstream inputFile1(path+fileName);
	string line;
	MesoData mesoData;
	while (getline(inputFile1, line)) {
		size_t found = line.find(sId);
		if(found!= string::npos){
			mesoData = getMeas(line);
			int s = calcSpeedBin(mesoData.windSpeed);
			int d = calcDirectBin(mesoData.windDirection);
			if(d< NUM_OF_SECTORS and s < SPEED_BUCKETS and d >= 0 and s>=0){
				wr[d][s]++;
			}
		}
	}
	inputFile1.close();
}


int main(int argc, char* argv[]) {
	//string fileListpath = "/Users/sandyarathidas/Documents/CMPE275_Sandy/Project1/mesonet1/files.txt";
	string fileListpath= "../Data/fileList.txt";
	int wr1[NUM_OF_SECTORS][SPEED_BUCKETS]= {0};
	int wr2[NUM_OF_SECTORS][SPEED_BUCKETS]= {0};
	string sId1 = "H0024", sId2 = "AR628";
	ifstream inputfile(fileListpath);
	string line, path;
	vector<string> vectorOfFilePaths;
	struct timeval start, end;
	double delta;
	gettimeofday(&start, NULL);
	while (getline(inputfile, line)) {
		vectorOfFilePaths.push_back(line);
	}
	inputfile.close();
	for(string path:vectorOfFilePaths){
		readData(sId1,path,wr1);
		readData(sId2,path,wr2);

	}
	gettimeofday(&end, NULL);
	delta = (end.tv_sec  - start.tv_sec) +((end.tv_usec - start.tv_usec) / 1.e6);
	cout<<"Seconds elapsed = "<<delta<<endl<<endl;
	cout << "***********************************************************" << endl;
	cout << "WindRose for[2001-2014] for 15:00 hrs to 16:00 hrs for station : "<<sId1<< endl;
	cout << "***********************************************************"<< endl;

	for (int i = 0; i < NUM_OF_SECTORS; i++) {
		for (int j = 0; j < SPEED_BUCKETS; j++) {
			cout << wr1[i][j] << "\t";
		}
		cout << endl;
	}
	cout << "***************************************************************" << endl;
		cout << "WindRose for[2001-2014] for 15:00 hrs to 16:00 hrs for station :"<<sId2<< endl;
		cout << "***********************************************************"<< endl;

		for (int i = 0; i < NUM_OF_SECTORS; i++) {
			for (int j = 0; j < SPEED_BUCKETS; j++) {
				cout << wr2[i][j] << "\t";
			}
			cout << endl;
		}

	return 0;

}

