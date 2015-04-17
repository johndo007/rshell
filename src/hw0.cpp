#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

using namespace std;

void prompt()
{
	//get user info
    if (getlogin() != NULL) //only runs if there is a login user
    {
		cout << getlogin() << "@"; //display login and @
    }
    char hostname[127] = {0}; //max bytes is 128 in hostname
    if (gethostname(hostname, 255) != -1) //if there is a hostname
    {
		gethostname(hostname, 255); //put hostname -> hostname
		cout << hostname << " "; //cout host name
    }
	//if no user info, then deflaut '$'
    cout << "$" << " "; //prompt $
}

void list_cmd(string& input, vector<string>&words,int& totalWordCount) //converts string into individal commands
{
    vector<string> v; 
	int cnt = 0, wordCnt = 0; 
	string word; 
	for(string::iterator it=input.begin(); it != input.end(); it++)
	{
		if(input.at(cnt) == '#') //case 0 - #
		{
			if(word != " " && !word.empty())
			{
				v.push_back(word);
			}
			v.push_back(";");	
			word = "";
			break;
		}
		else if(input.at(cnt) == ' ') //case1 - space
		{
		    if(!word.empty() && word != "" && word != " ") 
		    {
			    v.push_back(word);
			    word = "";
			    totalWordCount++;
		    }
	    }
	    else if(input.at(cnt) == ';') //case2 - semicolon
        {
            if(word != "" && word != " ") 
            {
                v.push_back(word); 
                totalWordCount++;
            }
            v.push_back(";"); 
            word = ""; // reset word
        }
        else	//continue inputing characters to string
		{
            word += input.at(wordCnt);
    	}
		wordCnt++;
		cnt++;
	}
	
	if(word != "" && word != " ")	//end of the cmd string
	{
		v.push_back(word);
		if(word != ";")
		{
			v.push_back(";");
		}
		totalWordCount++;
	}
	totalWordCount++;
	if(v.at(v.size()-1) != ";") 
    { 
        v.push_back(";"); 
    }
	totalWordCount = v.size();
	words = v;	//update vector with contents of v by reference
    
}

void exec_cmd(vector<string>&words,int& totalWordCount)
{
        const int finalWordCount = totalWordCount+1;

        char **cmdlist = new char*[finalWordCount];
        int count = 0;
        
        for(int i = 0; i < totalWordCount; i++)
        {
			if((words[i] == ";") || words[i] == "||" || (words[i] == "&&"))
			{
			 // execute the command
				//pid_t pid;
				int pid = 0;
				int status = 0;
				pid = fork();
				if(pid <= -1) // something went wrong
				{
					perror("ERROR [FORK]: CHILD FAILED\n");
					exit(1); // quit the program
				}
				else if(pid == 0) // child process
				{
					 cmdlist[count] = NULL;
					int success = execvp(cmdlist[0], cmdlist);
					if(success <= -1) // nope, it failed
					{
						perror("ERROR: EXECUTING THE CMD FAILED\n");
						exit(1);
					}
				}
				else // parent process---wait until the child is done
				{
					waitpid(-1, &status, 0);
            
					if(words[i] == "&&" && (status > 0)) break;
					if(words[i] == "||" && (status <= 0))break;
				}
				 // reset the list
				 for(int j = 0; j < count; j++)
				 {
					cmdlist[j] = NULL;
				 }
				 count = 0;
            }
            else 
			{
            
				if(count == 0 && words[i] == "exit")
				{
					exit(1);
				}
				cmdlist[count] = new char[words[i].size()+1];
				copy(words[i].begin(), words[i].end(), cmdlist[count]);
				cmdlist[count][words[i].size()] = '\0';
				count++;
            }
        }

        totalWordCount = 0;
}

int main(int argc, char** argv)
{
	string input;	//get user cmd in one line
    vector<string>words;	//seperate the user input into vector of strings
	int wordcnt = 0;	//word counter to keep track of postion

	while(true)
	{
        prompt(); //Setup user prompt
      	
		getline (cin, input);		
		       
        ///break up commands
        list_cmd(input, words,wordcnt);

        ///execute commands        
        exec_cmd(words,wordcnt);
	}
	return 0;
}
