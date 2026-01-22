/*
 Name : Nandeesh
 Dis  : Implementation of own mini shell project using system calls and signals and functions
*/

#include "main.h"
// global variables for inputstring,promot string
char *external[153];
char *commands[153];
int respid;
int info;
char promstr[100];
char ptr[20];
int main()
{
    // function to clear the therminal
    system("clear");

    strcpy(promstr, "User:~/minishell$ ");
    // function call to get external commands into array of pointers
    extract_external_commands(external);
    // signal handling for SIGINT,SIGTSTP
    signal(SIGINT, signal_handler);  // ctrl c
    signal(SIGTSTP, signal_handler); // ctrl z
    signal(SIGCHLD, signal_handler); // to clear the resources

    while (1) // SUPER LOOP
    {
        // print the promot message
        printf(ANSI_COLOR_GREEN "%s" RESET, promstr);
        bzero(ptr, 20);
        __fpurge(stdin);
        scanf("%[^\n]", ptr);
        if (strstr(ptr, "PS1=") != NULL)
        {

            if (strchr(ptr, ' ') != NULL)
            {
                printf("Error: Space not allowed\n");
            }
            else if (*(ptr + 4) == '\0')
            {
                printf("Error: No value after '='\n");
            }
            else
            {
                strcpy(promstr, (ptr + 4));
            }
        }
        // function call to convert input string to array of pointers
        stringconversiontoarray(ptr);
        if (ptr[0] == 0)
        {
            continue;
        }
        // function call to chech the command type
        int res = check_command_type(commands[0]);
        if (res == BUILTIN) // excute in internal commands using function or system calls
        {
            // function call to excute internal commands
            execute_internal_commands(commands[0]);
        }
        else if (res == EXTERNAL)
        {
            // create process
            respid = fork();
            if (respid > 0) // parent
            {
                // wait the parent process for child process
                waitpid(respid, &info, WUNTRACED);
                respid = 0;
            }
            else if (respid == 0) // child process
            {
                // signal handling for default action
                signal(SIGINT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
                // excute external commands
                 execute_external_commands(commands);
                 exit(0);
                
            }
            else
            {
                perror("fork");
            }
        }
        else if (res == NO_COMMAND)
        {
           // function call to excute another commands
            execute_no_commands(commands[0]);
        }
        else
        {
            printf("Invalid Commands\n");
        }
    }
}
