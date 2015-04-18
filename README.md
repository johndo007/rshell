# RShell

Rshell is a student made program that is meant to behave like a terminal. It is meant to run commands like other terminals and such.

##Features
1.Prompts for username and hostname if avialble (1.08)  
- user and host are displayed if available  

2.Can take in user command input and breaks it up into vectors(1.13)  (Updated 4/13/2015)  
- `&&` will execute the next command, if the previous command executes.

- `||` will execute the next command, if the previous command fails to execute.

- `;` will always execute the next command.  

3.Execute the commands (1.11)

## Known Bugs and Issues  
-Rshell cannot use 'cd' command  
-Connector logic has issues if you try to execute an unknown command.
-Tabs are not treated as spaces and instead are treated as a character.

