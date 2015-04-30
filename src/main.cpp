#include <iostream>
#include <vector>
#include <string>
#include "shell.h"

using namespace std;

int main(int argc, char** argv)
{
	string input;		//get user cmd in one line
    	vector<string>words;	//seperate the user input into vector of strings
	int wordcnt = 0;	//word counter to keep track of postion

	while(true)
	{
        prompt(); //Setup user prompt

	getline (cin, input);

        ///break up commands into vector<string>
        list_cmd(input, words,wordcnt);

        ///execute commands
        exec_cmd(words,wordcnt);
	}
	return 0;
}
