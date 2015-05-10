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
1.Prompts for username and hostname if avilable (1.00 - 1.31)
* user and host are displayed if available
* displays '$' and waits for user command and enter

2.Can take in user command input and breaks it up into vectors(1.13)  (Updated 4/13/2015)
* `&&` will execute the next command, if the previous command executes.

* `||` will execute the next command, if the previous command fails to execute.

* `;` will always execute the next command.

3.Shell (1.00 - 1.31)
* Memory allocation handled before and after execution  
++Added Error check messages  

4.Ls Command(1.31-1.38)  
* '-a': display the all contents of directory 
* '-l': lists out detailed info  
* '-R': does recursive calls  

5.Redirection (1.39)  
* input redirection  
* output redirection  
* piping  

## Known Bugs, Issues, Limitations 
-Rshell cannot use 'cd' command  
-Tabs are not treated as spaces and instead are treated as a character.  
-Arrow keys are treated as characters.They will not cycle through previous commands and while move the cursor through the consol.  
-File redirection('<' or '>' ) is not implemented  
-'~' case for ls commands has been implemented but is a bit buggy. Alternative access method at the momement is using: '/home/csmajs/(yourusername)'   

