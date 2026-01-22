# Mini Shell in C (Linux Internals)

## 📌 Description

Developed a Mini Shell in C that simulates core functionality of a Linux shell. The project demonstrates Linux system programming concepts such as process creation, command execution, signal handling, and inter-process communication, providing hands-on understanding of how shells work internally.

## 🎯 Key Features

* Execution of built-in and external commands
* Support for multiple pipes (n-pipe handling)
* Custom shell prompt
* Foreground and background job control (`fg`, `bg`, `jobs`)
* Signal handling for `Ctrl+C`, `Ctrl+Z`, and `SIGCHLD`
* Process management using `fork()`, `execvp()`, and `waitpid()`

## 🛠️ Technologies Used

* Programming Language: C
* Platform: Linux
* Concepts: System Calls, Process Control, IPC (Pipes), Signal Handling

## ▶️ How to Compile and Run

Using Makefile:

```bash
make
./minishell
```

Or compile manually:

````bash
gcc *.c -o minishell
./minishell
````

## ⚙️ Project Flow

1. Display custom shell prompt
2. Read user input
3. Parse and identify command type (internal / external)
4. Execute command using appropriate system calls
5. Handle foreground/background execution
6. Monitor and handle signals

## 📚 Learnings from the Project

* Clear understanding of `fork()`, `execvp()`, and `waitpid()`
* Practical implementation of inter-process communication using pipes
* Deep understanding of Linux signal handling mechanisms
* Understanding of process states: running, stopped, and terminated
* Hands-on experience implementing `fg`, `bg`, and `jobs` commands

## 🧠 Skills Demonstrated

* Linux System Programming
* Process and Signal Management
* IPC (Pipes)
* Command Parsing and Execution
* Debugging and Problem Solving
