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
#include <errno.h>

using namespace std;

void get_perms(mode_t mode, char pbuff[])
{
char ftype = '?';
if (S_ISREG(mode)) ftype = '-';
if (S_ISLNK(mode)) ftype = 'l';
if (S_ISDIR(mode)) ftype = 'd';
if (S_ISBLK(mode)) ftype = 'b';
if (S_ISCHR(mode)) ftype = 'c';
if (S_ISFIFO(mode)) ftype = '|';
//if (S_ISINDEX(mode)) ftype = 'i';
sprintf(pbuff, "%c%c%c%c%c%c%c%c%c%c ",
ftype,
mode & S_IRUSR ? 'r' : '-',
mode & S_IWUSR ? 'w' : '-',
mode & S_IXUSR ? 'x' : '-',
mode & S_IRGRP ? 'r' : '-',
mode & S_IWGRP ? 'w' : '-',
mode & S_IXGRP ? 'x' : '-',
mode & S_IROTH ? 'r' : '-',
mode & S_IWOTH ? 'w' : '-',
mode & S_IXOTH ? 'x' : '-');
//mode & S_ISUID ? 'U' : '-',
//mode & S_ISGID ? 'G' : '-',
//mode & S_ISVTX ? 'S' : '-');

} 

#define DIR_ALL		1	//display all hidden/or not files
#define DIR_LONG 	2	//display the detail permissions,pwd,user,filesize,Date,Time and filename
#define DIR_R		4	//display all the sub-directories
#define DIR_REVERSE	8	//display in reverse order
#define DIR_ONCE	16	//display directory once
#define DIR_FP		32	//display to File or Pipe

void get_color_code(mode_t st_mode,char *color_fmt)
{
	if (S_ISDIR(st_mode))
	{
		//Directory=Blue=34
		sprintf(color_fmt,"\x1b[34m");
	}else if(st_mode & S_IXUSR )
	{
		//EXE=>GREEN=32
		sprintf(color_fmt,"\x1b[32m");
	}
}



// read_directory()
//   Return a vector of filename entries in a given directory given by path.
//   If no path is specified, the current working directory is used.
//

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
  bool required=false;
  if((dir_ptr = opendir( path.empty() ? "." : path.c_str() ))==NULL)if(required)perror("Error:opendir");;
  if (dir_ptr)
    {
    while (true)
      {
      errno = 0;
      if((dir_en = readdir( dir_ptr ))==NULL)if(required)perror("Error:readdir");
      if (dir_en == NULL) break;
      if((dir_en->d_name[0]!='.') || (mode & DIR_ALL))
      filenames.push_back( std::string( dir_en->d_name ) );
      //cout<<"F="<<dir_en->d_name<<"  ";
      }
    if(closedir( dir_ptr )<0)perror("Error:closedir");
    sort( filenames.begin(), filenames.end(),stringCompare );
    //cout<<endl;
    //for(int i=0;i<filenames.size();i++)
		//cout<<filenames[i]<<" ";
		//cout<<endl;
    
    }
    else
    {
	//EACCES Permission denied.
	//EBADF  fd is not a valid file descriptor opened for reading.
	//EMFILE Too many file descriptors in use by process.
	//ENFILE Too many files are currently open in the system.
	//ENOENT Directory does not exist, or name is an empty string.
	//ENOMEM Insufficient memory to complete the operation.
	//ENOTDIR name is not a directory.	
	//perror("Error: opendir()\n");
	//cout<<"Path="<<path<<endl;	
	}
    
  return filenames;
  }

string make_string(string& path,string fname,int mode,char* link_fmt,char* filelength_fmt,char* usr_fmt,char* grp_fmt)
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
	bool required=false;
    if (stat(ppath.c_str(), &statbuf) == -1){if(required)perror("Error:stat");  return result;}
    
	if((mode & DIR_LONG)==DIR_LONG)
	{
    // Print out type, permissions, and number of links.
    get_perms(statbuf.st_mode,sperm); 
    
    result+=sperm;
   
    sprintf(tmp,link_fmt, statbuf.st_nlink);
	result+=tmp;

    // Print out owner's name if it is found using getpwuid(). 
    bool ok=false;
    if ((pwd = getpwuid(statbuf.st_uid)) == NULL){if(required)perror("Error:getpwuid");}
    else
    {
        sprintf(tmp,usr_fmt, pwd->pw_name);  //" %-s"
        ok=true;
	}
    if(ok==false)
        sprintf(tmp," %-d", statbuf.st_uid);
	result+=tmp;

    // Print out group name if it is found using getgrgid(). 
    ok=false;
    if ((grp = getgrgid(statbuf.st_gid)) == NULL){if(required)perror("Error:getgrgid");}
    else
    {
        sprintf(tmp,grp_fmt, grp->gr_name);
        ok=true;
	}
    if(ok==false)
        sprintf(tmp," %-d", statbuf.st_gid);
	result+=tmp;

    // Print size of file. 
    sprintf(tmp,filelength_fmt, (long int)statbuf.st_size); //intmax_t
	result+=tmp;

    tm = localtime(&statbuf.st_mtime);
	//const char fmt[]="%A, %B %d %Y. The time is %X"; 
	//strftime(tmdescr, sizeof(tmdescr)-1, fmt, &now)
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
		
	if((mode&DIR_FP)!=DIR_FP)
	sprintf(tmp,"%s%s%s", color_fmt,fname.c_str(),default_fmt);
	else sprintf(tmp,"%s",fname.c_str());	//DIR_FP:NO color
	result+=tmp;

	return result;
}

void form_screen(unsigned int *wpl,unsigned int *lpd, vector<string>& vx,unsigned int maxWlenB[],unsigned int ScreenWidth )
{
			unsigned int bIndex=0;
			unsigned int wordsPerLine;
			unsigned int linesPerDisplay;
			unsigned int s=vx.size();
			//unsigned maxWlenB[80];
			//unsigned int ScreenWidth=80;
			
			if(s>128)
			{
				
				s=128;
			}
			
			for(wordsPerLine=s;wordsPerLine!=0;wordsPerLine--)
			{
				bIndex=0;
				linesPerDisplay=s/wordsPerLine;
				if( linesPerDisplay*wordsPerLine < s)linesPerDisplay++;
				for(bIndex=0;bIndex<wordsPerLine;bIndex++)
				{
					maxWlenB[bIndex]=0; //reset max=0
					unsigned int cnt=0;
					for(unsigned int index=bIndex*linesPerDisplay;((index<vx.size())&&(cnt<linesPerDisplay));index++,cnt++)
					{
						if(maxWlenB[bIndex]<vx[index].length())
							maxWlenB[bIndex]=vx[index].length();
				
					}
					maxWlenB[bIndex]+=2; //2 spaces between filenames
				}
				unsigned int sum=0;
				for(unsigned int index=0; index<wordsPerLine; index++)
					sum+=maxWlenB[index];
				if(sum<=ScreenWidth)
				{
					if(s==4 && wordsPerLine==3)continue;
					if(s==16 && wordsPerLine==7 && linesPerDisplay==3)continue;
					*wpl=wordsPerLine;
					*lpd=linesPerDisplay;
					return;  //exit if screen format is found properly!
				}
				
			}

			for(unsigned int l=0;l<s;l++)maxWlenB[l]=vx[l].length();
			*wpl=wordsPerLine=1;
			*lpd=linesPerDisplay=s;
			return;  //exit 
}
void print_dir_both(string path,vector<string>vx, int mode)	//screen case
{
	unsigned int x,l;
	unsigned long blocks=0;
	string output;
	char filelength_fmt[16];
	char link_fmt[16];
	char usr_fmt[16];
	char grp_fmt[16];
	vector<string>v;
	//vector<string>v_sub;
	//bool displayFlag[128][128];
	if(vx.size()==0)return;
	
	//DIR_LONG mode
	long maxFilelength=0;
	unsigned int maxLink=1;
	
	//not DIR_LONG mode
	bool Free_fmt=false;  //true: 2 spaces between filenames
	unsigned int wordsPerLine=2;
	unsigned int linesPerDisplay=1;
	//unsigned int maxWlen=1;
	unsigned int ScreenWidth=80; //assume default 80 spaces available per line
	unsigned int wordsCount=0;
	unsigned int maxWlenB[128];	//max word length for each word per column
	unsigned int row=0,col=0;
	
	
	
	if((mode&DIR_LONG)!=DIR_LONG)	//while not -l
	{
		if((mode&DIR_FP)!=DIR_FP)
		{
			//get the current terminal windows size
			struct winsize w;
			if(ioctl(0, TIOCGWINSZ, &w)<0)perror("Error:ioctl");

			//printf ("lines %d\n", w.ws_row);
			//printf ("columns %d\n", w.ws_col);
			if(w.ws_col>0)ScreenWidth=w.ws_col;
			//
			
			if(Free_fmt==false)
			{
				
				form_screen(&wordsPerLine,&linesPerDisplay, vx,maxWlenB,ScreenWidth);
			
				col=0;row=0;
				
				for(l=0,row=0;l<linesPerDisplay;l++,row++)
				{
					 for(x=l,col=0;x<vx.size();x+=linesPerDisplay,col++) 
						{
							
							v.push_back(vx[x]);
							
						}
					if(col<wordsPerLine)v.push_back("\n");
				}
				
			
			} //end of Free_fmt==false
		}else
		{
			for(x=0;x<vx.size();x++)	//for DIR_FP operation
			v.push_back(vx[x]);
		}	//end of if DIR_FP
				
	}  //end of if !DIR_LONG mode
	else
	{
		for(x=0;x<vx.size();x++)	//for DIR_LONG operation
		v.push_back(vx[x]);
		string ppath;
		struct stat statbuf;
		struct passwd  *pwd;
		struct group   *grp;
		maxFilelength=0;
		maxLink=1;
		long blksize=0;
		size_t usr_len=1;
		size_t grp_len=1;
		for(x=0;x<v.size();x++)
		{
			ppath=path;
			ppath+='/';
			ppath+=v[x];
		
			if (stat(ppath.c_str(), &statbuf) == -1)perror("Error:stat");
			else
			{
				char  tmp[32];
				if((v[x][0]!='.') || (mode & DIR_ALL))
				{
				if(statbuf.st_size>maxFilelength)
				maxFilelength=statbuf.st_size;
				
				if(statbuf.st_nlink>maxLink)maxLink=statbuf.st_nlink;
				
				blocks+=statbuf.st_blocks;
				blksize=statbuf.st_blksize;
				bool ok=false;
				if ((pwd = getpwuid(statbuf.st_uid)) == NULL)perror("Error:getpwuid");
				else
				{
					sprintf(tmp,"%s", pwd->pw_name);
					ok=true;
				}
				if(ok==false)
					sprintf(tmp,"%d", statbuf.st_uid);
				if(usr_len< strlen(tmp))usr_len=strlen(tmp);

				// Print out group name if it is found using getgrgid(). 
				ok=false;
				if ((grp = getgrgid(statbuf.st_gid)) == NULL)perror("Error:getgrgid");
				else
				{
					sprintf(tmp,"%s", grp->gr_name);
					ok=true;
				}
				if(ok==false)
					sprintf(tmp,"%d", statbuf.st_gid);
				if(grp_len< strlen(tmp))grp_len=strlen(tmp);
				}
			}
		}
		unsigned long len=maxFilelength;
		for(x=1;len!=0;x++)
			len/=10;
		
		//cout<<"maxFl="<<maxFilelength<< "   digit="<<x<<endl;
		filelength_fmt[0]='%';
		sprintf(&filelength_fmt[1],"%dd",x);
		//cout<<"Fmt="<<filelength_fmt<<endl;
		
		unsigned linklength=maxLink;
		for(x=0;linklength!=0;x++)
			linklength/=10;
		if(x==0)x=1;	
		//cout<<"maxLl="<<maxLink<< "  digit="<<x<<endl;
		link_fmt[0]='%';
		sprintf(&link_fmt[1],"%dd",x);
		//cout<<"Link Fmt="<<link_fmt<<endl;
		//cout<<"blksize="<<blksize<<endl;
		if(blksize==4096)
			blocks/=2;
		else if(blksize==4096*4)blocks=blocks*2;
		
		usr_fmt[0]=' ';
		usr_fmt[1]='%';
		sprintf(&usr_fmt[2],"-%ds",(int)usr_len);
		
		grp_fmt[0]=' ';
		grp_fmt[1]='%';
		sprintf(&grp_fmt[2],"-%ds",(int)grp_len);
		
		if(mode & DIR_LONG && v.size()>1)
		{
		//cout<<path<<":"<<endl;
		cout<<"total "<<blocks<<endl;
		}
		
	}
	
	//normal without REVERSE mode
		wordsCount=0;
		if(mode & DIR_LONG)
		{
			for(x=0;x<v.size();x++)
				cout<<make_string(path,v[x],mode,link_fmt,filelength_fmt,usr_fmt,grp_fmt)<<endl;
			
		}
		else
		{
			
			
				wordsCount=0;
				
				row=0;  col=0;
				for(x=0;x<v.size();x++)
				{
					if((mode&DIR_FP)!=DIR_FP)
					{
						if(v[x][0]=='\n')
						{
						
							wordsCount=0;
							cout<<endl;
							x++;
							if(x>=v.size())break;
						}
						output=make_string(path,v[x],mode,link_fmt,filelength_fmt,usr_fmt,grp_fmt);
						//output=make_xstring(path,v[x],mode);
						l=v[x].length();
						int len=(int)maxWlenB[wordsCount]-(int)l;
						cout<<output;
						if((wordsCount+1 !=wordsPerLine))  //last word before output endl
						for(int y=0;y<len;y++)cout<<" ";
						
						
						wordsCount++;
						if(wordsCount>=wordsPerLine)
						{
							wordsCount=0;
							cout<<endl;
							
						}
					}else
					{	//DIR_FP mode
						if(v[x]!="\n")
						cout<<v[x]<<endl;
					}//end of if DIR_FP
					
				} //end of for-x-loop
				//cout<<endl;
				
			
		} //end of if mode&DIR_LONG
	
	
}


void print_dir3(string path,vector <string>v,int mode)
{
	
	print_dir_both(path,v,mode);
	
	for(unsigned int x=0;x<v.size();x++)
			{
				if(v[x]==".")continue;
				if((v[x][0]=='.')&&((mode&DIR_ALL)!=DIR_ALL))continue;	//prevent endless loop
				if(v[x]!="..")
				{
					DIR* dir_ptr;
					string ppath;
					ppath=path;
					ppath+='/';
					ppath+=v[x];
					bool display_required=false;
						
					if((dir_ptr=opendir(ppath.c_str()))==NULL)
					{
						if(display_required)perror("Error:opendir");
					}
					else
					{
						vector<string>v_sub;			
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

void ls_cmd(char **argv,int fp)
{
	//DIR* dir_ptr;
	vector<string>v;
	vector<string>v_sub;
	vector<string>u_sub;
	string path=".";	//default current directory
	string ppath="";
	string tmp;
	//char username[128];
	int mode=0;
	int slen=0;
	//R_count=0;
	v.clear();	//clear filenames array
	if(fp>0)mode|=DIR_FP;
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
	/*
	//cout<<"Path="<<path<<endl;
	v=read_directory(path,mode);
	cout<<"Directory Item's Count="<<v.size()<<endl;
	
	cout<<"Files="<<endl;
	for(int i=0;i<v.size();i++)
	cout<<v[i]<<"  ";
	cout<<endl;
	*/
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
						tmp+=getlogin();
						if(errno<0)perror("getlogin");   //hostname;
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
	//if(mode&DIR_R)cout<<"Recursive count="<<R_count<<endl;
	
	
}


#endif // LS_CMD_H__


