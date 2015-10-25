/*
 * Serial.cpp
 *
 *  Created on: Oct 21, 2015
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

int calcDirectBin(double windDir) {
	return windDir / NUM_OF_SECTORS;
}

struct MesoData {

	string stationId;
	string observationDate;
	double latitude;
	double longitude;
	double windSpeed;
	double windDirection;
};


vector<MesoData> readData(vector<string> List,string stationId) {
	string path ="/Users/sandyarathidas/Documents/CMPE275_Sandy/Project1/mesonet1/";
	vector<MesoData> inData;
	for (int i = 0; i < List.size(); i++) {
		ifstream inputfile1( path+List[i]);
		string line;
		string rowData[6] ;
		string token;
		int j = 0;
		while (getline(inputfile1, line)) {
			istringstream lineStream(line);
			j = 0;
			while (getline(lineStream, token, ',')) {
				rowData[j++] = token;
			}
			MesoData mesoData = { rowData[0], rowData[1], atof(rowData[2].c_str()), atof(
					rowData[3].c_str()), atof(rowData[4].c_str()),atof(rowData[5].c_str()) };
			if(mesoData.stationId==stationId){

				inData.push_back(mesoData);
			}
			lineStream.clear();
		}
		inputfile1.close();
	}
	return inData;
}

auto aggData(vector<MesoData> data) {
	auto wr = new int[NUM_OF_SECTORS][SPEED_BUCKETS]();
	for (MesoData m : data) {
		int s = calcSpeedBin(m.windSpeed);
		int d = calcDirectBin(m.windDirection);
		if (s < SPEED_BUCKETS and d < NUM_OF_SECTORS and d >= 0 and s>=0)
			wr[d][s]++;
	}
	return wr;
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


int main(int argc, char* argv[]) {
	string fileListpath = "/Users/sandyarathidas/Documents/CMPE275_Sandy/Project1/mesonet1/files.txt";
	string stationChoice="AR628";
	vector<string> vectorOfFilePaths = readFileList(fileListpath);

	vector<MesoData> inputData = readData(vectorOfFilePaths, stationChoice);
	cout<<"Final Vector Size = "<<inputData.size()<<endl;
	auto resultArray = aggData(inputData);
		cout << endl;
		cout << "**********************************************************************************" << endl;
		cout << "WindRose Plot Data for Oak Springs for years[2001-2014] for 15:00 hrs to 16:00 hrs"<< endl;
		cout << "**********************************************************************************"<< endl;

		for (int i = 0; i < NUM_OF_SECTORS; i++) {
			for (int j = 0; j < SPEED_BUCKETS; j++) {
				cout << resultArray[i][j] << "\t";
			}
			cout << endl;
		}
	return 0;

}

