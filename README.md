## Task to join global logic basecamp.

### Application (App) to find file on Local Host:

1. App has to find file by "name"
2. Search has to be started by root "/" directory. (or Disk "C:\" on Windows OS)
3. To make this search faster - App creates new Searching Thread for each sub-directory in root directory, but not more than 8 Executions Threads.
4. Thread who will find needed file has to print  Full Path to the file and has to notify all the rest threads to stop searching

- Logic "to search file" has to be placed into static library.
- Application just uses this library to find files in a faster way.
- Static Library compilation has to be done separately from Application.
- Compiler GCC or G++.
- Use Makefile for compilation/linking instructions.
