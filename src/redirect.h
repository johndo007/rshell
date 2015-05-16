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

using namespace std;

int check_pipe_ldir(vector<string>&inwords,vector<string>&outwords)
{
	string tmp = "";
	int pcount =0;
	int totalWordCount = inwords.size();
    	for(int x=0;x<totalWordCount;x++)
        {
		tmp=inwords[x];
		if(inwords[x]=="|") pcount +=1;
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
	return pcount;
}
void setup_pipe(char **cmdlist,int &count,int* &current_task_start, int &task_index, int &l,int* &current_task_end, int &end_task,int* &status,int* &pipes)
{
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
}

#endif // _REDIRECT_H__

