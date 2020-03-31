/*      Julian Aguilar
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
#include <time.h>
#include <vector>
using namespace std;

void func(int sockfd) //Heavily inspired by Geeks4Geeks TCP Server/Client Implementation
{ 
    int n = 0; 
    for (;;) {  
	char buff[300];
    bzero(buff, 301);
	cout << "Enter a city name: ";

	if(n==0) //Only need it to do this first time loop runs. Otherwise prints incorrectly
	cin.ignore(INT8_MAX, '\n');

        cin.getline(buff, 300);
        write(sockfd, buff, sizeof(buff)); 
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff)); 
        printf("\n%s", buff); 
        if ((strncmp(buff, "exit", 4)) == 0) { 
            printf("Client Exit...\n"); 
            break; 
        } 
	n++;
    } 
} 
  

int main (int argc, char* argv[])
{
    int listenFd, portNo;
    bool loop = false;
    struct sockaddr_in svrAdd;
    struct hostent *server;
    string hostname;

cout << "Enter the server host name: ";
cin >> hostname;

    if(hostname.length() < 3)
    {
        cerr<<"Please enter a proper host name. "<<endl;
        return 0;
    }


  cout << "Enter the server port number: ";
  cin >> portNo;

    if((portNo > 65535) || (portNo < 2000))
    {
        cerr<<"Please enter a port number between 2000 - 65535"<<endl;
        return 0;
    }       

 cout << endl;

    listenFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);    

    if(listenFd < 0)
    {
        cerr << "Cannot open socket!" << endl;
        return 0;
    }

    
    server = gethostbyname(hostname.c_str());
    
    if(server == NULL)
    {
        cerr << "Host does not exist!" << endl;
        return 0;
    }

    bzero((char *) &svrAdd, sizeof(svrAdd));
    svrAdd.sin_family = AF_INET;
    bcopy((char *) server -> h_addr, (char *) &svrAdd.sin_addr.s_addr, server -> h_length);
    svrAdd.sin_port = htons(portNo);

    int chk = connect(listenFd,(struct sockaddr *) &svrAdd, sizeof(svrAdd));

    if (chk < 0)
    {
        cerr << "Could not connect!" << endl;
        return 0;
    }

    func(listenFd);
    close(listenFd);
}