#include "main.h"
extern char *external[153];
extern char *commands[153];
static char *cmd_buffer = NULL;
extern int respid;
extern int info;
extern char promstr[100];
extern char ptr[20];
Slist *head = NULL;
// function defnition to convert file data into array of pointers
void extract_external_commands(char **external_commands)
{
    int fd = open("external.txt", O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        return;
    }

    char ch;
    char buffer[100];
    int index = 0;
    int i = 0;

    while (read(fd, &ch, 1) == 1)
    {
        // when we get \n then add null character and find the size and store to array of  pointers
        if (ch == '\n')
        {
            buffer[index] = '\0';

            external_commands[i] = malloc(index + 1);
            strcpy(external_commands[i], buffer);

            i++;
            index = 0;
        }
        else
        {
            buffer[index++] = ch;
        }
    }

    /* Handle last line if no trailing newline */
    if (index > 0)
    {
        buffer[index] = '\0';
        external_commands[i] = malloc(index + 1);
        strcpy(external_commands[i], buffer);
        i++;
    }

    external_commands[i] = NULL;
    close(fd);
}
// check the command type whether builtin,external
int check_command_type(char *command)
{
    char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
                        "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
                        "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", NULL};
    int i = 0;
    int j = 0;
    while (builtins[i])
    {
        if (strcmp(command, builtins[i]) == 0)
        {
            return BUILTIN;
        }
        i++;
    }
    while (external[j])
    {
        if (strcmp(command, external[j]) == 0)
        {
            return EXTERNAL;
        }
        j++;
    }
    return NO_COMMAND;
}
// execute internal commands
void execute_internal_commands(char *input_string)
{
    // function to excute exit command
    if (strcmp(input_string, "exit") == 0)
    {
        exit(0);
    }
    // to excute pwd command
    else if (strcmp(input_string, "pwd") == 0)
    {
        char buffer[100];
        getcwd(buffer, sizeof(buffer));
        printf("%s\n", buffer);
    }
    // to excute cd command
    else if (strncmp(input_string, "cd", 3) == 0)
    {
        chdir(input_string + 3);
    }
    // to excute echo commands
    else if(strcmp(ptr,"echo $$")== 0)
    {
        printf("%d\n",getpid());
    }
    else if(strcmp(ptr,"echo $?")==0)
    {
        printf("%d\n",info);
    }
    else if(strcmp(ptr,"echo $SHELL")==0)
    {
        char result[230];
        char *ptr=getenv("SHELL");
        strcpy(result,ptr);
        printf("%s\n",result);

    }
}
// excute external commands
void execute_external_commands(char *commands[])
{

    // commandindex
    int argc = 0;
    int a = 0;
    while (commands[a])
    {
        argc++;
        a++;
    }
    int cmdindex[argc];
    int commandcount = 1;
    cmdindex[0] = 0;
    int k = 1;
    // to check whether the pipe present or not
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(commands[i], "|") == 0)
        {
            cmdindex[k++] = i + 1;
            commandcount++;
            
        }
    }
    /* Pipe cannot be first or last argument */
    if (strcmp(commands[0], "|") == 0 || strcmp(commands[argc - 1], "|") == 0)
    {
        printf("Error: pipe cannot be at start or end\n");
        return;
    }
    /* No empty command between pipes */
    for (int i = 0; i < argc - 1; i++)
    {
        if (strcmp(commands[i], "|") == 0 && strcmp(commands[i + 1], "|") == 0)
        {
            printf("Error: empty command between pipes\n");
            return;
        }
    }
    // backupof stdin and stdout
    int b1 = dup2(0, 50); // stdin
    int b2 = dup2(1, 60); // stdout
    // pipe for ipc
    int pipefd[2];
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(commands[i], "|") == 0)
        {
            commands[i] = NULL;
        }
    }
 // to excute signal command   
if(commandcount==1)
{
        execvp(commands[0],&commands[cmdindex[0]]);
        return;
}
else
{

    // to excute n pipe and n+1 process
    for (int i = 0; i < commandcount; i++)
    {

        if (i < commandcount - 1) // pipe creation until last command and validation
        {
            if (pipe(pipefd) == -1)
            {
                perror("pipe");
                return;
            }
        }
        int pid = fork(); // process creation
        if (pid > 0)      // parent
        {
            if (i < commandcount - 1)
            {
                dup2(pipefd[0], 0);
                close(pipefd[1]);
            }
        }
        else if (pid == 0) // child
        {
            // redirecting stdout until last command
            if (i < commandcount - 1)
            {
                dup2(pipefd[1], 1);
                // closing unusing end
                close(pipefd[0]);
            }

            // excution of command
            execvp(commands[cmdindex[i]], &commands[cmdindex[i]]);
            printf("Error in execvp\n");
        }
    }
    for (int i = 0; i < commandcount; i++)
        wait(NULL);
    // updating stdin and stdout
    dup2(b1, 0);
    dup2(b2, 1);
    close(b1);
    close(b2);
}
}
// function defnition to convert input string to array of pointers
void stringconversiontoarray(char *input_string)
{
    int i = 0;

    /* Free previous buffer if any */
    free(cmd_buffer);

    /* Allocate buffer */
    cmd_buffer = malloc(strlen(input_string) + 1);
    if (cmd_buffer == NULL)
        return;

    /* Copy input string */
    strcpy(cmd_buffer, input_string);

    /* Tokenize the copied string */
    char *token = strtok(cmd_buffer, " ");

    while (token != NULL && i < 152)
    {
        commands[i++] = token;
        token = strtok(NULL, " ");
    }

    /* NULL terminate argv-style array */
    commands[i] = NULL;
}
// own function handler for SIGINT and SIGTSTP
void signal_handler(int sig_num)
{

    if (sig_num == SIGINT)
    {
        if (respid == 0) // no cmd is present
        {
            printf(ANSI_COLOR_GREEN "\n%s" RESET, promstr);
            fflush(stdout);
        }
    }
    else if (sig_num == SIGTSTP)
    {
        if (respid == 0)
        {
            printf(ANSI_COLOR_GREEN "\n%s" RESET, promstr);
            fflush(stdout);
        }
        else if (respid > 0)
        {
            // linked list  insert at first
            insert_at_first(&head, respid);
        }
    }
    else if (sig_num == SIGCHLD)
    {
        waitpid(-1, &info, WNOHANG);
        // signal(SIGCHLD, SIG_DFL);
    }
}
int insert_at_first(Slist **head, int data)
{

    Slist *new = malloc(sizeof(Slist)); // create a new node
    if (new == NULL)
    {
        // checking node created or not
        return -1;
    }
    printf("Stopped : %s\t %d\n", ptr, respid);
    new->id = data; // fill the node with data and link
    strcpy(new->commandname, ptr);
    new->link = NULL;
    if (*head == NULL)
    {
        *head = new;
        // return 0;              // If the List is empty
    }
    else
    {
        // Slist *temp=*head;
        new->link = *head;
        *head = new; // if the list is not empty
                     // return 0;
    }
}
int sl_delete_first(Slist **head)
{
    if (*head == NULL)
    {

        return -1;
    }
    Slist *temp = *head;
    *head = temp->link;
    free(temp);
    printf("Deleted from list\n");
    return 0;
}
void print_list(Slist *head)
{
    int i = 1;
    if (head == NULL)
    {
        // printf(ANSI_COLOR_GREEN"%s" RESET, promstr);
    }
    else
    {
        while (head)
        {
            printf("[%d]-  %-24s %s\n", i++, "Stopped", head->commandname);

            head = head->link;
        }
    }
}
// function defnition to excute another commands
void execute_no_commands(char *input_string)
{
    // to excute jobs command
    if (strcmp(input_string, "jobs") == 0)
    {
        print_list(head);
    }
    // to excute fg command
    else if (strcmp(input_string, "fg") == 0)
    {
        kill(head->id, SIGCONT);
        printf("%s continued with pid %d\n", head->commandname, head->id);
        waitpid(head->id, &info, WUNTRACED);
        respid=0;
        sl_delete_first(&head);
    }
    // to excute bg command
    else if (strcmp(input_string, "bg") == 0)
    {
        // signal(SIGCHLD, signal_handler);
        printf("%s continued &\n", head->commandname);

        kill(head->id, SIGCONT);
        sl_delete_first(&head);
    }
    else
    {
        printf("Invalid Commands\n");
    }
}