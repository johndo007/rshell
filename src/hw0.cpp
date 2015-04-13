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
        //Being reworked as of 1.09 update
        cout<<"Sorry execvp is unavailable for user usage"<<endl;
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
