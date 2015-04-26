#ifndef LS_CMD_H__
#define LS_CMD_H__

#include <vector>
#include <dirent.h>
#include <cstring>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

//defining bit values
#define DIR_ALL  1	//-a
#define DIR_LONG 2	//-l
#define DIR_R	 4  //-R

vector<string> read_filenames(string &path)
{
	//cout<<"running readfile names"<<endl;
	//cout<<"Path: " <<path<<endl;
	DIR*  dir_ptr = opendir(path.c_str());
	vector<string> filenames;
	dirent* dir_en;
	filenames.clear();
	if(dir_ptr)
	{
		//cout<<"open dir"<<endl;
		while(true)
		{
			//int errno = 0;
			dir_en = readdir(dir_ptr);
			if(dir_en == NULL) break;
			{
				filenames.push_back(std::string(dir_en -> d_name));
			}
		}
		closedir(dir_ptr);
		sort(filenames.begin(), filenames.end() );
	}
	return filenames;

}
void make_string(string &path, string &fname, string &output)
{
    string fullpath;
    fullpath = path;
    fullpath += '/';
    fullpath += fname;
    output = ""; //clear

    if(stat(full.c_str(), &statBuf)==-1)    return output;

    get_perms(statBuf.st_mode, fstatus);
    output = fstatus;
    sprintf(tmp,"%d", statBuf.st_link);
    output += tmp;

    if((pwd=getwuid(statBuf.st_uid))!=NULL)
        sprintf(tmp,"%-s", pwd->pw_name);
    else
        sprintf(tmp,"%-d", statBuf.st_uid);
    output += tmp;  //-perm-#-user-user

    if((grp=getgrid(statBuf.st_gid))!=NULL)
        sprintf(tmp,"%-s", grp->gr_name);
    else
        sprintf(tmp,"%-d", grp->gr_name);
    output += tmp;

    sprintf(tmp, "%7jd", (int max_t)statBuf.st_size);
    output += tmp;

    char datestring[256];
    char fmt = "%b %d %H: %M";
    tm = localtime(&statBuf, st_mtime);
    strftime(datestring, sizeof(datestring),fmt,tm);
    output += datestring;
}

void print_ALL(string &path, vector<string> &v, int &mode)
{
	//cout<<"Print all"<<endl;
	if((mode & DIR_LONG) && (mode & DIR_R))
	{
		//print -alR
	}
	else if(mode & DIR_LONG)
	{
		//print  -al
		for(int x = 0; x <v.size();x++)
        {
            string ouput;
            make_string(path, v[x], output, mode);
            cout<<output;
        }

	}
	else if(mode & DIR_R)
	{
		//print -aR
	}
	else
	{
		//cout<<"Size of v: "<<v.size()<<endl;
		//default print -a
		for(int i =0; i<v.size();i++)
		{
			cout<<v[i]<<"  ";
		}
		cout<<endl;
	}


}

void print_LONG(string &path, vector<string> &v, int &mode)
{
	cout<<"-l called"<<endl ;
	for(int x = 0; x <v.size();x++)
	{
        if(v[x][0] !='.')   //no hidden files
        {
            string ouput;
            make_string(path, v[x], output, mode);
            cout<<output;
        }
    }
}

void print_R(string &path, vector<string> &v, int &mode)
{
	cout<<"-R called"<<endl ;
}

void ls_cmd(char** argv)
{
	//cout<<"ls_cmd  was called"<<endl;
	//assume ls was called
	//DIR *mydir;
	int x = 1;
	int mode = 0;
	string path = ".";
	vector<string> v;
	//vector<bool> flag;
	//for(int i = 0; i < 3;i++)  flag.push_back(false);
	/*for(int i = 0 ;argv[i]!=NULL ;i++)
	{
		cout<<argv[i]<<endl;
	}*/


	for(;argv[x]!=NULL ;x++ )
	{

		//cout<<"ls_cmd1  was called"<<endl;
		if(argv[x][0]!='-' && x!=0)
		{
			cout<<"argv[x]="<<argv[x]<<endl;
			path = argv[x];
		}
		//first part of parameter uses '-'
		if(argv[x][0]=='-')
		{
			//-a -l -R
			//cout<<"ls_cmd argv ' -' was called"<<endl;
			for(int y = 1; argv[x][y]!='\0'; y++)
			{
				char input = argv[x][y];
				switch(input)
				{
					case 'a':
						mode |= DIR_ALL;
						break;
					case 'l':
						mode |= DIR_LONG;
						break;
					case 'R':
						mode |= DIR_R;
						break;
					default:
						cout<<"Unknown command: "<< input <<endl;
						break;

				}
			}

			//push parameters into ls helper  read function
			v = read_filenames(path);
			//cout<<"ls_cmd read paths  was called"<<endl;
			/*for(int i = 0; i< v.size();i++)
			{
				cout<<v[i]<<endl;
			}*/

			if(mode & DIR_ALL) print_ALL(path,v,mode);	//if -a is true

			if(mode & DIR_LONG) print_LONG(path,v,mode);	//if -l is true

			if(mode & DIR_R) print_R(path,v,mode);	//if -R is true

		}
		else
		{
			//assume path name
			path = argv[0];
			v = read_filenames(path);
			for(int i = 0; i<v.size();i++)
			{
				cout<<v[i]<<endl;
			}
		}

	}
}


#endif // LS_CMD_H__


