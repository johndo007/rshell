# RShell

Rshell is a student made program that is meant to behave like a terminal. It is meant to run commands like other terminals and such.

##Installation  
$ git clone http://github.om/aso001/rshell.git  
$ cd rshell  
$ git checkout hw0 
$ make  

##Launching  
After installation:  
$ bin/rshell 

##Features
1.Prompts for username and hostname if avialble (1.08)  
- user and host are displayed if available  
- displays '$' and waits for user command and enter

2.Can take in user command input and breaks it up into vectors(1.13)  (Updated 4/13/2015)  
- `&&` will execute the next command, if the previous command executes.

- `||` will execute the next command, if the previous command fails to execute.

- `;` will always execute the next command.  

3.Execute the commands (1.15)  
- Memory allocation handled before and after execution  

## Known Bugs, Issues, Limitations  
-Rshell cannot use 'cd' command  
-Tabs are not treated as spaces and instead are treated as a character.  
-Arrow keys are treated as characters and will not cycle through previous commands.  
-File redirection('<' or '>' ) is not implemented
