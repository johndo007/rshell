#ifndef _SHELL_H__
#define _SHELL_H__

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <cstring>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <algorithm>
#include <pwd.h>
#include "ls_cmd.h"
using namespace std;

    void prompt()   //get user info ... DONE
    {
		char* login = getlogin();
		char hostname[256];
		if (login == NULL) perror("login");
		if (-1 == gethostname(hostname, 256)) perror("hostname");

		printf("%s", login);
		printf("%s", "@");
		printf("%s ", hostname);
		printf("%s ", "$");
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
				//
				word +='#';
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
					v.push_back(word);
					totalWordCount++;
					word="";
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
					v.push_back(word);
					totalWordCount++;
					word="";
				}
			}
		}
		else if(input.at(cnt)=='|') //case4 - bar |
        {
			if(word != "" && !word.empty())	//with no space
			{
				v.push_back(word);
				totalWordCount++;
				word="";
			}
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
				word="";
			}

		}
		else if(input.at(cnt)=='>') //case5 - redirect out >
        {
			if(word != "" && !word.empty())	//with no space
			{
				v.push_back(word);
				totalWordCount++;
				word="";
			}
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
				word="";
			}


		}
		else if(input.at(cnt)=='<') //case6 - redirect in <
        {
			if(word != "" && !word.empty())	//with no space
			{
				v.push_back(word);
				totalWordCount++;
				word="";

			}
			if((input.size()>cnt+1) && (input[cnt+1] == '<') && (input[cnt+2]=='<'))
			{
				v.push_back("<<<");
				word="";
				cnt+=2;
				totalWordCount++;
			}
			else
			{
				word = "<";
				v.push_back(word);
				totalWordCount++;
				word="";

			}


		}
		else if(input.at(cnt)=='"')
        {
            //int index = 0;
            if(word != "" && !word.empty())	//with no space
			{
				v.push_back(word);
				totalWordCount++;
				word="";
			}
			unsigned int x=cnt+1;
			for(;x<input.size();x++)
			{
				if(input.at(x)=='"')
				{
					v.push_back(word);
					totalWordCount++;
					word="";
					cnt=x;

					break; //jump for-x-loop
				}
				word+=input.at(x);
			
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

    void exec_cmd(vector<string>&xwords,int& totalWordCount)
    {
    	vector<string>words;
		string tmp;
        const int finalWordCount = totalWordCount+1;
        for(int x=0;x<totalWordCount;x++)
        {
			tmp=xwords[x];
			if(xwords[x]!="cat")
			{
				words.push_back(tmp); //xwords[x]);
			}
			else
			{
				if( (x+2)<totalWordCount)
				{
					if(xwords[x+1]=="<<<")
						{
							words.push_back("echo"); //xwords[x]); //push cat
							words.push_back(xwords[x+2]); //"....word.."
							words.push_back("|");		//  |
							words.push_back("cat");
							x+=2;						//skip "


						}else words.push_back(tmp);
				}else words.push_back(tmp);

			}

		}
		int y=words.size();
		totalWordCount=y;
//++++++++++++++++++++++
#define MAX_TASK	18
	int pipes[MAX_TASK*2];	//Assume: total 18 pipes max
	int in=-1;	//input file handle
	int out=-1;	//output file handle
	//int pid=0;
	int status[MAX_TASK];	//Assume: total 18 status
	int current_task_start[MAX_TASK];
	int current_task_end[MAX_TASK];

	char *current_task[32];
//++++++++++++++++++++++++++++
        char **cmdlist = new char*[finalWordCount];
        int count = 0;

        int index_else=-1;
        int end_task=0;  //=3-1;
        int status1 = 0;
        for(int i = 0; i < totalWordCount; i++)
        {
			if((words[i] == ";") || words[i] == "||" || (words[i] == "&&"))
			{
				cmdlist[count] = NULL;	//mark the end of arg list
				//execute the command

				//int pid = 0;
				//int status1 = 0;
				//++++++++++++++++++++++++++++++++++++++++++++


	//int end_task=0;  //=3-1;
	int task_index=0;
	int l=0;

	//--Simulation data



	//cout<<"count="<<count<<endl;
	//calculate starting and stopping indexes for child processing
	current_task_start[0]=0;
	task_index=0;
	for(l=0;cmdlist[l]!=NULL;l++)
	{
		//cout<<"l="<<l<<endl;
		//if(l>=count)break;
		if(strcmp(cmdlist[l],"|")==0)
		{

			current_task_end[task_index]=l;
			current_task_start[task_index+1]=l+1;

			end_task++;
			task_index++;
		}

	}
	//cout<<"la="<<l<<endl;
	current_task_end[task_index]=l;
	//prepare numbers of pipes for communication
	for(int x=0;x<end_task;x++)
	{

		status[x]=pipe(pipes+2*x);		//1st pipe ..2nd...
		if(status[x]<0)
		{
			perror("pipe");
			exit(1);
		}
	}



int status2;
//beginning of child processing loop
for(int task=0;task<=end_task;task++)
	{
		//locate the individial task into current_task
		int n=0;
		for(int cur=current_task_start[task];cur<current_task_end[task];cur++)
		{
			current_task[n++]=cmdlist[cur];
		}
		current_task[n]=NULL;

		if( (status2=fork())==0)
		{
			if(end_task>0)	//pipe operation
			{
				if(task==0)		//first command in the process
				{
					dup2(pipes[end_task*2-1],1);	//output from last pipe
					for(int ii=0;ii<end_task;ii++)close(pipes[ii]);

					//Handle output
					bool found=false;
					int s;
					for(s=0;current_task[s]!=NULL;s++)
					{
						if(strcmp(current_task[s],">")==0)
						{
							found=true;
							break;
						}
						if(strcmp(current_task[s],">>")==0)
						{
							found=true;
							break;
						}
					}
					if(found)
					{
						if(strcmp(current_task[s],">")==0)
						{
							//cerr<<"Outputfile="<<current_task[s+1]<<endl;
						out=open(current_task[s+1],O_WRONLY|O_TRUNC|O_CREAT,
												S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
						}else
						{
							out=open(current_task[s+1],O_WRONLY|O_APPEND,
												S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
						}


						dup2(out,1);
						current_task[s]=NULL;
					} //end of Handle OUTPUT redirection

					//Handle input redirection
					bool found_in=false;
					//int s;
					for(s=0;current_task[s]!=NULL;s++)
					{
						if(strcmp(current_task[s],"<")==0)
						{
							found_in=true;
							break;
						}
					}
					if(found_in)
					{
						//cerr<<"inputfile="<<current_task[s+1]<<endl;
						in=open(current_task[s+1],O_RDONLY);

						dup2(in,0);		//make file info for inputting to current child cmd process
						current_task[s]=NULL;
					}//end of input redirection

					if(found_in)close(in);										 // and inputfile handle
					if(found)close(out);										 // and outputfile handle
					//cerr<<"Before in starting:"<<current_task[0]<<" Id="<<getpid()<<endl;
					if(strcmp(current_task[0],"ls")==0)
					{
						ls_cmd(current_task);
						exit(0);
					}
					else
					{
					status[task]=execvp(current_task[0],current_task);
					cerr<<endl<<"Error:"<<current_task[0]<<" Id="<<getpid()<<endl;
					exit(0);
					}


				} //end of if task==0

				else if(task>0 && task!=end_task)
				{
					dup2(pipes[(end_task-task)*2-1],1);		//output to next pipe below
					dup2(pipes[(end_task-task)*2],0);		//input from the pipe above
					for(int ii=0;ii<end_task*2;ii++)close(pipes[ii]);	//Important!! close all pipes before execvp...
					//cerr<<"Before in startingn:"<<current_task[0]<<" Id="<<getpid()<<endl;
					status[task]=execvp(current_task[0],current_task);
					cerr<<endl<<"Error:"<<current_task[0]<<" Id="<<getpid()<<endl;
					exit(0);
				} //end if task>0 && task!=end_task
				else if(task==end_task)
				{	//task==end_task

					//cerr<<"setup :"<<current_task[0]<<" its ID="<<getpid()<<endl;
					if(end_task>0)
					dup2(pipes[0],0);	//input from the lowest pipe

					//Handle any OUTPUT redirection
					int s;
					bool found=false;

					for(s=0;current_task[s]!=NULL;s++)
					{
						if(strcmp(current_task[s],">")==0)
						{
							found=true;
							break;
						}
						if(strcmp(current_task[s],">>")==0)
						{
							found=true;
							break;
						}
					}
					if(found)
					{
						//cerr<<"Outputfile="<<current_task[s+1]<<endl;
						if(strcmp(current_task[s],">")==0)
						{
							//cerr<<"Outputfile="<<current_task[s+1]<<endl;
						out=open(current_task[s+1],O_WRONLY|O_TRUNC|O_CREAT,
												S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
						}else
						{
							out=open(current_task[s+1],O_WRONLY|O_APPEND,
												S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
						}

						dup2(out,1);
						current_task[s]=NULL; 	//command only, no parameters
					}//end of //Handle any OUTPUT redirection

					// Handle any INPUT redirection
					bool found_in=false;
					for(s=0;current_task[s]!=NULL;s++)
					{
						if(strcmp(current_task[s],"<")==0)
						{
							found_in=true;
							break;
						}

					}
					if(found_in)
					{
						//cerr<<"Inputfile="<<current_task[s+1]<<endl;
						in=open(current_task[s+1],O_RDONLY);

						dup2(in,0);
						current_task[s]=NULL; 	//command only, no parameters
					} //end of Handle any INPUT redirection

					if(end_task>0)for(int ii=0;ii<end_task*2;ii++)close(pipes[ii]); //Important! Close all pipes
					if(found_in)close(in);										// input file handle
					if(found)close(out);										// and output file handle before child process..
					//cerr<<"Before out starting:"<<current_task[0]<<" Id="<<getpid()<<endl;
					if(strcmp(current_task[0],"ls")==0)
					{
						ls_cmd(current_task);
						exit(0);
					}
					else
					{
					status[task]=execvp(current_task[0],current_task);
					cerr<<endl<<"Error:"<<current_task[0]<<" Id="<<getpid()<<endl;
					exit(0);
					}
				} //end if task==last task
			} //end if end_task>0
			else
			{
				//end_task==0
				//cerr<<"setup :"<<current_task[0]<<" its ID="<<getpid()<<endl;
					//if(end_task>0)
					//dup2(pipes[0],0);	//input from the lowest pipe

					//Handle any OUTPUT redirection
					int s;
					bool found=false;

					for(s=0;current_task[s]!=NULL;s++)
					{
						if(strcmp(current_task[s],">")==0)
						{
							found=true;
							break;
						}
						if(strcmp(current_task[s],">>")==0)
						{
							found=true;
							break;
						}
					}
					if(found)
					{
						//cerr<<"Outputfile="<<current_task[s+1]<<endl;
						if(strcmp(current_task[s],">")==0)
						{
							//cerr<<"Outputfile="<<current_task[s+1]<<endl;
						out=open(current_task[s+1],O_WRONLY|O_TRUNC|O_CREAT,
												S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
						}else
						{
							out=open(current_task[s+1],O_WRONLY|O_APPEND,
												S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
						}

						dup2(out,1);
						current_task[s]=NULL; 	//command only, no parameters
					}//end of //Handle any OUTPUT redirection

					// Handle any INPUT redirection
					bool found_in=false;
					for(s=0;current_task[s]!=NULL;s++)
					{
						if(strcmp(current_task[s],"<")==0)
						{
							found_in=true;
							break;
						}

					}
					if(found_in)
					{
						//cerr<<"Inputfile="<<current_task[s+1]<<endl;
						in=open(current_task[s+1],O_RDONLY);

						dup2(in,0);
						current_task[s]=NULL; 	//command only, no parameters
					} //end of Handle any INPUT redirection

					if(end_task>0)for(int ii=0;ii<end_task*2;ii++)close(pipes[ii]); //Important! Close all pipes
					if(found_in)close(in);										// input file handle
					if(found)close(out);										// and output file handle before child process..
					//cerr<<"Before out starting:"<<current_task[0]<<" Id="<<getpid()<<endl;
					if(strcmp(current_task[0],"ls")==0)
					{
						ls_cmd(current_task);
						exit(0);
					}
					else
					{
					status[task]=execvp(current_task[0],current_task);
					cerr<<endl<<"Error:"<<current_task[0]<<" Id="<<getpid()<<endl;
					exit(0);
					}

			}// end of end_task==0
		}
		if(status2==-1)
		{
			perror("fork error");
			exit(-1);
		}
} //end of for-task<=end_task loop

	//-------------------------------------------end of updated
	if(end_task>0)
	{
		//cerr<<"Here out33 id="<<getpid()<<endl;
		for(int ii=0;ii<end_task*2;ii++)close(pipes[ii]);	//Important !!! Close pipes first for piping communication
		for(int ii=0;ii<end_task*2;ii++)wait(&status1);		//then wait...
	}else
	{
		wait(&status1);					//wait for non-Piping operation
		if(out>0)close(out);			//close for out-redirection
		if(in>0)close(in);
		//cerr<<"Done with end_task=0  status1aaa="<<status1<<" id="<<getpid()<<endl;
		//close(pipes[0]);
		//close(pipes[1]);
		if(status1<0)
		{
			cerr<<"last cmd"<<current_task[0]<<endl;
			char c;
			cerr<<"Wait for a key before exiting?";
			cin>>c;
			break; //abort!!!
		}
	}

				//++++++++++++++++++++++++++++++++++++++++++++
				waitpid(-1, &status1, 0);	//wait for a current child process finishing
				if(words[i] == "&&" && (status1 > 0))	//current process done
					{	//the process of if-then is ok then skip until a connector
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
					}else if(words[i] == "||" && (status1 == 0))
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
				//cerr<<"Here xxx id="<<getpid()<<endl;
				if(status1<0)
				{
					cerr<<"Error4"<<endl;
				break;  //abort current operation
				}

				//clean any abort operation flag

				 // reset the list for next command if any
				 for(int j = 0; j < count; j++)
				 {
				 	delete cmdlist[j];
					cmdlist[j] = NULL;
				 }
				 // clear cmd counter
				 count = 0;
				 //cerr<<"Here out5 id="<<getpid()<<endl;
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
					exit(1); //EXit THIS program!!!
				}
				cmdlist[count] = new char[words[i].size()+1];
				copy(words[i].begin(), words[i].end(), cmdlist[count]);
				cmdlist[count][words[i].size()] = '\0';
				count++;
            }
        }
        //cerr<<"Here out id="<<getpid()<<endl;
        if(end_task>0)
        {
			for(int ii=0;ii<end_task*2;ii++)close(pipes[ii]);	//Important !!! Close pipes first for piping communication
			for(int ii=0;ii<end_task*2;ii++)wait(&status1);		//then wait...
		}
		//normal case  memory clean up when the list is completely processed
		for (int j = 0;j<count;j++)
		{
			delete cmdlist[j];	//delete the argument content
		}
		delete cmdlist;	//delete argument pointers
        totalWordCount = 0;

}

#endif // _SHELL_H__
