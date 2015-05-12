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

void check_pipe_ldir(vector<string>&inwords,vector<string>&outwords, int &pcount)
{
    for(int x=0;x<totalWordCount;x++)
        {
			tmp=inwords[x];
			if(xwords[x]=="|") pcount +=1;
			if(xwords[x]!="cat")
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
							outwords.push_back("|");		//  |
							outwords.push_back("cat");
							x+=2;						//skip "


						}else outwords.push_back(tmp);
				}
				else outwords.push_back(tmp);

			}

		}
}




// <,>,>>,|
#endif // _REDIRECT_H__

