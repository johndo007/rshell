#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
//#include <queue>
#include <string>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

using namespace std;

bool printErrors = false;

int totalWordCount = 0;

void prompt()
{
    if (getlogin() != NULL) //only runs if there is a login
    {
    cout << getlogin() << "@"; //display login and @
    }
    char hostname[127] = {0}; //max bytes is 128 in hostname
    if (gethostname(hostname, 255) != -1) //if there is a hostname
    {
    gethostname(hostname, 255); //put hostname -> hostname
    cout << hostname << " "; //cout host name
    }
    cout << "$" << " "; //prompt $
}

void list_cmd(string& input, vector<string>&words) //converts string into individal commands
{
    vector<string> v; 
	int cnt = 0, wordCnt = 0; 
	string word; 
	for(string::iterator it=input.begin(); it != input.end(); it++)
	{
		if(input.at(cnt) == '#')
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
		    else 
            { 
                cout<<"Hello World"<<endl; 
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
        else
		{
            word += input.at(wordCnt);
    	}
		wordCnt++;
		cnt++;
	}
	
	if(word != "" && word != " ")
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
	words = v;
    
}

void exec_cmd(vector<string>&words)
{
       const int finalWordCount = totalWordCount+1;

        char **list = new char*[finalWordCount];
        int count = 0;
        
        for(int i = 0; i < totalWordCount; i++)
        {
        //cout << "TWC " << totalWordCount << "..i=" << i << endl;
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
            
            	if(words[i] == "&&" && (status > 0)) break;
            	if(words[i] == "||" && (status <= 0))break;
            }
            // reset the list
            for(int j = 0; j < count; j++) {
            list[j] = NULL;
            }
            count = 0;
            }
            else {
            //cout << "Adding word" << endl;
            if(count == 0 && words[i] == "exit")
                {
                    exit(1);
                }
            list[count] = new char[words[i].size()+1];
            copy(words[i].begin(), words[i].end(), list[count]);
            list[count][words[i].size()] = '\0';
            //cout << "..." << list[count] << endl;
            count++;
            }
        }

        totalWordCount = 0;
}

int main(int argc, char** argv)
{
	string input;
    vector<string>words;
	
	while(true)
	{
        prompt(); //Setup user prompt
      	
		getline (cin, input);		
		       
        ///break up commands
        list_cmd(input, words);

        ///execute commands        
        exec_cmd(words);
	}
	return 0;
}
