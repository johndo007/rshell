# RShell

Rshell is a student made program that is meant to behave like a terminal. It is meant to run commands like other terminals and such.

##Installation
$ git clone http://github.com/aso001/rshell.git  
$ cd rshell  
$ git checkout hw3  
$ make  

##Launching
After installation:
$ bin/rshell

##Features
1.Shell (1.50)
* Memory allocation handled before and after execution  
* Prompts for username and hostname if avilable  
* `&&` will execute the next command, if the previous command executes.
* `||` will execute the next command, if the previous command fails to execute.
* `;` will always execute the next command.
* (1.38) Applied student made LS  
* (1.45) Handle redirection and piping  
++Added Error check messages  

2.Ls Command(1.38)  
* '-a': display the all contents of directory 
* '-l': lists out detailed info  
* '-R': does recursive calls  
* (1.45) Add support for file output when piping  

3.Redirection (1.50)  
* input redirection - understands '<<<' case   
* output redirection  - understands '2>' case  
* piping  

4.Signals (1.51)  
* cd  
* ^c signal

## Known Bugs, Issues, Limitations 
-Rshell cannot use 'cd' command  
-Input redirection require a specific order of 'cat<(file)'  
-Tabs are not treated as spaces and instead are treated as a character.  
-Arrow keys are treated as characters.They will not cycle through previous commands and while move the cursor through the consol.  
-'~' case for ls commands has been implemented but is a bit buggy. Alternative access method at the momement is using: '/home/csmajs/(yourusername)'   
-Clicking on terminal will cause the cause the current line to be overwritten by the ascii value of a mouse button click.  
