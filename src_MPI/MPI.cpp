/*
 * Approach1.cpp
 *
 *  Created on: Oct 21, 2015
 *      Author: sandyarathidas
 */
#include <mpi.h>
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

	struct timeval start, end;
		double delta;
	vector<MesoData> inData;
	for (int i = 0; i < List.size(); i++) {
		//cout<<"fileName:"<<subList[i]<<endl;
		ifstream inputfile1(path + List[i]);
		//inputfile1.ignore(100, '\n');//To ignore column header line
		string line;
		string rowData[5] ;
		string token;
		int j = 0;
		while (getline(inputfile1, line)) {
			//cout<<"i = " <<i << "; line = " <<line<<endl;
			istringstream lineStream(line);
			j = 0;
			while (getline(lineStream, token, ',')) {
				rowData[j++] = token;
			}
			MesoData mesoData = { rowData[0], rowData[1], atof(rowData[2].c_str()), atof(
								rowData[3].c_str()), atof(rowData[4].c_str()),atof(rowData[5].c_str()) };
			if(mesoData.stationId==stationId){

				inData.push_back(mesoData);
				//cout<<mesoData.stationId<<endl;
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
		//istringstream lineStream(line);
		list.push_back(line);
	}
	inputfile.close();
	return list;
}

vector<string> fileSubList(vector<string> entireFileList, int no_of_processes,
		int rank) {
	vector<string> subList;
	int maxFiles = entireFileList.size() / no_of_processes;
	for (int i = rank * maxFiles; i < maxFiles + (rank * maxFiles); i++) {
		//cout<<"Iteration"<<i<<endl;
		if (i < entireFileList.size()) {
			subList.push_back(entireFileList[i]);
		}
	}
	return subList;
}

int main(int argc, char* argv[]) {

	bool debug = false;

	string fileListpath = "/Users/sandyarathidas/Documents/CMPE275_Sandy/Project1/mesonet1/files.txt";
	string stationChoice="AR628";

	MPI::Init(argc, argv);
	//  Get the number of processes.
	int no_of_processes = MPI::COMM_WORLD.Get_size();

	//Determine the current processes's rank.
	int rank_of_current_process = MPI::COMM_WORLD.Get_rank();

	vector<string> vectorOfFilePaths = readFileList(fileListpath);
	vector<string> processSubList = fileSubList(vectorOfFilePaths,
			no_of_processes, rank_of_current_process);

	/*debug help code*/
	if (debug) {
		{
			int i = 0;
			printf("PID %d ready for attach\n", getpid());
			fflush(stdout);
			while (0 == i)
				sleep(5);
		}
	}
	vector<MesoData> inputData = readData(processSubList,stationChoice);

	auto resultArray = aggData(inputData);
	auto finalArray = new int[NUM_OF_SECTORS][SPEED_BUCKETS]();
	MPI::COMM_WORLD.Reduce(resultArray, finalArray,
			NUM_OF_SECTORS * SPEED_BUCKETS, MPI::INT, MPI::SUM, 0);
	if (rank_of_current_process == 0) {
		cout << endl;
		cout << "...................................................." << endl;
		cout << "                  Final 2D Array                    " << endl;
		cout << "...................................................." << endl;

		for (int i = 0; i < NUM_OF_SECTORS; i++) {
			for (int j = 0; j < SPEED_BUCKETS; j++) {
				cout << finalArray[i][j] << "\t";
			}
			cout << endl;
		}
	}
	MPI::Finalize();
	return 0;

}

