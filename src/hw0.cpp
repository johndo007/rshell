#include<iostream>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<cstdlib>
#include<cstdio>
#include<queue>
using namespace std;

void prompt()
{
	if (getlogin() != NULL) 				//only runs if there is a login 
	{	
		cout << getlogin() << "@";			//display login and @
	}
	char hostname[127] = {0};	        	//max bytes is 128 in hostname
	if (gethostname(hostname, 255) != -1)   //if there is a hostname
	{
		gethostname(hostname, 255);			//put hostname -> hostname
		cout << hostname << " ";			//cout host name
	}
	cout << "$" << " ";						//prompt $
}

void list_cmd(string& cmds, queue<string>&str_cmd)	//converts string into individal commands
{
	///convert to string to cmds
	
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
}

void exec_cmd(queue<string> &str_cmd)
{
	//begin executing commands
	while(!str_cmd.empty())
	{
		string curr_cmd = str_cmd.front();

		int pid = fork();
		if(pid == -1)	//error value is -1
		{
			perror("fork ");
			exit(1);
		}
		else if(pid == 0)
		{
			//cout<<"This is the child process ";
			if(execvp("ls",argv) == -1)
			{
				perror("Command failed to run ");
				exit(1);
			}
			
			exit(0);
		}

		str_cmd.pop();					//remove command after execution
	}
	
}

int main(int argc, char *argv[])
{
	
	queue<string>cmd;
	string commands;

	while (true)								//keep asking user for commands
	{
		prompt();								//Setup user prompt
				
		getline(cin, commands);					//user input commands

		/// break up commands
		list_cmd(commands,cmd);					//function handles user input into cmds
		
		/// execute commands
		exec_cmd(cmd);
	}

  return 0;
  
}
