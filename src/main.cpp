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
using namespace std;

void prompt(string &input);   //get user info ... DONE
vector<string> extract_ls_cmd (string& input) ;
void exec_cmd(vector<string>&xwords);



int main(int argc, char** argv)
{
	string input;			//get user cmd in one line
   	vector<string>words;	//holds all user input
	
	while(true)
	{
        prompt(input);				//Setup user prompt and input
       
	    words = extract_ls_cmd(input);	//return vector<string>
	   
        exec_cmd(words);			//execute command
	}
	return 0;
}
