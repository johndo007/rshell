#include <iostream>
#include <vector>
#include <string>
#include "shell.h"

using namespace std;

int main(int argc, char** argv)
{
	string input;			//get user cmd in one line
   	vector<string>words;	//holds all user input
	
	while(true)
	{
        prompt(input);				//Setup user prompt and input
	    words = list_cmd(input);	//return vector<string>
        exec_cmd(words);			//execute command
	}
	return 0;
}
