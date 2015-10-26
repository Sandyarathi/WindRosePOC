/*
 * Parallel01_03.cpp
 *
 *  Created on: Oct 25, 2015
 *      Author: aditi
 */

/*
 * Parallel03.cpp
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
const int MAX_NUM_DATA_POINTS = pow(2,27);
const int NUM_OF_THREADS = 4;
const float DELTA_BUCKET = ((float)360/(float)NUM_OF_SECTORS); //equals to 22.5 degrees
const int STRIP_WIDTH = 32;

struct MesoData
{
	int maxDataPoints;
	int numDataPoints;
	float* windDir;
	float* windSpd;
};

typedef int outputData[NUM_OF_SECTORS][NUM_OF_SPEED];

int calcSpeedsBin(float winSpd);

int calcDirectBin(float winDir);

void readData(MesoData & inputData, vector<string> List);

void aggData(MesoData & inputData, outputData & outData);

vector<string> readFileList(string filepath);


int main(){
	struct timeval start, end;
		double delta;

		gettimeofday(&start, NULL);
		cout<<"Hello World... I am processing.." << endl << endl;

		string fileListpath = "/Users/sandyarathidas/Documents/CMPE275_Sandy/Project1/mesonet1/files.txt";
		vector<string> vectorOfFilePaths = readFileList(fileListpath);

		MesoData inputData = {MAX_NUM_DATA_POINTS,
					0,
					(float*)calloc(MAX_NUM_DATA_POINTS, sizeof(float)),
					(float*)calloc(MAX_NUM_DATA_POINTS, sizeof(float))
			};

		outputData outData;

		for(int i=0; i<NUM_OF_SECTORS; i++){
				for(int j=0; j<NUM_OF_SPEED; j++){
					outData[i][j] = 0;
				}
			}

		readData(inputData, vectorOfFilePaths);

		aggData(inputData, outData);

		cout<<"******************** Printing final 2D array *******************************"<< endl;
		for(int i=0; i<NUM_OF_SECTORS; i++){
			for(int j=0; j<NUM_OF_SPEED; j++){
				cout<< outData[i][j] << "\t";
			}
			cout << endl;
		}

		gettimeofday(&end, NULL);
		delta = (end.tv_sec  - start.tv_sec) +
			         ((end.tv_usec - start.tv_usec) / 1.e6);

		cout<< endl;
		printf("%.6lf seconds elapsed\n", delta);

		free(inputData.windDir);
		free(inputData.windSpd);

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


void readData(MesoData & inputData, vector<string> List) {



	string line, stationId="H0024";
	string path = "/Users/sandyarathidas/Documents/CMPE275_Sandy/Project1/mesonet1/";

	omp_set_num_threads(NUM_OF_THREADS);

	MesoData mesoArray[NUM_OF_THREADS]; // global container for all threads

	#pragma omp parallel
	{
		string line;
		int count = 0;

		MesoData localInData ={MAX_NUM_DATA_POINTS,
				0,
				(float*)calloc(MAX_NUM_DATA_POINTS, sizeof(float)),
				(float*)calloc(MAX_NUM_DATA_POINTS, sizeof(float))
		};

		int tid = omp_get_thread_num();

		#pragma omp for
		for (int i = 0; i < List.size(); i++) {
				//cout<<"FileName:"<<List[i]<<endl;
				ifstream inputFile(path + List[i]);

				//ifstream inputFile("Dataset/07-01_mesonet-20010701_2200.csv");
				string rowData[6] ;
				string token;
				int j = 0;
				while (getline(inputFile, line)) {
					istringstream lineStream(line);
					j = 0;
					while (getline(lineStream, token, ',')) {
						rowData[j++] = token;
					}

					if(rowData[0] == stationId){
						//cout<<"StationID "<<rowData[0]<<endl;
						localInData.windDir[count] = strtof(rowData[5].c_str(), NULL);
						localInData.windSpd[count] = strtof(rowData[4].c_str(), NULL);
						count++;
					}

					lineStream.clear();
				}
				inputFile.close();
			}

		localInData.numDataPoints = count;
		//cout<<"Number of data points collected by thread# "<< tid <<" = "<<localInData.numDataPoints<<endl;

		mesoArray[tid] = localInData;  // each thread stores its local container in global container at index equal to its thread Id

	}


	// adding data into global container from local containers of each thread
	int min;
	for(int k=0; k< NUM_OF_THREADS; k++){

		inputData.numDataPoints += mesoArray[k].numDataPoints;

		if(k == 0)
			min=0;
		else
			min += mesoArray[k-1].numDataPoints;

		for(int n=0; n< mesoArray[k].numDataPoints; n++){
				inputData.windDir[min+n] = mesoArray[k].windDir[n];
				inputData.windSpd[min+n] = mesoArray[k].windSpd[n];
			}

		free(mesoArray[k].windDir);
		free(mesoArray[k].windSpd);
	}


	cout<<"Total number of data points= "<<inputData.numDataPoints<<endl;

}

void aggData(MesoData & inputData, outputData & outData){

	omp_set_num_threads(NUM_OF_THREADS);
	int globalPrint = 0, globalNoPrint=0;

	//cout<<"No. of data points = "<< inputData.numDataPoints << endl;

	#pragma omp parallel
	{
		// initialising local output array for each thread
		outputData localOutData;
		int print=0, noPrint=0;

		for(int i=0; i< NUM_OF_SECTORS; i++){
			for(int j=0; j< NUM_OF_SPEED; j++)
				localOutData[i][j] = 0;
		}

		int maxOfStrip = (inputData.numDataPoints/STRIP_WIDTH)*STRIP_WIDTH;

		#pragma omp for
		for(int k=0; k< maxOfStrip; k+= STRIP_WIDTH){

			int __attribute__((aligned(64))) D[STRIP_WIDTH] ;
			int __attribute__((aligned(64))) S[STRIP_WIDTH];

			const float* winDirection = &(inputData.windDir[k]);
			const float* winSpeed = &(inputData.windSpd[k]);

			#pragma vector aligned
			for(int c=0; c<STRIP_WIDTH; c++){
				D[c] = calcDirectBin(winDirection[c]);
				S[c] = calcSpeedsBin(winSpeed[c]);
			}

			for(int c=0; c<STRIP_WIDTH; c++){
				if((D[c]< NUM_OF_SECTORS && D[c]>=0) && (S[c]< NUM_OF_SPEED && S[c]>=0)){
					localOutData[D[c]][S[c]]++;
					print++;
				}
				else
					noPrint++;
			}
		}

//		int rem = inputData.numDataPoints % STRIP_WIDTH;
//		int remIndex = rem + (inputData.numDataPoints/STRIP_WIDTH);

		#pragma omp for
		for(int i= maxOfStrip; i< inputData.numDataPoints;i++){
				int d = calcDirectBin(inputData.windDir[i]);
				int s = calcSpeedsBin(inputData.windSpd[i]);

				if((d< NUM_OF_SECTORS && d>=0) && (s< NUM_OF_SPEED && s>=0)){
					localOutData[d][s]++;
					print++;
				}
				else
					noPrint++;
		}

		for(int i=0; i< NUM_OF_SECTORS; i++){
			for(int j=0; j< NUM_OF_SPEED; j++){
				#pragma omp atomic
				outData[i][j] += localOutData[i][j];
			}
		}

//		#pragma omp atomic
//		globalPrint += print;
//
//		#pragma omp atomic
//		globalNoPrint += noPrint;
	}

//	cout<< "No. of points in wr = "<< globalPrint<< endl;
//	cout<< "No. of points not in wr = "<< globalNoPrint<< endl;
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














