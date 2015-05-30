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
#include <grp.h>
#include <dirent.h>
#include <sys/ioctl.h>	//Terminal width info
#include <signal.h>		//control C
#include <setjmp.h>		//control C
using namespace std;

void prompt(string &input);   //get user info ... DONE
vector<string> extract_ls_cmd (string& input) ;
void exec_cmd(vector<string>&xwords);

static volatile sig_atomic_t jumpok=0;	//control C handling
static sigjmp_buf jmpbuf;	//control C handling
int my_state=0;	//control C handling
int my_bg_status;
int my_current_pid;
int bg_pid;
const char *myfifo="myfifo";

//static string findpath;	//find or cmd path operation
void int_handler(int x)
{
	if(jumpok==0)return;
	my_bg_status=x;
	//if(errno==SIGTSTP)my_error=4;
	//if(errno==SIGINT)
	bg_pid=getpid();
	siglongjmp(jmpbuf,1);
}


int main(int argc, char** argv)
{
	string input;			//get user cmd in one line
   	vector<string>words;	//holds all user input
	mode_t fifo_mode=S_IRUSR|S_IWUSR;
	int fd;
	//int status;
	char buf[32];
	if((mkfifo("myfifo",fifo_mode)==-1)&&(errno != EEXIST))
	{
		cerr<<"Could not create a named pipe:"<<myfifo<<endl;
	}
   
	
	//---prepare control c return
		
	struct sigaction act;
	act.sa_handler= (__sighandler_t)int_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;
	if(sigaction(SIGINT,&act,NULL)<0)
	{
		perror("Error setting up SIGINT handler");
		
		exit(1);
	}
	if(sigaction(SIGTSTP,&act,NULL)<0)
	{
		perror("Error setting up SIGTSTP handler");
		exit(1);
	}
	if(sigsetjmp(jmpbuf,1))
	{
		cerr<<" Back to main loop state="<<my_state<<" bg_status="<<my_bg_status<<endl;
		if((fd=open("myfifo",O_RDONLY|O_NONBLOCK))==-1)
		{
			perror("Parent cannot open2 FIFO");
			exit(1);
		}
		if(my_bg_status==20)
		{
			//bg_pid=my_current_pid;
			cerr<<"Wait for FIFO ready..."<<endl;
			//while((wait(&status)==-1) && (errno==EINTR));
			if(read(fd,buf,32)<=0)
			{
				perror("Parent read from FIFO2 failed\n");
				exit(1);
			}
			
			cerr<<" detected control Z PID="<<buf<<endl;
		
		}
		else
		{
			if(my_state==2)exit(-1);	//exit from the previous Child process
			
			if(my_state==1)				//wait until the current process over...
			{
				int status;
				if(wait(&status)<0)perror("Error:wait");
			}
		}
		
	}
	jumpok=1;
	//---end of preparation control C
	while(true)
	{
        prompt(input);				//Setup user prompt and input
       
	    words = extract_ls_cmd(input);	//return vector<string>
	   
        exec_cmd(words);			//execute command
	}
	return 0;
}
