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
		string run_cmd = str_cmd.front();				//execute the first command in queue
		
		const int finalWordCount = totalWordCount+1;

		char **list = new char*[finalWordCount];
		int count = 0;
		
		for(int i = 0; i < totalWordCount; i++)
		{	
			if((words[i] == ";") || words[i] == "||" || (words[i] == "&&"))
			{
				// execute the command
				pid_t pid;
				int status = 0;
				pid = fork();
				if(pid <= -1) // something went wrong
				{
					perror("ERROR [FORK]: CHILD FAILED\n");
					exit(1); // quit the program
				}
				else if(pid == 0) // child process
				{
					list[count] = NULL;
					int success = execvp(list[0], list);
					if(success <= -1) // nope, it failed
					{
						perror("ERROR: EXECUTING THE CMD FAILED\n");
						exit(1); // dip
					}
					else 
					{ 
						cout << "It succeeded..." << success << endl; 
					}
				}
				else // parent process---wait until the child is done
				{
					waitpid(-1, &status, 0);
					//cout << "THE STATUS: " << status << endl;
					if(words[i] == "&&" && (status > 0)) break;
					if(words[i] == "||" && (status <= 0))break;
				}
				// reset the list
				for(int j = 0; j < count; j++) 
				{
					list[j] = NULL;
				}
				count = 0;
			}
			else 
			{
				if(count == 0 && words[i] == "exit")
				{
					exit(1);
				}
				list[count] = new char[words[i].size()+1];
				copy(words[i].begin(), words[i].end(), list[count]);
				list[count][words[i].size()] = '\0';
				count++;
			}
		}

		totalWordCount = 0;
	

		str_cmd.pop();					//remove command after execution
	}
	
}

int main(int argc, char *argv[])
{
	
    bool bloop = true;
	queue<string>cmd;
	while (bloop == true)						//keep asking user for commands
	{
		prompt();								//Setup user prompt
		
		string commands;		
		getline(cin, commands);					//user input commands

		/// break up commands
		list_cmd(commands,cmd);					//function handles user input into cmds
		
		/// execute commands
		exec_cmd(cmd);
	}

  return 0;
  
}
