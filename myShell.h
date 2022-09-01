#include <iostream>
#include <string>       
#include <sys/types.h>
#include <unistd.h>
#include <vector>

using namespace std;
 
class myShell {
private:

public: 

    string userName = getenv("USER");
    string greeting = "\033[1;4;36m|Welcome " + userName + "|\033[0m  ";
    string command;

    // The run loop
    void execute();

    // Helper Functions
    void parse_and_execute(string c);

    void WaitFor(int pid);

    // Holds command list
    int commands(vector<char*> parsed, string c);

    //Runs command in the background
    void cmdBackground(vector<string> strings);

    // Runs stdout() & stdin() cmds
    vector<string> splitString(string c);
    void standardOut(vector<string> strings);
    void standardIn(vector<string> strings);

    // Runs History command
    void newHistory(string c);
    string cmdHistory();

    // Runs Repeat cmd
    string cmdRepeat(string c);
};