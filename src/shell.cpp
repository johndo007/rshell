
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
//#include "ls_cmd.h"
//#include "redirect.h"

using namespace std;
struct str_direct
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

vector<string> check_extra_credit(vector<string>inwords);
void calculate_num_subtask(struct str_direct &r);
void prepare_pipes(struct str_direct &r);
void update_current_task_list(struct str_direct &r, int task);
int setup_task0(struct str_direct &r);
int setup_task_end(struct str_direct &r);
int setup_task_inBetween(struct str_direct &r,int task);
int wait_until_all_tasks_over(struct str_direct &r);
int process_if_then_else(vector<string>&words,int status,int i,int total);
void ls_cmd(char **argv,int fp);
extern int my_state;	//control C handling
extern int my_bg_status; //control Z handling
extern int my_current_pid;
extern char *myfifo;

bool replaceHOME(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void prompt(string &input)   //get user info ... DONE
{
    char* login = getlogin();	//get username
    char hostname[256];			//allocated char memory
    char cpath[256];			//hold current directory info

    if (login == NULL) perror("login");	//login:fail then error
    if (-1 == gethostname(hostname, 256)) perror("hostname");	//get hostname
	///print out prompt
    printf("%s", login);
    printf("%s", "@");
    printf("%s:", hostname);
    printf("%s:", ": ");
    if(getcwd(cpath, 255)<0)perror("Error:getcwd");
    char* pHome;
	//std::string home_char "~";
	if (0 == (pHome = getenv("HOME"))) {
	    perror("getenv");
	    exit(1);
	}
	// HOME case
	string tempcwd(cpath);
	string thome(pHome);
	replaceHOME(tempcwd, thome, "~");
    
    cout<<tmpcwd.c_str();
    printf("%s ", "$");
    
	my_state=0;			//control C - state 0 -- waiting for input
    getline(cin,input);	//get user input cmd
}

vector<string> extract_ls_cmd (string& input) //converts string into individal commands
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
void cd_cmd(struct str_direct &r)
{
	if(strcmp(r.current_task[r.cmd_start],"cd")==0)
		{
				
			//r.current_task[r.cmd_start]=NULL;	//eliminate race condition
			//cerr<<"Trying to read Dir="<<current_task[cmd_start+1]<<endl;
			string myhome;
			string oldpwd;
			char *msg;
			char lpath[1024];
			//process "cd" or "cd ~"
			if((r.current_task[r.cmd_start+1]==NULL)||(strcmp(r.current_task[r.cmd_start+1],"~")==0))
			
			{	//cd without any argument
				myhome="";
				msg=getenv("HOME");
				if(msg==NULL)perror("Error:getlogin");
				else
				myhome +=msg;   //home dir
								
				r.current_task[r.cmd_start+1]=(char *)myhome.c_str();	//updated with usr home path
				
			}else if(strcmp(r.current_task[r.cmd_start+1],"-")==0)		//process "cd -"
			{
				oldpwd="";
				msg=getenv("OLDPWD");
				if(msg==NULL)perror("Error:getlogin");
				else
				oldpwd +=msg;   //previous path
				
				
				r.current_task[r.cmd_start+1]=(char *)oldpwd.c_str();			//updated with previous path
			}
			
			if(getcwd(lpath,1023)<0)perror("Error:getcwd");						//save current path as previous path
				else if(setenv("OLDPWD",lpath,1)<0)perror("Error:setenv");		//update OLDPWD if available	
			if(chdir(r.current_task[r.cmd_start+1])<0)perror("Error:chdir");	//process chdir()
				
			//cerr<<"End of cd"<<endl;
		}
}	
void exec_cmd(vector<string>&xwords)
{
	vector<string>words;
	words = check_extra_credit(xwords);
	struct str_direct r;
	int fd;
	char buf[32];
	int total = words.size();
	int count = 0;
	int task, status, pid;
	int redirect_out;
//cout<<"exec:"; for(int i=0;i<total;i++)cout<<words[i]<<" ";
//cout<<endl;
	for(int i=0;i<total;i++ )
	{
		if(words[i]=="&&"||words[i]=="||"||words[i]==";")
		{
			r.cmdlist[count]=NULL;
			calculate_num_subtask(r);	//calculate # of subtasks
			prepare_pipes(r);	//prepare pipes for subtasks

			for(task=0; task<=r.task_count;task++)
            {
                update_current_task_list(r,task);

                if((pid=fork())<0)
                {
                    perror("fork");
                    exit(-1);
                }
                else if(pid==0)
                {
					my_state=2;		//control C	- child process in progress
                    if(task==0)
                    {	//setup task0
						
                        int file_or_pipe=setup_task0(r);
                        //if(file_or_pipe==1)file_or_pipe=2;
                        if(strcmp(r.current_task[r.cmd_start],"ls")==0)
                        {
                            //ls_cmd(&r.current_task[r.cmd_start]);
                            ls_cmd(&r.current_task[r.cmd_start],file_or_pipe);
                            exit(0);
                        }
                        else if(strcmp(r.current_task[r.cmd_start],"cd")==0)
                        {
                        //		if(chdir(r.current_task[r.cmd_start+1])<0)
                        //		perror("Error:chdir");
                        		exit(0);
						}
						else if(strcmp(r.current_task[r.cmd_start],"fg")==0)
						{
							if(my_bg_status==20)
							{
								my_bg_status=0;
								raise(SIGCONT);
								cerr<<"Finishing the background processing..."<<endl;
								exit(0);
							}
							else
							{
								perror("No forground process");
								exit(0);
							}
						}
						
						//char buf[32];
						my_current_pid=getpid();
						if((fd=open("myfifo",O_WRONLY))==-1)
						{
							perror("Child cannot open FIFO write");
							exit(1);
						}
						sprintf(buf,"%ld",(long)getpid());
						int msize=strlen(buf)+1;
						if(write(fd,buf,msize)!=msize)
						{
							cerr<<"Child write to FIFO failed"<<buf<<endl;
							exit(1);
						}
						close(fd);
						//cerr<<"Current PID="<<my_current_pid<<endl;
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

			if((fd=open("myfifo",O_RDONLY|O_NONBLOCK))==-1)
		{
			perror("Parent cannot open1 FIFO");
			exit(1);
		}
		//if(read(fd,buf,32)<=0)
			{
				//perror("Parent read from FIFO1 failed\n");
				//exit(1);
			}

		    my_state=1;		//control C	--child process is over but wait until i/o pipe finish
			if(strcmp(r.current_task[r.cmd_start],"cd")==0)
				{
					cd_cmd(r);
				}
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
