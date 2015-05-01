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
   if( left.size() > right.size() )
      {
		  return true;
	  }
   return false;
}

void print_ALL(string &path, vector<string> &v, int mode);

string make_string(string &path, string &fname, string &output, int &mode, char* link_fmt, char* filelength_fmt)
{
    struct stat statBuf;
	char fstatus[16];
	char tmp[32];
	struct passwd *pwd;
	struct group *grp;
	struct tm *tm;
	char datestring[256];
	char sperm[16];
	string result="";
	char tmp[32];


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
		sprintf(tmp,link_fmt, statBuf.st_nlink);
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
	sprintf(datestring, "%s%s%s ", color_fmt, fname.c_str(), default_fmt);
	output += tmp;
	return output;
}

void print_dir_both(string path, vector<string>vx, int mode)
{
	unsigned int x, l;
	unsigned long blocks = 0;
	string output;
	char filelenght_fmt[16];
	char link_fmt[16];
	vector<string>v_sub;
	vector<string>v_sub;

	//DIR_LONG mode
	long maxFilelength=0;
	unsigned int maxLink=1;

	//not DIR_LONG mode
	bool Free_fmt = false; //true 2 spaces between filename
	int wordsPerLine = 2;
	unsigned int linesPerDisplay=1;
	unsigned int maxWlen=1;
	int ScreenWidth=80;
	int wordcount=0;
	unsigned int maxWlenB[80]
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
		else if(v.size()<8)	//only 7 filenames case
		{
			unsigned int len = 0;
			for(x=0; x<v.size();x++) len+=(v[x].length()+2)
			{
				if(len<=ScreenWidth)
				{
					Free_fmt=true;
					wordsPerLine=7;
				}
			}
		}
		else if(v.size()<7)	//only 6 filenames case
		{
			unsigned int len = 0;
			for(x=0; x<v.size();x++) len+=(v[x].length()+2)
			{
				if(len<=ScreenWidth)
				{
					Free_fmt=true;
					wordsPerLine=6;
				}
			}
		}
		else if(v.size()<6)	//only 5 filenames case
		{
			unsigned int len = 0;
			for(x=0; x<v.size();x++) len+=(v[x].length()+2)
			{
				if(len<=ScreenWidth)
				{
					Free_fmt=true;
					wordsPerLine=5;
				}
			}
		}
		else if(v.size()<5)	//only 4 filenames case
		{
			unsigned int len = 0;
			for(x=0; x<v.size();x++) len+=(v[x].length()+2)
			{
				if(len<=ScreenWidth)
				{
					Free_fmt=true;
					wordsPerLine=4;
				}
			}
		}
		else if(v.size()<4)	//only 3 filenames case
		{
			unsigned int len = 0;
			for(x=0; x<v.size();x++) len+=(v[x].length()+2)
			{
				if(len<=ScreenWidth)
				{
					Free_fmt=true;
					wordsPerLine=3;
				}
			}
		}
		else if(v.size()<3)	//only 2 filenames case
		{
			unsigned int len = 0;
			for(x=0; x<v.size();x++) len+=(v[x].length()+2)
			{
				if(len<=ScreenWidth)
				{
					Free_fmt=true;
					wordsPerLine=2;
				}
			}
		}
		else if(v.size()<2)
        {
            Free_fmt=true;
            wordsPerLine=1;
        }
        if(Free_fmt==false)
        {
            for(wordsPerLine=vx.size();wordsPerLine!=1;wordsPerLine--)
            {
                unsigned bIndex=0;
                linesPerDisplay=xv.size()/wordsPerLine;
                if(linesPerDisplay*wordsPerLine < vx.size()) linesPerDisplay++;
                for(bIndex=0;bIndex<wordsPerLine;bIndex++)
                {
                    maxWlenB[bIndex]=0; //reset max=0
					unsigned int cnt=0;
					for(unsigned int index=bIndex*linesPerDisplay;(index<vx.size()&&cnt<linesPerDisplay);index++,cnt++)
					{
						if(maxWlenB[bIndex]<vx[index].length())
							maxWlenB[bIndex]=vx[index].length();
					}
					maxWlenB[bIndex]+=2;
                }
                unsigned int sum=0;
				for(unsigned int index=0; index<wordsPerLine; index++)
					sum+=maxWlenB[index];
                if(sum<=ScreenWidth)
                {
                   if(vx.size()==4 && wordsPerLine ==3)continue;
                   if(vx.size()==16 && wordsPerLine ==7 && linesPerDisplay==3)continue;
                   break;
                }
            }
            v.clear();

            for(l=0;l<linesPerDisplay;l++)
            {
                for(x=l;x<vx.size();x+=linesPerDisplay) v.push_back(vx[x]);
                cout<<"File size="<<vx.sixe()<< " WPL="<<wordsPerLine<<" LPD="<<linesPerDisplay<<endl;
            }
        }   ///not of NOT DIR_LONG mode

        string ppath;
        struct stat statbuf;
        string ppath;
		struct stat statbuf;
		maxFilelength=0;
		maxLink=1;
		long blksize=0;
		for(x=0;x<v.size();x++)
		{
			ppath=path;
			ppath+='/';
			ppath+=v[x];

			if (stat(ppath.c_str(), &statbuf) != -1)
			{
				if((v[x][0]!='.') || (mode & DIR_ALL))
				{
				if(statbuf.st_size>maxFilelength)
				maxFilelength=statbuf.st_size;
				if(statbuf.st_nlink>maxLink)maxLink=statbuf.st_nlink;
				blocks+=statbuf.st_blocks;
				blksize=statbuf.st_blksize;
				}
			}
		}
		unsigned long len=maxFilelength;
		for(x=1;len!=0;x++) len/=10;

		filelenght_fmt[0]='%';
		sprintf(&filelenght_fmt[1],"%dd",x);

		unsigned linklength=maxLink;
		for(x=0;linklength!=0;x++)
            linklength/=10;
        if(x==0)x=1;
        link_fmt[0]='%';
        sprintf(&link_fmt[1],"%dd",x);
        if(blksize==4096)
            blocks/=2;
        else if(blksize==4096*4)blocks=blocks*2;
        if(mode & DIR_LONG && v.size(>1))
        {
            cout<<"Total "<<blocks<<endl;
        }

        ///normal without Reverse mode
        wordcount=0;
        for(x=0;x<v.size();x++)
        {
            if(((mode & DIR_ALL)==DIR_ALL) && (mode & DIR_LONG))
                cout<<make_string(path,v[x],mode,link_fmt,filelength_fmt)<<endl;
            else if( ((mode & DIR_LONG)==DIR_LONG)&&(v[x][0]!='.') )
                cout<<make_string(path,v[x],mode,link_fmt,filelength_fmt)<<endl;
            else if( (mode & DIR_ALL)==DIR_ALL )
            {
                output = make_string(path,v[x],mode,link_fmt,filelenght_fmt);
                l=v[x].length();
                int len = (int)maxWlen[wordcount]-(int)l;
                cout<<output;
                if(wordcount+1 !=wordsPerLine)&&(Free_fmt==false))
                    for(int y=0;y<len;y++)cout<<" ";
				else if(Free_fmt==true)cout<<"  ";

				wordsCount++;
				if(wordsCount>=wordsPerLine)
				{
					wordsCount=0;
					cout<<endl;
				}
            }
            else if((v[x][0]!='.'))
            {
                output = make_string(path,v[x],mode,link_fmt,filelenght_fmt);
                l=v[x].length();
                int len = (int)maxWlen[wordcount]-(int)l;
                cout<<output;
                if(wordcount+1 !=wordsPerLine)&&(Free_fmt==false))
                    for(int y=0;y<len;y++)cout<<" ";
				else if(Free_fmt==true)cout<<"  ";

				wordsCount++;
				if(wordsCount>=wordsPerLine)
				{
					wordsCount=0;
					cout<<endl;
				}
            }
        }
        cout<<endl;

	}
}

void recurdir3(string &path, vector<string> &v, int mode)   ///print dir recursively
{
    DIR* dir_ptr;
	string temp_path;
	vector<string>tempv;
    print_dir_both(path,v,mode);   //print current Dir filenames
	for(unsigned int x=0;x<v.size();x++)
    {
        if(v[x]==".")continue;
        if(v[x]!="..")
        {
            temp_path=path;
            temp_path+='/';
            temp_path+=v[x];

			//cout<<temp_path<<":"<<endl;
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

void ls_cmd(char **argv)
{
	vector<string>v;
	vector<string>v_sub;
	vector<string>u_sub;
	string path=".";
	string ppath="";
	string tmp;

	int mode=0;
	int slen=0;
	v.clear();

	for(int x=0;argv[x]!=NULL;x++)
	{

		if((argv[x][0]!='-')&&(x!=0))
		{
			v.push_back(string(argv[x]));
		}
		if(argv[x][0]=='-')
		{
			slen=-1;
			while(argv[x][++slen]!='\0');


			for(int i=1;i<slen;i++)
			{
				char chr=argv[x][i];
				switch(chr)
				{
					case 'a':
						mode|=DIR_ALL;
						break;
					case 'l':
						mode|=DIR_LONG;
						break;

					case 'R':
						mode|=DIR_R;
						break;
					default:
						cerr<<"ls Error: argument option#"<<endl;
						return;
				}
			}
		}

	}


	v=read_directory(path,mode);

	if(v.size()==0)
		{
			v=read_directory(path,mode);
			if(mode & DIR_R)print_dir3(path,v,mode);
			else print_dir_both(path,v,mode);
		}
	else
	{
		u_sub.clear();
		for(unsigned int i=0;i<v.size();i++)
		{
			if(v[i][0]=='~')	//special case
            {
                tmp="/home/";
                tmp+=getlogin();
                if(v[i].length()>1)
                tmp+=&v[i][1];
                v[i]=tmp

            }
			ppath=path;
			ppath+='/';
			ppath=v[i];
			errno=0;
			v_sub=read_filenames(ppath,mode);

			if(v_sub.size()>0)
			{
				if(mode & DIR_R)print_dir3(ppath,v_sub,mode);
				else
				print_dir_both(ppath,v_sub,mode);

			}
			else
			{

				if(errno==2)
				{
					cerr<<"ls: CANNOT ACCESS "<< v[i]<< ": "<<strerror(errno)<<endl;
				}

				if(errno !=2)
				u_sub.push_back(v[i]);
			}
		}
		if(u_sub.size()>0)
		{
			if(mode & DIR_R)print_dir3(path,u_sub,mode);
			else print_dir_both(path,u_sub,mode);
		}
	}
	cout<<endl;


}


#endif // LS_CMD_H__


