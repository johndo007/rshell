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

extern struct str_direct
{
	char *cmdlist[256];
	char *current_task[64];
	int task_start[64];
	int task_end[64];
	int pipes[64];		//Max child process = 32
	int cmd_start;
	int task_count;
	int in;
	int out;
};

void prompt(string &input)   //get user info ... DONE
{
    char* login = getlogin();	//get username
    char hostname[256];			//allocated char memory

    if (login == NULL) perror("login");	//login:fail then error
    if (-1 == gethostname(hostname, 256)) perror("hostname");	//get hostname
	///print out prompt
    printf("%s", login);
    printf("%s", "@");
    printf("%s ", hostname);
    printf("%s ", "$");

    getline(cin,input);	//get user input cmd
}

vector<string>ls_cmd (string& input) //converts string into individal commands
{
    vector<string> word;
	string tmp="";
	for(unsigned int i=0;i<input.size();i++)	//analyze user input stream
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
            word.push_back(";"); break;
        }
        else if(input[i]==';')
        {
            if(tmp.size()>0){ word.push_back(tmp); tmp="";}
            word.push_back(";"); continue;
        }
        else if(input[i]=='<')
        {
            //Todo: Here process the current input and its related "<" "<<<"
            if(tmp.size()>0){word.push_back(tmp); tmp="";}
            if(input[i+1]=='<' && input[i+2]=='<'){word.push_back("<<<"); i+=2; continue;}
            word.push_back("<"); continue;
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
            tmp+=input[i];	//continue to gather input char
        }
    }
	 //end of for-i-loop
	if(tmp.size()>0)word.push_back(tmp);
	word.push_back(";");	//at the end push a ';'
	return word;
}

void exec_cmd(vector<string>&xwords)
{
	vector<string>word;
	word = check_extra_credit(xwords);
	struct str_direct r;

	int total = word.size();
	int count = 0;
	int task, status, pid;
	int redirect_out;

	for(int i=0;i<total;i++ )
	{
		if(word[i]=="&&"||word[i]=="||"||word[i]==";")
		{
			r.cmdlist[count]=NULL;
			calculate_num_subtask(r);	//calculate # of subtasks
			prepare_pipes(r);	//prepare pipes for subtasks

			for(task=0; task<=r.task_count;task++
            {
                update_current_task_list(r,task)

                if((task=fork())<0)
                {
                    perror("fork");
                    exit(-1);
                }
                else if(pid==0)
                {
                    if(task==0)
                    {	//setup task0
                        int file_or_pipe=setup_task0(r);
                        if(strcmp(r.current_task[r.cmd_start],"ls")==0)
                        {
                            ls_cmd(&r.current_task[r.cmd_start]);
                            //ls_cmd(&r.current_task[r.cmd_start],file_or_pipe);
                            exit(0);
                        }
                        if(execvp((const char *)r.current_task[r.cmd_start],(char* const*)&r.current_task[r.cmd_start])<1)
                            perror(r.current_task[r.cmd_start]);
                        exit(1);
                    }
                    else if(task==r.task_count)
                    {	    //last subtask
                        redirect_out=setup_task_end(r);
                        if(redirect_out==-1)
                        {
                            perror("No output filename");
                            exit(255);
                        }

                        if(execvp((const char*)r.current_task[r.cmd_start],(char* const*)&r.current_task[r.cmd_start])<0)
                        perror(r.current_task[r.cmd_start]);
                        exit(-1);

                    }// end if task==r.task_count
                    else
                    {		//In Between subtask operation

                        redirect_out=setup_task_inBetween(r,task);
                        if(redirect_out==-1)
                        {
                            perror("No output filename");
                            exit(255);
                        }

                        if(execvp(r.current_task[r.cmd_start],(char* const*)&r.current_task[r.cmd_start])<0)
                        perror(r.current_task[r.cmd_start]);
                        exit(-1);

                    }
                }
            }//end of for-task-loop

//Parent comes Here
			status=wait_until_all_tasks_over(r);
			//determine next i=words-task
			i=process_if_then_else(words,status,i,total);

			count=0;


		}
		else
		{
			if(count==0 && words[i]=="exit")exit(0);	//exit from this Program
			r.cmdlist[count++]=(char *)words[i].c_str();
		}
	}//end of for-total-loo
}

#endif // _SHELL_H__
