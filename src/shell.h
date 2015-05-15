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
#include "redirect.h"

using namespace std;

void prompt(string &input)   //get user info ... DONE
{
    char* login = getlogin();

    char hostname[256];

    if (login == NULL) perror("login");

    if (-1 == gethostname(hostname, 256)) perror("hostname");



    printf("%s", login);
    printf("%s", "@");
    printf("%s ", hostname);
    printf("%s ", "$");

    getline(cin,input);
}

void list_cmd(string& input, vector<string>&word) //converts string into individal commands
{
    vector<string> v;
	string tmp="";
	for(unsigned int i=0;i<input.size();i++)
	{
	    if(input[i]=='&')
        {
            //Todo: Here process the current input and its related  "&&"
            if(tmp.size()>0){ word.push_back(tmp); tmp="";}
            if(input[i+1]=='&'){ word.push_back("&&"); i+=1; continue;}
            word.push_back("&"); continue;
        }
        else if(input[i]=='|')
        {
            //Todo: Here process the current input and its related "||"
            if(tmp.size()>0){ word.push_back(tmp); tmp="";}
            if(input[i+1]=='|'){ word.push_back("||"); i+=1; continue;}
            word.push_back("|"); continue;
        }
        else if(input[i]=='#')
        {
            if(tmp.size()>0){word.push_back(tmp); tmp="";}
            word.push_back(";");
            break;
        }
        else if(input[i]==';')
        {
            if(tmp.size()>0){ word.push_back(tmp); tmp="";}
            word.push_back(";");
            continue;
        }
        else if(input[i]=='<')
        {
            //Todo: Here process the current input and its related "<" "<<<"
            if(tmp.size()>0){word.push_back(tmp); tmp="";}
            if(input[i+1]=='<' && input[i+2]=='<'){word.push_back("<<<"); i+=2; continue;}
            word.push_back("<");
            continue;
        }
        else if(input[i]=='>')
        {
            //Todo: Here process the current input and its related ">>" ">"
            if(tmp.size()>0){word.push_back(tmp); tmp="";}
            if(input[i+1]=='>'){word.push_back(">>"); i+=1; continue;}
            word.push_back(">"); continue;
        }
        else if(input[i]=='"')
        {
            if(tmp.size()>0)word.push_back(tmp);
            tmp="";
            int x;
            for(x=i+1;input[x]!=0;x++)
            {
                if(input[x]=='"')
                {
                    word.push_back(tmp); tmp=""; i=x;
                    break;
                }
                tmp+=input[x];

            }
            if(tmp.size()>0)
            {
                word.push_back(tmp);
                i=x-1;
            }
            continue;

        }
        else if(input[i]==' ')
        {
            if(tmp.size()>0){word.push_back(tmp); tmp="";}
            continue;
        }
        else
        {
            tmp+=input[i];
        }
    }
	 //end of for-i-loop
	if(tmp.size()>0)word.push_back(tmp);
	word.push_back(";");
}

void exec_cmd(vector<string>&xwords)
{
    //holds the new vector that checked for special cases(EC)
    vector<string>words;
    //redirect <<< check process and # of |
    int pcount = check_pipe_ldir(xwords,words);
    //hold number of arguments
    int totalWordCount=words.size();
    const int finalWordCount = totalWordCount+1;

    //++++++++++++++++++++++

    // pipe/redirection variables
    int in=-1, out=-1;	//i/o file handle status
    int* pipes = new int[pcount*2];	//Assume: total 18 pipes max
    int* status = new int[pcount];	//Assume: total 18 status
    int* current_task_start= new int[pcount];
    int* current_task_end= new int[pcount];
    char *current_task[32];

    //++++++++++++++++++++++++++++
    char **cmdlist = new char*[finalWordCount]; //hold cmd as char*
    int count = 0;
    int index_else=-1;
    int end_task=0;  //=3-1;
    int status1 = 0, status2 = 0;

    for(int i = 0; i < totalWordCount; i++)
    {
        if((words[i] == ";") || words[i] == "||" || (words[i] == "&&"))
        {
            cmdlist[count] = NULL;	//mark the end of arg list
            int task_index=0, l=0;
            //calculate starting and stopping indexes for child processing
            current_task_start[0]=0;	//setup current task list to 0
            task_index=0;
            for(l=0;cmdlist[l]!=NULL;l++)	//count the number of '|' available
            {
                if(strcmp(cmdlist[l],"|")==0)
                {
                    current_task_end[task_index]=l;
                    current_task_start[task_index+1]=l+1;
                    end_task++;
                    task_index++;
                }
            }
            current_task_end[task_index]=l;
            //prepare numbers of pipes for communication
            for(int x=0;x<end_task;x++)
            {
                //1st pipe ..2nd...
                status[x]=pipe(pipes+2*x);
                if(status[x]<0)
                {
                    perror("pipe");
                    exit(1);
                }
            }
            //begin child processing loop
            for(int task=0;task<=end_task;task++)
            {
                //locate the individial task from the cmdlist[] based on the task value into current_task[]
                int n=0;
                for(int cur=current_task_start[task];cur<current_task_end[task];cur++)
                {
                    current_task[n++]=cmdlist[cur];
                }
                current_task[n]=NULL;	//mark the end of the current task

                if( (status2=fork())==0)	//begin child process
                {
                    if(status2==-1)perror("fork");
                    if(end_task>0)	//pipe operation
                    {
                        if(task==0)		//first command in the process
                        {
                            if(dup2(pipes[end_task*2-1],1)==-1) perror("dup2");	//output from last pipe
                            for(int ii=0;ii<end_task;ii++)
                                if(close(pipes[ii])==-1)
								{perror("close");cout<<"1"<<endl;}

                            //Handle output
                            bool found=false;
                            int s;
                            for(s=0;current_task[s]!=NULL;s++)	///check for output redirection
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


                                if(dup2(out,1)==-1)perror("dup2");
                                current_task[s]=NULL;
                            } //end of Handle OUTPUT redirection
                            //Handle input redirection
                            bool found_in=false;	//set false to check on next redirection

                            for(s=0;current_task[s]!=NULL;s++)	///check for input redirection
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
                                if(in==-1)  perror("open");
                                if(dup2(in,0)==-1) perror("dup2");		//make file info for inputting to current child cmd process
                                current_task[s]=NULL;
                            }//end of input redirection

                            if(found_in)
                                if(close(in)==-1)
								{perror("close");cout<<"2"<<endl;}										 // and inputfile handle
                            if(found)
                            if(close(out)==-1)
							{perror("close");cout<<"3"<<endl;}										 // and outputfile handle
                            //cerr<<"Before in starting:"<<current_task[0]<<" Id="<<getpid()<<endl;
                            if(strcmp(current_task[0],"ls")==0)
                            {
                                ///RUN STUDENT LS
                                ls_cmd(current_task);
                                exit(0);
                            }
                            else
                            {
                                ///RUN SHELL
                                status[task]=execvp(current_task[0],current_task);
                                if(status[task]==-1)    perror("execvp");
                                exit(0);
                            }
                        } //end of if task==0

                        else if(task>0 && task!=end_task)
                        {
                            if(dup2(pipes[(end_task-task)*2-1],1)==-1)
                                perror("dup2");		//output to next pipe below
                            if(dup2(pipes[(end_task-task)*2],0)==-1)
                                perror("dup2");		//input from the pipe above
                            for(int ii=0;ii<end_task*2;ii++)
                                if(close(pipes[ii])==-1)   
								{perror("close");cout<<"4"<<endl;}	//Important!! close all pipes before execvp...
                            //cerr<<"Before in startingn:"<<current_task[0]<<" Id="<<getpid()<<endl;
                            status[task]=execvp(current_task[0],current_task);
                            if(status[task]==-1)    perror("execvp");
                            //cerr<<endl<<"Error:"<<current_task[0]<<" Id="<<getpid()<<endl;
                            exit(0);
                        } //end if task>0 && task!=end_task
                        else if(task==end_task)
                        {	//task==end_task
                            if(end_task>0)
                                if(dup2(pipes[0],0)==-1) perror("dup2");	//input from the lowest pipe

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

                                if(dup2(out,1)==-1)perror("dup");
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
                                if(in==-1)  perror("open");
                                if(dup2(in,0)==-1)  perror("dup2");
                                current_task[s]=NULL; 	//command only, no parameters
                            } //end of Handle any INPUT redirection

                            if(end_task>0)
                                for(int ii=0;ii<end_task*2;ii++)
                                    if(close(pipes[ii]))   
									{perror("close");cout<<"5"<<endl; } //Important! Close all pipes
                            if(found_in)
                                if(close(in)==-1)   
								{perror("close");cout<<"6"<<endl;}										// input file handle
                            if(found)
                                if(close(out)==-1)  
								{perror("close");cout<<"7"<<endl;}										// and output file handle before child process..
                            //cerr<<"Before out starting:"<<current_task[0]<<" Id="<<getpid()<<endl;
                            if(strcmp(current_task[0],"ls")==0)
                            {
                                ls_cmd(current_task);
                                exit(0);
                            }
                            else
                            {
                            status[task]=execvp(current_task[0],current_task);
                            if (status[task] ==-1) perror("execvp");
                            exit(0);
                            }
                        } //end if task==last task
                    } //end if end_task>0
                    else
                    {
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

                            if(dup2(out,1)==-1) perror("dup2");
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
                            in=open(current_task[s+1],O_RDONLY);
                            if(in==-1)  perror("open");
                            if(dup2(in,0)==-1)perror("dup2");
                            current_task[s]=NULL; 	//command only, no parameters
                        } //end of Handle any INPUT redirection

                        if(end_task>0)
                            for(int ii=0;ii<end_task*2;ii++)
                                if(close(pipes[ii])==-1)   
								{ perror("close"); cout<<"8"<<endl;}//Important! Close all pipes
                        if(found_in)
                            if(close(in)==-1)   
							{perror("close"); cout<<"9"<<endl;}									// input file handle
                        if(found)
                            if(close(out)==-1)  {perror("close");cout<<"9"<<endl;}										// and output file handle before child process..
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
            //+++++++++++++++++++++++++++++++++++
                        //Delete dynamically allocated memory
                        delete [] pipes;
                        delete [] status;
                        delete [] current_task_start;
                        delete [] current_task_end;

            //+++++++++++++++++++++++++++++++++++
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
                for(int ii=0;ii<end_task*2;ii++)
                    if(close(pipes[ii])==-1) 
					{perror("close");cout<<"10"<<endl;}	//Important !!! Close pipes first for piping communication
                for(int ii=0;ii<end_task*2;ii++)
                    if(wait(&status1)==-1)   
						if(status1!=0)	{perror("wait");cout<<"11"<<endl;}		//then wait...
            }
            else
            {
                if(wait(&status1)==-1)  perror("wait");					//wait for non-Piping operation
                if(out>0)
                    if(close(out)==-1)  
					{perror("close");cout<<"12"<<endl;}			//close for out-redirection
                if(in>0)
                    if(close(in)==-1)   
					{perror("close");cout<<"13"<<endl;}
                if(status1<0)
                {
                    break; //abort!!!
                }
            }
            //++++++++++++++++++++++++++++++++++++++++++++
            /* if(waitpid(-1, &status1, 0)==-1)
            {	
				cout<<"I'm here!"<<endl;
                perror("waitpid");	//wait for a current child process finishing
            }*/
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
            }
            else if(words[i] == "||" && (status1 == 0))
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
            if(status1<0)
            {
                cerr<<"Error4"<<endl;
                break;
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
    /*if(end_task>0)
    {
        for(int ii=0;ii<end_task*2;ii++)	//Important !!! Close pipes first for piping communication
            
			if(close(pipes[ii])==-1)
			{ 
				cout<<ii<<endl;
				perror("close");
				cout<<"14"<<endl;
			}
        for(int ii=0;ii<end_task*2;ii++)
            if(wait(&status1)==-1)perror("wait");		//then wait...
    }*/
    //normal case  memory clean up when the list is completely processed
    for (int j = 0;j<count;j++)
    {
        delete cmdlist[j];	//delete the argument content
    }
    delete cmdlist;	//delete argument pointers
}

#endif // _SHELL_H__
