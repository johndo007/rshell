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

bool stringCompare( const string &left, const string &right ){
   for( string::const_iterator lit = left.begin(), rit = right.begin(); lit != left.end() && rit != right.end(); ++lit, ++rit )
      if( tolower( *lit ) < tolower( *rit ) )
         return true;
      else if( tolower( *lit ) > tolower( *rit ) )
         return false;
   if( left.size() < right.size() ) // <
      return true;
   return false;
}

vector <string> read_directory( const string& path,int mode )
{
  vector <string> filenames;
  dirent* dir_en;
  DIR* dir_ptr;
  errno = 0;
  dir_ptr = opendir( path.empty() ? "." : path.c_str() );
  if (dir_ptr)
    {
    while (true)
      {
      errno = 0;
      dir_en = readdir( dir_ptr );
      if (dir_en == NULL) break;
      if((dir_en->d_name[0]!='.') || (mode & DIR_ALL))
      filenames.push_back( std::string( dir_en->d_name ) );
      //cout<<"F="<<dir_en->d_name<<"  ";
      }
    closedir( dir_ptr );
    sort( filenames.begin(), filenames.end(),stringCompare );
    }
  return filenames;
  }

string make_string(string& path,string fname,int mode,char* link_fmt,char* filelength_fmt)
{
	//struct dirent  *dp;
	struct stat     statbuf;
	struct passwd  *pwd;
	struct group   *grp;
	struct tm      *tm;
	char            datestring[256];
	char   sperm[16];
	string result="";
	char  tmp[32];
	// Get entry's information.
	string ppath;
	ppath=path;
	ppath+='/';
	ppath+=fname;

    if (stat(ppath.c_str(), &statbuf) == -1)
        return result;
	if((mode & DIR_LONG)==DIR_LONG)
	{
    // Print out type, permissions, and number of links.
    get_perms(statbuf.st_mode,sperm);

    result+=sperm;

    sprintf(tmp,link_fmt, statbuf.st_nlink);
	result+=tmp;

    // Print out owner's name if it is found using getpwuid().
    if ((pwd = getpwuid(statbuf.st_uid)) != NULL)
        sprintf(tmp," %-s", pwd->pw_name);
    else
        sprintf(tmp," %-d", statbuf.st_uid);
	result+=tmp;

    // Print out group name if it is found using getgrgid().
    if ((grp = getgrgid(statbuf.st_gid)) != NULL)
        sprintf(tmp," %-s", grp->gr_name);
    else
        sprintf(tmp," %-d", statbuf.st_gid);
	result+=tmp;

    // Print size of file.
    sprintf(tmp,filelength_fmt, (long int)statbuf.st_size); //intmax_t
	result+=tmp;

    tm = localtime(&statbuf.st_mtime);
	const char fmt[]="%b %d %H:%M"; //Month Day Hour:Minute
    // Get localized date string.
    strftime(datestring, sizeof(datestring), fmt,tm); //nl_langinfo(D_T_FMT), tm);
    sprintf(tmp," %s ", datestring);  //, fname.c_str());
    result+=tmp;
	}
	//color the filename
	char color_fmt[32];
	color_fmt[0]=0;
	char default_fmt[]="\x1b[39;49m";
	if(fname.at(0)=='.')
		sprintf(color_fmt,"%s","\x1b[34;47m");
	else
		get_color_code(statbuf.st_mode,color_fmt);

	sprintf(tmp,"%s%s%s", color_fmt,fname.c_str(),default_fmt);
	result+=tmp;

	return result;
}

void print_dir_both(string path,vector<string>vx, int mode)	//screen case
{
	unsigned int x,l;
	unsigned long blocks=0;
	string output;
	char filelength_fmt[16];
	char link_fmt[16];
	vector<string>v;
	vector<string>v_sub;

	//DIR_LONG mode
	long maxFilelength=0;
	unsigned int maxLink=1;

	//not DIR_LONG mode
	bool Free_fmt=false;  //true: 2 spaces between filenames
	int wordsPerLine=2;
	unsigned int linesPerDisplay=1;
	unsigned int maxWlen=1;
	int ScreenWidth=80;	//assume default 80 spaces available per line
	int wordsCount=0;
	unsigned int maxWlenB[80];	//max word length for each word per column

	for(x=0;x<vx.size();x++)	//for DIR_LONG operation
	v.push_back(vx[x]);

	if((mode&DIR_LONG)!=DIR_LONG)	//while not -l
	{
		//get the current terminal windows size
		struct winsize w;
		ioctl(0, TIOCGWINSZ, &w);

		if(w.ws_col>0)ScreenWidth=w.ws_col;
		//
		for(x=0;x<v.size();x++)
		if(v[x].length() >maxWlen)maxWlen=v[x].length();
		maxWlen+=2;
		wordsPerLine=ScreenWidth/maxWlen;
		if(v.size()<9) //only 8 filenames
		{
			unsigned int len=0;
			for(x=0;x<v.size();x++)len+=(v[x].length()+2);
			if(len<=ScreenWidth){Free_fmt=true; wordsPerLine=8;}

		}else if(v.size()<8) //only 7 filenames
		{
			unsigned int len=0;
			for(x=0;x<v.size();x++)len+=(v[x].length()+2);
			if(len<=ScreenWidth){Free_fmt=true; wordsPerLine=7;}

		}else if(v.size()<7) //only 6 filenames
		{
			unsigned int len=0;
			for(x=0;x<v.size();x++)len+=(v[x].length()+2);
			if(len<=ScreenWidth){Free_fmt=true; wordsPerLine=6;}

		}else if(v.size()<6) //only 5 filenames
		{
			unsigned int len=0;
			for(x=0;x<v.size();x++)len+=(v[x].length()+2);
			if(len<=ScreenWidth){Free_fmt=true; wordsPerLine=5;}

		}else if(v.size()<5) //only 4 filenames
		{
			unsigned int len=0;
			for(x=0;x<v.size();x++)len+=(v[x].length()+2);
			if(len<=ScreenWidth){Free_fmt=true; wordsPerLine=4;}

		}else if(v.size()<4) //only 3 filenames
		{
			unsigned int len=0;
			for(x=0;x<v.size();x++)len+=(v[x].length()+2);
			if(len<=ScreenWidth){Free_fmt=true; wordsPerLine=3;}

		}else if(v.size()<3) //only 2 filenames
		{
			unsigned int len=0;
			for(x=0;x<v.size();x++)len+=(v[x].length()+2);
			if(len<=ScreenWidth){Free_fmt=true; wordsPerLine=2;}

		}else if(v.size()<2) //only 1 filenames
		{
			Free_fmt=true; wordsPerLine=1;

		}
		if(Free_fmt==false)
		{
			for(wordsPerLine=vx.size();wordsPerLine!=1;wordsPerLine--)
			{
				unsigned int bIndex=0;
				linesPerDisplay=vx.size()/wordsPerLine;
				if( linesPerDisplay*wordsPerLine < vx.size())linesPerDisplay++;
				for(bIndex=0;bIndex<wordsPerLine;bIndex++)
				{
					maxWlenB[bIndex]=0; //reset max=0
					unsigned int cnt=0;
					for(unsigned int index=bIndex*linesPerDisplay;(index<vx.size()&&cnt<linesPerDisplay);index++,cnt++)
					{
						if(maxWlenB[bIndex]<vx[index].length())
							maxWlenB[bIndex]=vx[index].length();
					}
					maxWlenB[bIndex]+=2; //2 spaces between filenames
				}
				unsigned int sum=0;
				for(unsigned int index=0; index<wordsPerLine; index++)
					sum+=maxWlenB[index];
				if(sum<=ScreenWidth)break;

			}
			v.clear();

			for(l=0;l<linesPerDisplay;l++)
				 for(x=l;x<vx.size();x+=linesPerDisplay) v.push_back(vx[x]);
			cout<<"File size="<<vx.size()<<"  WPL="<<wordsPerLine<<" LPD="<<linesPerDisplay<<endl;
		}


	}  //end of Not DIR_LONG mode

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
		for(x=1;len!=0;x++)
			len/=10;

		filelength_fmt[0]='%';
		sprintf(&filelength_fmt[1],"%dd",x);


		unsigned linklength=maxLink;
		for(x=0;linklength!=0;x++)
			linklength/=10;
		if(x==0)x=1;
		link_fmt[0]='%';
		sprintf(&link_fmt[1],"%dd",x);
		if(blksize==4096)
			blocks/=2;
		else if(blksize==4096*4)blocks=blocks*2;
		if(mode & DIR_LONG && v.size()>1)
		{
		cout<<"total "<<blocks<<endl;
		}



	//normal without REVERSE mode
		wordsCount=0;
		for(x=0;x<v.size();x++)
		{
		if(((mode & DIR_ALL)==DIR_ALL) && (mode & DIR_LONG))
		cout<<make_string(path,v[x],mode,link_fmt,filelength_fmt)<<endl;
		else if( ((mode & DIR_LONG)==DIR_LONG)&&(v[x][0]!='.') )
		cout<<make_string(path,v[x],mode,link_fmt,filelength_fmt)<<endl;
		else if( (mode & DIR_ALL)==DIR_ALL )
			{

				output=make_string(path,v[x],mode,link_fmt,filelength_fmt);
				l=v[x].length();
				int len=(int)maxWlenB[wordsCount]-(int)l;
				cout<<output;
				if((wordsCount+1 !=wordsPerLine) && (Free_fmt==false))  //last word before output endl
				for(int y=0;y<len;y++)cout<<" ";
				else if(Free_fmt==true)cout<<"  ";

				wordsCount++;
				if(wordsCount>=wordsPerLine)
				{
					wordsCount=0;
					cout<<endl;
				}
			}
		else if( (v[x][0]!='.'))
			{
				output=make_string(path,v[x],mode,link_fmt,filelength_fmt);
				l=v[x].length();
				int len=(int)maxWlenB[wordsCount]-(int)l;
				cout<<output;
				if((wordsCount+1 !=wordsPerLine)&& (Free_fmt==false))  //last word before output endl
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

//Printing a directory recursively
void print_dir3(string path,vector <string>v,int mode)
{
	DIR* dir_ptr;

	string ppath;
	vector<string>v_sub;
	print_dir_both(path,v,mode);
	for(unsigned int x=0;x<v.size();x++)
			{
				if(v[x]==".")continue;
				if((v[x][0]=='.')&&((mode&DIR_ALL)!=DIR_ALL))continue;	//prevent endless loop
				if(v[x]!="..")
				{
					ppath=path;
					ppath+='/';
					ppath+=v[x];

					if((dir_ptr=opendir(ppath.c_str()))!=NULL)
					{

						v_sub=read_directory(ppath,mode);
						if(v_sub.size()>0)	//not an empty path case
						{

							cout<<endl<<ppath<<":"<<endl;
							print_dir3(ppath,v_sub,mode);

						}

					}

				}

			}

}
void ls_cmd(char **argv)
{
	//DIR* dir_ptr;
	vector<string>v;
	vector<string>v_sub;
	vector<string>u_sub;
	string path=".";	//default current directory
	string ppath="";
	string tmp;
	int mode=0;
	int slen=0;
	v.clear();	//clear filenames array
	for(int x=0;argv[x]!=NULL;x++)
	{
		//cout<<"Argv:"<<x<<" "<<argv[x]<<endl;
		if((argv[x][0]!='-')&&(x!=0))	//prevent option or cmd become filename/path
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
					case '#':
					default:
						cerr<<"ls Error: argument option#"<<endl;
						return;
				}
			}
		}

	}

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
						//home


						tmp="/home/";
						tmp+=getlogin();   //hostname;
						if(v[i].length()>1)
						tmp+=&v[i][1];
						v[i]=tmp;
						//cout<<"HOME="<<v[i]<<endl;


					}
			ppath=path;
			ppath+='/';
			//ppath+= v[i];
			ppath=v[i];
			errno=0;
			v_sub=read_directory(ppath,mode);
			if(v_sub.size()>0)
			{
				//cout<<ppath<<":"<<endl;
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


