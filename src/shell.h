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
                    word+='#';
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
            }
            else {word="";}
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
                    }
                }
            }
            else if(input.at(cnt)=='|') //case4 - bar |
            {
                if(word != "" && !word.empty())	//with no space
                {
                    v.push_back(word);
                    totalWordCount++;
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
                    }
                }
            }
            else if(input.at(cnt)=='>') //case5 - redirect out >
            {
                if(word != "" && !word.empty())	//with no space
                {
                    v.push_back(word);
                    totalWordCount++;
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
                    }
                }

                else    //with space
                {
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

                    }
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

    void exec_cmd(vector<string>&words,int& totalWordCount)
    {
            const int finalWordCount = totalWordCount+1;
            /*
            for(int i=0;i<finalWordCount-1;i++)
            cout<<"Argv="<<i<<":"<<words[i]<<endl;
            return;
            */

            char **cmdlist = new char*[finalWordCount];
            int count = 0;
            int index_else=-1;
            //int index_then=-1;

            for(int i = 0; i < totalWordCount; i++)
            {
                if((words[i] == ";") || words[i] == "||" || (words[i] == "&&"))
                {
                    //execute the command

                    int pid = 0;
                    int status = 0;
                    bool abort = false; // meaning ls has not been called

                    pid = fork();

                    if(pid <= -1) // something went wrong
                    {
                        perror("ERROR [FORK]: CHILD FAILED\n");
                        for(int j =0;j<count;j++)
                        {
                            delete cmdlist[j];
                        }
                        delete cmdlist;
                        exit(1); // quit the program
                    }

                    if(pid == 0 && abort == false) //starting  child process
                    {
                        cmdlist[count] = NULL;


                        int success;
                        if(strcmp(cmdlist[0],"ls")==0)
                        {
                            //cout<<"ls was called"<<endl;
                            ls_cmd(cmdlist);
                            abort = true;
                            success =0;
                        }

                        if(pid==0 && abort  == false)

                        success = execvp(cmdlist[0], &cmdlist[0]);
                        if(success <= -1 ) // nope, it failed ... failed on command
                        {

                            perror(cmdlist[0]);
                            //exit(1);
                            //cerr<<"CMD Error:  "<<cmdlist[0]<<endl;
							int x;
                            index_else =-1;
                            if(words[i]== "&&")
                            //start copying here
                                x = i+1;	//skip && and look for next avaiable connector
                            else
                                x = i;	//look for next possible connector
                            for( ; x < totalWordCount; x++)
                            {
                                if(words[i]=="||"||words[i]=="&&"||words[i]==";")
                                {
                                    index_else = x;
                                    break;
                                }
                            }
                            if(index_else>0)
                            {
                                i=index_else;
                                index_else =-1;
                            }

                        }

                        for(int j = 0; j < count; j++)
                        {
                            delete cmdlist[j];
                            cmdlist[j] = NULL;
                        }

                        exit(success);
                    }

                    else // parent process---wait until the child is done
                    {
                        if(waitpid(-1, &status, 0)==-1)
						{
							perror("waitpid" );
						}
                        //if(fwmode>0)close(fw);

                        if(words[i] == "&&" && (status > 0))	//failed on prameter
                        {
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
                        if(words[i] == "||" && (status == 0))
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
                    }
                     // reset the list
                     for(int j = 0; j < count; j++)
                     {
                        delete cmdlist[j];
                        cmdlist[j] = NULL;
                     }
                     count = 0;
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
                        exit(1);
                    }
                    cmdlist[count] = new char[words[i].size()+1];
                    copy(words[i].begin(), words[i].end(), cmdlist[count]);
                    cmdlist[count][words[i].size()] = '\0';
                    count++;
                }
            }
            //normal case  memory clean up
            for (int j = 0;j<count;j++)
            {
                delete cmdlist[j];	//delete the argument content
            }
            delete cmdlist;	//delete argument pointers
            totalWordCount = 0;

}

#endif // _SHELL_H__
