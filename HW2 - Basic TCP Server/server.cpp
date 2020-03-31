/*  Julian Aguilar
	PSID: 1576322
	COSC 3360 - Assignment 2
	Compiles and runs in g++

	http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html/

	http://www.cs.uh.edu/~paris/3360/Sockets.html

	https://stackoverflow.com/questions/19107439/c-getline-multiple-variable-types-using-comma-as-delimiter

	http://www.cplusplus.com/reference/sstream/stringstream/str/

	https://www.techiedelight.com/find-index-element-vector-cpp/
	
	https://stackoverflow.com/questions/41176675/reading-data-from-file-into-a-vector-object

	https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/

	https://www.youtube.com/watch?v=WDn-htpBlnU
*/

#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector> 
#include <algorithm>
using namespace std;

static int connFd;

class City{
public:
string CityName;
int CityTemp;
string CityForecast;

City(){CityName = ""; CityTemp=0; CityForecast = "";}

City(string onlyname){CityName = onlyname; CityTemp=0; CityForecast = "";}

City(string tcn, int tct, string tcf){
	CityName = tcn;
	CityTemp = tct;
	CityForecast = tcf;
	}

void PrintCityInfo(City tempCity){
cout << tempCity.CityName << " " << tempCity.CityTemp << " " << tempCity.CityForecast << endl; 
}

string getCityName(City tempCity){
return tempCity.CityName;
}
int getCityTemp(City tempCity){
return tempCity.CityTemp;
}

string getCityForecast(City tempCity){
return tempCity.CityForecast;
}

bool operator== (const City &rhs) 
{
	if (this->getCityForecast(rhs) == "NULL"){ //This is to tell apart temp city from real city.
	return false;
	}
     return CityName == rhs.CityName;
}
};

void fillVector(vector<City>& tempVec, ifstream &tempStream){ //Function to take from .txt file and turn cities into objects and place in vector
string tempCityName, line, str;
string str_tempCityTemp;
string tempCityForecast;

    while (getline(tempStream, line)) 
    {   
	if(line.empty()) {break;}
	
	istringstream ss (line);

    getline(ss, tempCityName, ',');
    getline(ss, str_tempCityTemp, ',');
	int tempCityTemp = stoi(str_tempCityTemp);
    getline(ss, tempCityForecast, ',');

City newCity(tempCityName, tempCityTemp, tempCityForecast);
    tempVec.push_back(newCity);
     }


tempStream.close();
}


int main(int argc, char* argv[])
{

	int pId, portNo, listenFd;
	socklen_t len; //store size of address
	bool loop = false;
	struct sockaddr_in svrAdd, clntAdd;

	//Read in from weather20.txt
	ifstream infile("weather20.txt");
	string line;
	string str;
	string nullflag = "NULL";

	vector<City> CityVector;
	fillVector(CityVector, infile);

	cout << "Enter the server port number: ";
	cin >> portNo;

	if ((portNo > 65535) || (portNo < 2000))
	{
		cerr << "Please enter a port number between 2000 - 65535" << endl;
		return 0;
	}

	//Create socket
	listenFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (listenFd < 0)
	{
		cerr << "Cannot open socket" << endl;
		return 0;
	}

	bzero((char*)& svrAdd, sizeof(svrAdd));

	svrAdd.sin_family = AF_INET;
	svrAdd.sin_addr.s_addr = INADDR_ANY;
	svrAdd.sin_port = htons(portNo);

	//bind socket

	if (bind(listenFd, (struct sockaddr*) & svrAdd, sizeof(svrAdd)) < 0)
	{
		cerr << "Cannot bind" << endl;
		return 0;
	}

	listen(listenFd, 5);

	cout << "Listening" << endl;
	len = sizeof(clntAdd);

	//Client connects here and will hang till then
	connFd = accept(listenFd, (struct sockaddr*) & clntAdd, &len);

	if (connFd < 0)
	{
		cerr << "Cannot accept connection" << endl;
		return 0;
	}
	else
	{
		cout << "Connection successful" << endl << endl;

		char test[300];
		string s_msg = "";
		bool loop = false;
		int n;
		

		while (!loop)
		{
			bzero(test, 301);
			recv(connFd, test, 300, 0);

			stringstream ss;
			ss.str(test);
			string stringbuffer = ss.str();

			City tempCity(stringbuffer,0,nullflag); //Making temporary city with name of user input. 'nullflag' is used to show this is a temp city.
			CityVector.push_back(tempCity);
			City CopiedCity = CityVector.at(distance(CityVector.begin(), (find(CityVector.begin(), CityVector.end(), stringbuffer)))); //copying the actual city from the vector

			if (tempCity == CopiedCity) { // Need to make it so the server doesnt run this first
				cout << "Weather report for " << tempCity.getCityName(CopiedCity) << endl;
				cout << "Tomorrow's maximum temperature is " << tempCity.getCityTemp(CopiedCity) << " F" << endl;
				cout << "Tomorrow's sky condition is " << tempCity.getCityForecast(CopiedCity) << endl << endl;

				s_msg = "Tomorrow's maximum temperature is " + to_string(tempCity.getCityTemp(CopiedCity)) + " F\n";
				s_msg += "Tomorrow's sky condition is " + tempCity.getCityForecast(CopiedCity) + "\n" + "\n";
			}
			else{
			cout << "Weather report for " << tempCity.getCityName(tempCity) << endl;
			cout << "No Data." << endl << endl;

			s_msg = "No data\n\n";
			}
			
			//writing message to buffer
			
			bzero(test, 301);
		for (int m = 0; m < s_msg.length(); m++) {
			test[m] = s_msg[m];
		}
			//Sending buffer to client 
			write(connFd, test, sizeof(test));

			//exit if "exit" is typed
			if (strncmp("exit", test, 4) == 0) {
				printf("Server Exit...\n");
				break;
			}
		}
		cout << "\nShutting down server..." << endl;
		close(connFd);
	}
}