

#include <mpi.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>

#include <array>
using namespace std;

const int NUM_OF_SECTORS = 16;
const int SPEED_BUCKETS=5;

int calcSpeedBin(float windSpd) {
	if ( windSpd == 0)
		return 0;
	else if ( windSpd > 0 and windSpd <= 5)
		return 1;
	else if ( windSpd > 5 and windSpd <= 15)
		return 2;
	else if (windSpd > 15 and windSpd <= 25)
		return 3;
	else return 4;
}



struct MesoData
{

	//int year;
	//int month;
	//int day;
	string stid;
	double windSpd;
	int windDir;
};

vector<MesoData> readData(string file_name) {
  vector<MesoData> inData;
  ifstream  inputfile1(file_name);
  inputfile1.ignore(100, '\n');//To ignore column header line
    string line;
    while(getline(inputfile1,line))
    {
      bool setflag =false;
      string rowData[3];
      istringstream lineStream(line);
      string token;
      int i=0;
      while(getline(lineStream,token,',')){
    	if(!rowData[0] == "AP005" ){
    		setflag=true;
    		break;
    	}
    	rowData[i++]=token;
      }
      if(setflag)
    	  continue;
       MesoData mesoData ={
        //atoi(rowData[0].c_str()),
        //atoi(rowData[1].c_str()),
        //atoi(rowData[2].c_str()),
        rowData[0],
        atoi(rowData[1].c_str()),
        atof(rowData[2].c_str())
      };
    if(mesoData.stid == "AP005")
    	inData.push_back(mesoData);
    }
    return inData;
}



auto aggData(vector<MesoData> data) {
	auto wr = new int[NUM_OF_SECTORS][SPEED_BUCKETS]();
	for (MesoData m : data) {
		int s = calcSpeedBin(m.windSpd);
		int d = calcDirectBin(m.windDir);
		if (s < SPEED_BUCKETS and d < NUM_OF_SECTORS and d >= 0 and s>=0)
			wr[d][s]++;
	}
	return wr;
}

int main (int argc, char* argv[]){

  
  //Initialize MPI
  MPI::Init ( argc, argv );
  string file_names[] ={"../Data/ACME_2011.csv",
  "../Data/ACME_2012.csv",
  "../Data/ACME_2013.csv",
  "../Data/ACME_2014.csv",
  "../Data/ACME_2015.csv",
  "../Data/WASH_2011.csv",
  "../Data/WASH_2012.csv",
  "../Data/WASH_2013.csv",
  "../Data/WASH_2014.csv",
  "../Data/WASH_2015.csv"};
  string fileName;
  //  Get the number of processes.
  int no_of_processes = MPI::COMM_WORLD.Get_size ( );
  //Determine the current processes's rank.
  int rank_of_current_process = MPI::COMM_WORLD.Get_rank ( );
  if(rank_of_current_process == 0){

  	fileName = file_names[0];
  	for(int i=1;i<no_of_processes;i++){
      //wtime = MPI::Wtime ( );
  		MPI::COMM_WORLD.Send(file_names[i].c_str(), file_names[i].size(), MPI::CHAR, i, 0);
 	}
  }
  else{
  	char *file;
  	file = new char[100];
  	MPI::COMM_WORLD.Recv(file,100, MPI::CHAR, 0,0);
  	fileName= string(file);
  }
  //cout << "My rank=" << rank_of_current_process << "\t FileName"<<fileName<<endl;
  vector<MesoData> inputData= readData(fileName);
  auto resultArray = aggData(inputData);
  auto finalArray= new int[NUM_OF_SECTORS][SPEED_BUCKETS]();
  MPI::COMM_WORLD.Reduce(resultArray, finalArray,NUM_OF_SECTORS* SPEED_BUCKETS, MPI::INT,MPI::SUM, 0);
 if(rank_of_current_process == 0){
    cout<<endl;
    cout<<"...................................................."<<endl;
    cout<<"                  Final 2D Array                    "<<endl;
    cout<<"...................................................."<<endl;

    for (int i = 0; i < NUM_OF_SECTORS; i++){
      for (int j = 0; j < SPEED_BUCKETS; j++){
          cout << finalArray[i][j] <<"\t";
      }
      cout <<endl;
    }
  }
  MPI::Finalize();
  return 0;

}
