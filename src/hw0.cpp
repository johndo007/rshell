#include<iostream>
#include<unistd.h>
//#include<sys/type.h>
//#include<sys/wait.h>
#include<cstdlib>
#include<cstdio>
#include<queue>
using namespace std;

void list_cmd(string& cmds)	//converts string into individal commands
{
	///convert to string to cmds
	
	queue<string>str_cmd;	//temporary command buffer
	string temp_cmd = "";	//temporary command
	unsigned int str_len = cmds.size();	//string size	
	
	for(int j=0;j<str_len;j++)
	{
		temp_cmd += cmds[j];
		///check for connectors {|| or && or ;}	
		if(cmds[j]==';')
		{
			str_cmd.push(temp_cmd);
			temp_cmd="";
			if(cmds[j++]==' ') j++;
		}
		if(j==(str_len-1)) str_cmd.push(temp_cmd);	
	}
	
	/*	
	cout<<"Test multiple cmd line"<<endl; 
	while(!str_cmd.empty())
	{
		cout<<str_cmd.front()<<endl;
		str_cmd.pop();
	}
	
	*/	
}

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
			exit(0);							//exit if prompted by user
			//return 0;
		}
		
		
		///testing user input
        	//cout<<commands<<endl;
		
		/// break up commands
		list_cmd(commands);
	}

  return 0;
}
