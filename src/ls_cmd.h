#ifndef LS_CMD_H__
#define LS_CMD_H__

#include <iostream>
#include <vector>
#include <dirent.h>
#include <cstring>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>	//Terminal width info

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
void get_perms(mode_t mode, char buf[])
{
	char ftype = '?';
	if(S_ISREG(mode)) ftype = '-';
	if(S_ISDIR(mode)) ftype = 'd';
	sprintf(buf, "%c%c%c%c%c%c%c%c%c%c",ftype,
			mode & S_IRUSR ? 'r' : '-',
			mode & S_IWUSR ? 'w' : '-',
			mode & S_IXUSR ? 'x' : '-',
			mode & S_IRGRP ? 'r' : '-',
			mode & S_IWGRP ? 'w' : '-',
			mode & S_IXGRP ? 'x' : '-',
			mode & S_IROTH ? 'r' : '-',
			mode & S_IWOTH ? 'w' : '-',
			mode & S_IXOTH ? 'x' : '-');

}

void get_color_code (mode_t st_mode, char *color_fmt)
{
	if(S_ISDIR(st_mode))
	{
		sprintf(color_fmt, "\x1b[34m");
	}
	else if (st_mode & S_IXUSR)
	{
		sprintf(color_fmt, "\x1b[32m");
	}
}


bool stringCompare( const string &left, const string &right )
{
   for( string::const_iterator lit = left.begin(), rit = right.begin(); lit != left.end() && rit != right.end(); ++lit, ++rit )
      if( tolower( *lit ) < tolower( *rit ) )
      {
         return true;
	 }
      else if( tolower( *lit ) > tolower( *rit ) )
       {  
		   return false;
	   }
   if( left.size() < right.size() )
      {
		  return true;
	  }
   return false;
}

void print_ALL(string &path, vector<string> &v, int mode);

string make_string(string &path, string &fname, string &output, int &mode)
{
    struct stat statBuf;
	char fstatus[16];
	char tmp[32];
	struct passwd *pwd;
	struct group *grp;
	struct tm *tm;

	string fullpath;
    fullpath = path;
    fullpath += '/';
    fullpath += fname;
    output = ""; //clear

    if(stat(fullpath.c_str(), &statBuf)==-1)    return output;
	if(mode & DIR_LONG)
	{
		get_perms(statBuf.st_mode, fstatus);
		output = fstatus;
		output +=' ';
		sprintf(tmp,"%ld", statBuf.st_nlink);
		output += tmp;
		output += ' ';
		//get owner's name
		if((pwd=getpwuid(statBuf.st_uid) )!=NULL)
			sprintf(tmp,"%-s", pwd->pw_name);
		else
			sprintf(tmp,"%-d", statBuf.st_uid);
		output += tmp;  //-perm-#-user-user
		output += ' ';
		//get groups's name
		if((grp=getgrgid(statBuf.st_gid))!=NULL)
			sprintf(tmp,"%-s", grp->gr_name);
		else
			sprintf(tmp,"%-d", statBuf.st_gid);
		output += tmp;
		output += ' ';
		//get file size
		sprintf(tmp, "%11ld", (long int)statBuf.st_size);
		output += tmp;
		output += ' ';
		//get Month:Day:Hour::Minute
		char datestring[256];
		char fmt[] = "%b %d %H: %M";
		tm = localtime(&statBuf.st_mtime);
		strftime(datestring, sizeof(datestring),fmt,tm);
		output += datestring;
		output += ' ';
	}
	char color_fmt[32];
	color_fmt[0] = 0;
	char default_fmt[]= "\x1b[39;49m";
	if(fname[0] == '.')
	{
		sprintf(color_fmt,"%s", "\x1b[34;47m");
	}
	else
	{
		get_color_code(statBuf.st_mode, color_fmt);
	}
	sprintf(tmp, "%s%s%s", color_fmt, fname.c_str(), default_fmt);
	output += tmp;
	return output;
}

void recurdir3(string &path, vector<string> &v, int mode)
{
    DIR* dir_ptr;
	string temp_path;
	vector<string>tempv;
    print_ALL(path,v,mode);   //print current Dir filenames
	for(unsigned int x=0;x<v.size();x++)
    {
        if(v[x]==".")continue;
        if(v[x]!="..")
        {
            temp_path=path;
            temp_path+='/';
            temp_path+=v[x];

			cout<<temp_path<<":"<<endl;
            if((dir_ptr= opendir(temp_path.c_str() ) ))
            {

                tempv=read_filenames(temp_path);
                //display content

                if(temp_path.size()>0)
                {
                    recurdir3(temp_path,tempv,mode);
                }

            }

        }

    }

}

void print_dir_both(string path, vector<string>vx, int mode)
{
	unsigned int x, l;
	unsigned long blocks = 0;
	string output;
	char filelenght_fmt[16];
	char link_fmt[16];
	vector<string>v_sub;
	bool Free_fmt = false; //true 2 spaces between filename
	unsigned int maxLink=1;
	int wordsPerLine = 2;
	int ScreenWidth=80;
	int wordcount=0;
	for(x=0;x<vx.size();x++)
	{
		v.push_back(vx[x]);
	}
	if((mode & DIR_LONG) != DIR_LONG)	//while not -l
	{
		struct winsize w;
		ioctl(0,TIOCGWINSZ, &w);
		if(w.ws_col>0) ScreenWidth=w.ws_col;
		for(x=0;x<v.size();x++)
		{
			if(v[x].length > maxWlen) maxWlen=v[x].legth();
		}
		maxWlen+=2;
		wordsPerLine=ScreenWidth/maxWlen;
		if(v.size()<9)	//only 8 filenames case
		{
			unsigned int len = 0;
			for(x=0; x<v.size();x++) len+=(v[x].length()+2)
			{
				if(len<=ScreenWidth)
				{
					Free_fmt=true;
					wordsPerLine=8;
				}
			}
		}
	}
}

void print_ALL(string &path, vector<string> &v, int mode)
{
	if((mode & DIR_ALL) && (mode & DIR_LONG))
	{
		//print  -al
		for(unsigned int x = 0; x <v.size();x++)
        {
            string output= make_string(path, v[x], output, mode);
            cout<<output<<endl;
        }

	}
	
	else if(mode & DIR_ALL)
	{
		//-a
		for(unsigned int i = 0; i<v.size();i++)
		{
			string output =  make_string(path, v[i], output, mode);
			cout<< output << " ";
		} 
		cout<<endl;
	}

	else if(mode & DIR_LONG)
	{
		//-l
		for(unsigned int i = 0; i<v.size();i++)
		{
			if(v[i][0]!='.')
			{
				string output =  make_string(path, v[i], output, mode);
				cout<< output <<endl;
			}
		}
		
	}
	
	else	//default
	{
		for(unsigned int i = 0; i <v.size();i++)
		{
			if(v[i][0]!='.')
			{
				string output =  make_string(path, v[i], output, mode);
				cout<< output << " ";
			}
		}
		cout<<endl;
	}


}

void ls_cmd(char** argv)
{
	int x = 1;
	int mode = 0;
	string path = ".";
	vector<string> v;


	for(;argv[x]!=NULL ;x++ )
	{

		//cout<<"ls_cmd1  was called"<<endl;
		if(argv[x][0]!='-' && x!=0)
		{
			//cout<<"argv[x]="<<argv[x]<<endl;
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
		}
	}
	//push parameters into ls helper  read function
	v = read_filenames(path);
	if(mode & DIR_R)
		recurdir3(path,v,mode);
	else
		print_ALL(path,v,mode);	
}


#endif // LS_CMD_H__


