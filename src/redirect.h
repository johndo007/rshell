#ifndef _REDIRECT_H__
#define _REDIRECT_H__

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
#include "shell.h"

using namespace std;

vector<string> check_extra_credit(vector<string>inwords)

{
	string tmp = "";
	vector<string> outwords;
	int totalWordCount = inwords.size();
    for(int x=0;x<totalWordCount;x++)
    {
		tmp=inwords[x];
		if(inwords[x]!="cat")
		{
			outwords.push_back(tmp); //xwords[x]);
		}
		else
		{
			if( (x+2)<totalWordCount)
			{
				if(inwords[x+1]=="<<<")
				{
					outwords.push_back("echo"); //xwords[x]); //push cat
					outwords.push_back(inwords[x+2]); //"....word.."
					outwords.push_back("|"); //  |
					outwords.push_back("cat");
					x+=2;	//skip "
				}
				else outwords.push_back(tmp);
			}
			else outwords.push_back(tmp);

		}

	}
	return outwords;
}
void calculate_num_subtask(struct str_direct &r)
{
	//calculate the number of subtask
	r.task_count=0;
	r.task_start[0]=0;

	int n = 0;
	for(n=0;r.cmdlist[n]!=NULL;n++)
	{
		if(strcmp(r.cmdlist[n],"|")==0)
		{
			r.task_end[r.task_count]=n;
			r.task_count++;
			r.task_start[r.task_count]=n+1;
		}
	}
	r.task_end[r.task_count]=n;
	//end of calculating the # of subtasks
}

void prepare_pipes(struct str_direct &r)
{
	if(r.task_count>0)
	{
		for(int ii=0;ii<r.task_count;ii++)
		{
			if(pipe(r.pipes+ii*2)<0)	perror("Error: set pipe");
		}
	}
}


void update_current_task_list(struct str_direct &r, int task)
{

	int ii=0;
	int n =0;
	for(n=r.task_start[task];n<r.task_end[task];n++)
	{
		r.current_task[ii++]=r.cmdlist[n];
	}
	r.current_task[ii]=NULL;
	r,cmd_start=0;
}

int setup_task0(struct str_direct &r)
{
	int redirect_out=0;
	r.in=-1;
	r.out=-1;
	int s;

	bool found_out = false;

	for(s =0; r.current_task[s]!=NULL;s++)
	{
		if(strcmp(r.current_task[s],">")==0)
		{
			found_out = true;
			break;
		}
		if(strcmp(r.current_tasks[s],">>")==0)
		{
			found_out = true;
			break;
		}

	}
	if(found_out)
	{
		redirect_out = 1;
		if(strcmp(r.current_task[s],">")==0)
		{
			r.out=open(r.current_task[s+1],O_WRONLY|O_TRUNC|O_CREAT, S_IRUSR|S_IRGRP|S_IWGRP|S_IRGRP|S_IWGRP|S_IWUSR);
			if(r.out<0)	perror("open");
		}
		//check if error ouput channel ==2
		if((s-1>0)&& strcmp(r.current_task[s-1],"2")==0)
		{
			if(dup2(r.out,2)<0)	perror("dup2 - out ");
			r.current_task[s-1]==NULL;
		}
		else
		{
			if(dup2(r.out,1)<0)	perror("Error: dup2 2");
			r.current_task[s-1]=NULL;
		}
		else
		if(dup2(r.out,1)<0) perror("Error: dup2 1");
		if(s==0)
		{
			r.cmd_start=s+2;
		}
		r.current_task[s]=NULL;
	} //end of output redirection

	bool found_in=false;
	for(s=0;r.current_task[s]!=NULL;s++)
	{
		if(strcmp(r.current_task[s],"<")==0)
		{
			found_in=true;
			break;
		}
	}
	if(found_in)
	{
		r.in = open(r.current_task[s+1],O_RDONLY);
		if(r.in<0) perror("Error: open read");
		if(dup2(r.in,0)<0) perror("Error: dup2 0");
		r.current_task[s]=NULL;
	}
	if(r.task_count>0)
		for(int ii=0;ii<r.task_count*2;ii++)
			if(close(r.pipes[ii]<0) perror("ErrorL close pipe");
	if(found_in)
		if(close(r.in)<0) perror("Error: close input file");
	if(found_out)
		if(close(r.out<0) perror("Error: close output file");

	return redirect_out;

}

int setup_task_end(struct rs &r)
{
	r.in=-1;  r.out=-1;	//reset file handles
	if(r.task_count>0)
		if(dup2(r.pipes[0],0)<0)perror("Error:dup2 0");	//input from the lowest pipe
		int s;
		bool found=false;

		for(s=0;r.current_task[s]!=NULL;s++)
		{
			if(strcmp(r.current_task[s],">")==0)
			{
				found=true;
				break;
			}
			if(strcmp(r.current_task[s],">>")==0)
			{
				found=true;
				break;
			}
		}
		if(found)
		{
			if(r.current_task[s+1]==NULL)
			{
				return -1;	//error:No output filename
			}
			else
			if(strcmp(r.current_task[s],">")==0)
			{
				//cerr<<"Outputfile="<<current_task[s+1]<<endl;
			r.out=open(r.current_task[s+1],O_WRONLY|O_TRUNC|O_CREAT,
									S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
			if(r.out<0)perror("Error: open output file");
			}else
			{
				r.out=open(r.current_task[s+1],O_WRONLY|O_APPEND,
									S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
				if(r.out<0)perror("Error: open output file");
			}
			//check if error output channel==2
			if((s-1 >0)&& strcmp(r.current_task[s-1],"2")==0)
			{
				if(dup2(r.out,2)<0)perror("Error: dup2 2");
				r.current_task[s-1]=NULL;
			}
			else
			if(dup2(r.out,1)<0)perror("Error: dup2 1");;
			r.current_task[s]=NULL; 	//command only, no parameters
		}//end of //Handle any OUTPUT redirection

		// Handle any INPUT redirection
		bool found_in=false;
		for(s=0;r.current_task[s]!=NULL;s++)
		{
			if(strcmp(r.current_task[s],"<")==0)
			{
				found_in=true;
				break;
			}

		}
		if(found_in)
		{
			//cerr<<"Inputfile="<<current_task[s+1]<<endl;
			r.in=open(r.current_task[s+1],O_RDONLY);
			if(r.in<0)perror("Error: open read");
			if(dup2(r.in,0)<0)perror("Error:dup2 0");
			r.current_task[s]=NULL; 	//command only, no parameters
		} //end of Handle any INPUT redirection


		if(r.task_count>0)for(int ii=0;ii<r.task_count*2;ii++)if(close(r.pipes[ii])<0)perror("Error:close pipe");; //Important! Close all pipes
		if(found_in)if(close(r.in)<0)perror("Error:close input file");										// input file handle
		if(found)if(close(r.out)<0)perror("Error:close output file");										// and output file handle before child process..

		return 0;
}
int setup_task_inBetween(struct rs &r,int task)
{
	int redirect_out;
	r.in=-1; r.out=-1; //clear file handles
	if(r.task_count>0)
		{
			if(dup2(r.pipes[(r.task_count-task)*2-1],1)<0)perror("Error:dup2 1");		//output to next task below
			if(dup2(r.pipes[(r.task_count-task)*2],0)<0)perror("Error:dup2 0");		//input from the pipe above
			//for(int ii=0;ii<r.task_count*2;ii++)if(close(r.pipes[ii])<0)perror("Error:close pipe");	//Important!! close all pipes before execvp...
		}
	int s;
	bool found=false;

	for(s=0;r.current_task[s]!=NULL;s++)
	{
		if(strcmp(r.current_task[s],">")==0)
		{
			found=true;
			break;
		}
		if(strcmp(r.current_task[s],">>")==0)
		{
			found=true;
			break;
		}
	}
	if(found)
	{
		if(r.current_task[s+1]==NULL)
			return -1; //Error:exit
		//cerr<<"Outputfile="<<current_task[s+1]<<endl;
		if(strcmp(r.current_task[s],">")==0)
		{
			//cerr<<"Outputfile="<<current_task[s+1]<<endl;
		r.out=open(r.current_task[s+1],O_WRONLY|O_TRUNC|O_CREAT,
								S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
		if(r.out<0)perror("Error:open write");
		}else
		{
			r.out=open(r.current_task[s+1],O_WRONLY|O_APPEND,
								S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
			if(r.out<0)perror("Error:open write");
		}
		//check if error output channel==2
		if((s-1 >0)&& strcmp(r.current_task[s-1],"2")==0)
		{
			if(dup2(r.out,2)<0)perror("Error:dup2 2");;
			r.current_task[s-1]=NULL;
		}
		else
		if(dup2(r.out,1)<0)perror("Error: dup2 1");;
		redirect_out=1;
		r.current_task[s]=NULL; 	//command only, no parameters
	}//end of //Handle any OUTPUT redirection

	// Handle any INPUT redirection
	bool found_in=false;
	for(s=0;r.current_task[s]!=NULL;s++)
	{
		if(strcmp(r.current_task[s],"<")==0)
		{
			found_in=true;
			break;
		}

	}
	if(found_in)
	{
		r.in=open(r.current_task[s+1],O_RDONLY);
		if(r.in<0)perror("Error:open read");
		if(dup2(r.in,0)<0)perror("Error:dup2 0");
		r.current_task[s]=NULL; 	//command only, no parameters
	} //end of Handle any INPUT redirection

	if(r.task_count>0)for(int ii=0;ii<r.task_count*2;ii++)if(close(r.pipes[ii])<0)perror("Error:close pipe m");; //Important! Close all pipes
	if(found_in)if(close(r.in)<0)perror("Error:close input file");										// input file handle
	if(found)if(close(r.out)<0)perror("Error:close output file");										// and output file handle before child process..
	return redirect_out;
}

int wait_until_all_tasks_over(struct rs &r)
{
	int status;
			if(r.task_count>0)
				for(int ii=0;ii<r.task_count*2;ii++)
					if(close(r.pipes[ii])<0)perror("Error:pipe p");

			for(int ii=0;ii<=r.task_count;ii++)
					if(wait(&status)<0)perror("Error:wait");
	return status;
}

int process_if_then_else(vector<string>&words,int status,int i,int total)
{
			if(status==0)
			{
				//the current process is ended in a NORMAL condition
				if(words[i]=="||")	//if last ending location(i) =="||"
				{//skip FAIL condition="||" until the argument= ";" or "&&"
					int x=i+1;
					for(;x<total;x++)
					if(words[x]==";" ||words[x]=="&&")
					{
						i=x;
						break;	//if found, it is necessary to BREAK away from for-x-loop
					}
				}
			}
			else
			{
				//the current process is ended in a FAIL condition
				if(words[i]=="&&")	//if last ending location(i)=="&&"
				{//seek the operation=="||" or ";" to handle the FAILURE
					int x=i+1;
					for(;x<total;x++)
					if(words[x]=="||" || words[x]==";")
					{
						i=x;
						//count=0;
						break;	//if found, it is necessary to BREAK away from for-x-loop
					}
				}

			}//end of if(status==0)
	return i;
}



#endif // _REDIRECT_H__

