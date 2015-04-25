#ifndef LS_CMD_H__
#define LS_CMD_H__

//defining bit values
#define DIR_ALL  1	//-a
#define DIR_LONG 2	//-l
#define DIR_R	 4  //-R

vector<string> read_filenames(string &path)
{
	DIR*  dir_ptr = opendir(path.c_str());
	vector<string> filenames;
	dirent* dir_en;
	filenames.clear();
	if(dir_ptr)
	{
		while(true)
		{
			errno = 0;
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

void print_ALL(string &path, vector<string> &path, int &mode)
{
	if((mode & DIR_LONG) && (mode & DIR_R))
	{
		//print -alR
	}
	else if(mode & DIR_LONG)
	{
		//print  -al
	}
	else if(mode & DIR_R)
	{
		//print -aR
	}
	else
	{
		//default print -a
		for(int i =0; i<v.size();i++)
		{
			cout<<v[i]<<endl;
		}
	}


}

void print_LONG(string &path, vector<string> &path, int &mode)
{

}

void print_R(string &path, vector<string> &path, int &mode)
{

}

void ls_cmd(char** argv)
{
	//cout<<"ls was called"<<endl;
	//assume ls was called
	//DIR *mydir;
	int x = 0;
	int mode = 0;
	string path = ".";
	vector<string> v;
	//vector<bool> flag;
	//for(int i = 0; i < 3;i++)  flag.push_back(false);
	for(;argv[x]!=NULL ;x++ )
	{
		if(argv[x][0]!='-' && x!=0) path = argv[x];
		//first part of parameter uses '-'
		if(argv[x][0]=='-')
		{
			//-a -l -R
			for(int y = 1; argv[x][y]!='\0'; y++)
			{
				char input = argv[x][y];
				switch(input)
				{
					case 'a':
						mode |= DIR_ALL
						break;
					case 'l':
						mode |= DIR_LONG
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


