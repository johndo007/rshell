#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
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
	unsigned int  cnt , wordCnt ;
	string word="";
	for(cnt=0,wordCnt=0;cnt<input.size();cnt++)
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
		    }else {word="";}
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
        else if(input.at(cnt)=='&') //case3 - ampercent &
        {
			if(word != "" && !word.empty())	//with no space
			{
				v.push_back(word);
				totalWordCount++;
				if(input[cnt+1] == '&')
				{
					v.push_back("&&");
					word="";
					cnt++;
					totalWordCount++;
				}
				else
				{
					word = '&';
				}
			}
			else    //with space
			{
				if(input[cnt+1] == '&')
				{
					v.push_back("&&");
					word="";
					cnt++;
					totalWordCount++;
				}
				else
				{
					word = '&';
				}
			}
		}
		else if(input.at(cnt)=='|') //case4 - bar |
        {
			if(word != "" && !word.empty())	//with no space
			{
				v.push_back(word);
				totalWordCount++;
				if(input[cnt+1] == '|')
				{
					v.push_back("||");
					word="";
					cnt++;
					totalWordCount++;
				}
				else
				{
					word = '|';
					v.push_back(word);
					totalWordCount++;
				}
			}
		}
		else if(input.at(cnt)=='>') //case5 - redirect out >
        {
			if(word != "" && !word.empty())	//with no space
			{
				v.push_back(word);
				totalWordCount++;
				if(input[cnt+1] == '>')
				{
					v.push_back(">>");
					word="";
					cnt++;
					totalWordCount++;
				}
				else
				{
					word = ">";
					v.push_back(word);
					totalWordCount++;
				}
			}

			else    //with space
			{
				if(input[cnt+1] == '>')
				{
					v.push_back(">>");
					word="";
					cnt++;
					totalWordCount++;
				}
				else
				{
					word = ">";
					v.push_back(word);
					totalWordCount++;

				}
			}
		}
        else	//continue inputing characters to string
		{
            //word += input.at(wordCnt);
            word +=input.at(cnt);
    	}
		wordCnt++;
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
        /*
		for(int i=0;i<finalWordCount-1;i++)
        cout<<"Argv="<<i<<":"<<words[i]<<endl;
		return;
		*/

        char **cmdlist = new char*[finalWordCount];
        int count = 0;
        int index_else=-1;
        //int index_then=-1;

        for(int i = 0; i < totalWordCount; i++)
        {
			if((words[i] == ";") || words[i] == "||" || (words[i] == "&&"))
			{
				//execute the command

				int pid = 0;
				int status = 0;
				bool abort = false; // meaning ls has not been called

				pid = fork();

				if(pid <= -1) // something went wrong
				{
					perror("ERROR [FORK]: CHILD FAILED\n");
					for(int j =0;j<count;j++)
					{
						delete cmdlist[j];
					}
					delete cmdlist;
					exit(1); // quit the program
				}

				if(cmdlist[0]=="ls")    //begin hw1 ls assignment
                {
                    cout<<"ls was called!"<<endl;
                    //ls_command(cmdlist);
                    abort = true;
                }

				else if(pid == 0 && abort == false) //starting  child process
				{
					cmdlist[count] = NULL;
					/*
					//processing any redirecting operation
					int r;
					fwmode=-1;
					for( r=1;r<count;r++)
					{
						int s=sizeof(cmdlist[r])/2;
						if(cmdlist[r][0]=='>')
						{//redirect-out
							if(s>1)fwmode=2; //append
							else fwmode=1;   //create
							break;
						}
					}
					if(fwmode>0) //redirect-out operation
					{
						cmdlist[r]='\0'; //set NULL
						if(fwmode==1)
						fw=open(cmdlist[r+1],O_WRONLY|O_TRUNC|O_CREAT,S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
						if(fwmode==2)
						fw=open(cmdlist[r+1],O_WRONLY|O_APPEND,S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
						dup2(fw,1);	//replace standard output(1) with output file=cmdlist[r+1]

					}


					if(fwmode>0)close(fw);	//close file handle before child process
					*/

					int success;
					if(pid==0)

					success = execvp(cmdlist[0], &cmdlist[0]);
					if(success <= -1 ) // nope, it failed ... failed on command
					{

						perror("ERROR: EXECUTING THE CMD FAILED\n");
						//exit(1);
						int x;
						index_else =-1;
						if(words[i]== "&&")
						//start copying here
							x = i+1;	//skip && and look for next avaiable connector
						else
							x = i;	//look for next possible connector
						for( ; x < totalWordCount; x++)
						{
							if(words[i]=="||"||words[i]=="&&"||words[i]==";")
							{
								index_else = x;
								break;
							}
						}
						if(index_else>0)
						{
							i=index_else;
							index_else =-1;
						}

					}

					for(int j = 0; j < count; j++)
					{
						delete cmdlist[j];
						cmdlist[j] = NULL;
					}

					exit(success);
				}

				else // parent process---wait until the child is done
				{
					waitpid(-1, &status, 0);
					//if(fwmode>0)close(fw);

					if(words[i] == "&&" && (status > 0))	//failed on prameter
					{
						int x;
						index_else = -1;
						x = i+1;
						for(;x<totalWordCount; x++)
						{
							if(words[x]== "&&"||words[x]=="||"||words[x]==";")
							{
								index_else = x;
								break;
							}
						}
						if(index_else>0)
						{
							i = index_else;
							index_else = -1;
						}
					}
					if(words[i] == "||" && (status == 0))
					{//break;
						int x;
						index_else = -1;
						x = i+1;
						for(;x<totalWordCount; x++)
						{
							if(words[x]== "&&"||words[x]=="||"||words[x]==";")
							{
								index_else = x;
								break;
							}
						}
						if(index_else>0)
						{
							i = index_else;
							index_else = -1;
						}

					}
				}
				 // reset the list
				 for(int j = 0; j < count; j++)
				 {
				 	delete cmdlist[j];
					cmdlist[j] = NULL;
				 }
				 count = 0;
            }
            else  //simple command case or beginning of if-then cmd
			{

				if(count == 0 && words[i] == "exit")
				{
					for(int j =0; j<count;j++)
					{
						delete cmdlist[j];
					}
					delete cmdlist;
					exit(1);
				}
				cmdlist[count] = new char[words[i].size()+1];
				copy(words[i].begin(), words[i].end(), cmdlist[count]);
				cmdlist[count][words[i].size()] = '\0';
				count++;
            }
        }
		//normal case  memory clean up
		for (int j = 0;j<count;j++)
		{
			delete cmdlist[j];	//delete the argument content
		}
		delete cmdlist;	//delete argument pointers
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
