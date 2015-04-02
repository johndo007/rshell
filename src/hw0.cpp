#include<iostream>
#include<unistd.h>
#include<sys/type.h>
#include<sys/wait.h>

using namespace std;

int main(int argc, char *argv[])
{
    bool bloop = true;	
	while (bloop == true)						//keep asking user for commands
	{
		if (getlogin() != NULL) 				//only runs if there is a login 
		{	
			cout << getlogin() << "@";			//display login and @
		}
		char hostname[255] = {0};	        	//max bytes is 255 in hostname
		if (gethostname(hostname, 255) != -1)   //if there is a hostname
		{
			gethostname(hostname, 255);			//put hostname -> hostname
			cout << hostname << " ";			//cout host name
		}
		
		cout << "$" << " ";	//prompt $

		string commands;		
		getline(cin, commands);					//user input commands
		if (commands.find("exit") != std::string::npos) 
		{
			exit(0);							//exit if prompted
		}
		
		
		///testing user input
		cout<<cmd<<endl;
	}

  return 0;
}
