# RShell

Rshell is a student made program that is meant to behave like a terminal. It is meant to run commands like other terminals and such.

##Installation
$ git clone http://github.com/aso001/rshell.git  
$ cd rshell  
$ git checkout hw0  
$ make  

##Launching
After installation:
$ bin/rshell

##Features
1.Shell (1.00 - 1.31)
* Memory allocation handled before and after execution  
* Prompts for username and hostname if avilable  
* `&&` will execute the next command, if the previous command executes.
* `||` will execute the next command, if the previous command fails to execute.
* `;` will always execute the next command.
* (1.38) Applied student made LS  
* (1.45) Handle redirection and piping  
++Added Error check messages  

2.Ls Command(1.31-1.38)  
* '-a': display the all contents of directory 
* '-l': lists out detailed info  
* '-R': does recursive calls  
* (1.45) Add support for file output when piping  

3.Redirection (1.39)  
* input redirection  
* output redirection  
* piping  

## Known Bugs, Issues, Limitations 
-Rshell cannot use 'cd' command  
-Tabs are not treated as spaces and instead are treated as a character.  
-Arrow keys are treated as characters.They will not cycle through previous commands and while move the cursor through the consol.  
-File redirection('<' or '>' ) is not implemented  
-'~' case for ls commands has been implemented but is a bit buggy. Alternative access method at the momement is using: '/home/csmajs/(yourusername)'   
-Clicking on terminal will cause the cause the current line to be overwritten by the ascii value of a mouse button click.  
