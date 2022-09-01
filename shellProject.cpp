#include "myShell.h"
#include <bits/stdc++.h>
#include <sys/wait.h>

using namespace std;


vector<string> history;


int main(int argc, char* argv[]){
    myShell ms;
    ms.execute();
}

 
void myShell::execute(){
    
    while(true){
        cout << greeting;
        getline(cin, command);
        newHistory(command);
        parse_and_execute(command);
    }
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*
* WaitFor(pid)                                                  *
* Waits for foreground processes...based on Rochkind's waitfor  *
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void myShell::WaitFor(int pid){
  int gotpid, status;

  while( (gotpid = wait(&status)) != pid)
    cout << "Process [" << gotpid << "] Finished..." << endl;
}


void myShell::parse_and_execute(string c){
    int id = fork();
    if(id == 0){ // we are in the child process

        vector<char *> argv;
        istringstream iss(c);

        string token;
        while(iss >> token) {
            char *arg = new char[token.size() + 1];
            copy(token.begin(), token.end(), arg);
            arg[token.size()] = '\0';
            argv.push_back(arg);
        }
        argv.push_back(0);
        
        commands(argv, c);
        execvp(argv[0], &argv[0]);

        for(size_t i = 0; i < argv.size(); i++)
            delete[] argv[i];


    }
    else{
        // Here we are in the parent
        WaitFor(id);
    }

    if(c.compare("exit") == 0){
        exit(0);
    }

}




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*
* commands(parsed)                                              *
* Holds list of built cmds... runs terminal cmds -> built cmds  *
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int myShell::commands(vector<char*> parsed, string c) {
   
    // background cmd ..... COMPLETED ***** 30 POINTS
    if(c.find("&") != string::npos) {
        cmdBackground(splitString(c));
        exit(0);
        return 1;
    }

    // stdin cmd ..... COMPLETED ***** 40 POINTS
    if(c.find("<") != string::npos) {
        standardIn(splitString(c));
        exit(0);
        return 1;
    }

    // stdout cmd ..... COMPLETED ***** 40 POINTS
    if(c.find(">") != string::npos) {   
        standardOut(splitString(c));  
        exit(0);
        return 1;          
    }

    // history cmd ..... COMPLETED ***** 30 POINTS
    if(c.find("history") != string::npos) {
        cout << cmdHistory();
        exit(0);
        return 1;
    }

    // repeat cmd ..... COMPLETED ***** 30 POINTS
    if(c.find("!") != string::npos) {
        parse_and_execute(cmdRepeat(c));
        exit(0);
        return 1;
    }
    
    return 1;
}




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*
* cmdBackground()                                               *
* takes cmd and runs it while leaving the shell accessible      *
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void myShell::cmdBackground(vector<string> strings) {
    int a = fork();
    if (a == 0) {    
    string command = "";
    for (string s : strings)
      if (s != "&") command.append(s);
    
    parse_and_execute(command);
    exit(0);
    } else {
        execute();    
    }
}




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*
* stdout()/stdin()                                              *
* splits strings... runs them through std out & in              *
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// Splits given string by spaces
vector<string> myShell::splitString(string c) {
    vector<string> split;
    stringstream ss(c);
        string word;
        while (ss >> word) {
            split.push_back(word);
        }

    return split;
}


// Sends output into a specified file
void myShell::standardOut(vector<string> strings){
    vector<string> before;
    vector<string> after;
    string s;
    int switchNow = 0;

    // splits strings into two vectors before/after ">"
    for(int i = 0; i < strings.size(); i++) {
        if(strings.at(i) == ">") {
            switchNow = 1;
            continue;
        }
        if(switchNow == 1) after.push_back(strings.at(i));
        else before.push_back(strings.at(i));
    }

        // Combine everything in before to a string
        string commandGiven = "";
            for(int i = 0; i < before.size(); i++) {
                commandGiven.append(" ");
                commandGiven.append(before.at(i));
            }

        // If string contains ! then run cmd from history
        if (commandGiven.find("!") != string::npos) {
                string command = cmdRepeat(before.at(0));
                command.append(" > ");  
                command.append(after.at(0));   

                // append "> filename" & execute cmd
                parse_and_execute(command);
                return;
            }

    //Time to build the pipe :(
    int pipefd[2];
    pipe(pipefd);
    int a = fork();

    if(a == 0) {

        // Open writing end of pipe
        close(pipefd[0]);
        close(1);
        dup(pipefd[1]);


        // Check if history cmd is present, if so return history
        if (find(before.begin(), before.end(), "history") != before.end())
            cout << cmdHistory();
        
        else { // Run anything else through execvp()
            vector<char *> argv;
            istringstream iss(commandGiven);

            string token;
            while(iss >> token) {
                char *arg = new char[token.size() + 1];
                copy(token.begin(), token.end(), arg);
                arg[token.size()] = '\0';
                argv.push_back(arg);
            }
            argv.push_back(0);
            
            execvp(argv[0], &argv[0]);

            // should only run if execvp() doesn't recognize command
            cout << "INVALID COMMAND" << endl;
            cout << commandGiven << endl;
        }

    } else {

        // Wait for child to finish work
        WaitFor(a); 

        // Open the reading end of pipe
        close(pipefd[1]);
        close(0);
        dup(pipefd[0]);

        // Open file and print information from child
        ofstream myfile;
        myfile.open (after.at(0));
        while(getline(cin, s))
            myfile << s << endl;
        myfile.close();

        // Exit Parent (really child from parse_and_execute())
        exit(0);
    }
}


// Sends given input to command called
void myShell::standardIn(vector<string> strings){
    vector<string> before;
    vector<string> after;
    vector<string> ifOutput;
    string s;
    int switchNow = 0;

    // splits strings into two vectors before/after "<"
    for(int i = 0; i < strings.size(); i++) {
        if(strings.at(i) == "<") {
            switchNow = 1;
            continue;
        }

        if(strings.at(i) == ">") {
            switchNow = 2;
            continue;
        }

        if(switchNow == 1) after.push_back(strings.at(i));
        else if(switchNow == 2) ifOutput.push_back(strings.at(i));
        else before.push_back(strings.at(i));
    }

        // Combine everything in before to a string
        string inputGiven = "";
            for(int i = 0; i < before.size(); i++) {
                inputGiven.append(before.at(i));
            }

        string command = inputGiven + " " + after.at(0);
        if(switchNow == 2) command = command + " > " + ifOutput.at(0);

        parse_and_execute(command);

        // Exit Parent (really child from parse_and_execute())
        exit(0);
}




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*
* cmdHistory()                                                  *
* Adds cmds to a vector... prints history of used cmds          *
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// pushes the given cmd to the history vector
void myShell::newHistory(string c) {
    history.push_back(c);
}

// Prints the history vector as a string with desired spacing
string myShell::cmdHistory() {
    int i = 1;
    string historyyy = "";
    for(string str: history) {
        historyyy.append("  ");
        historyyy.append(to_string(i));
        historyyy.append("  ");
        historyyy.append(str);
        historyyy.append("\n");

        i++;
    }

    return historyyy;
}




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*
* cmdRepeat()                                                   *
* Grabs cmd from index... runs the cmd and prints               *
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
string myShell::cmdRepeat(string c) {
    size_t pos = c.find("!");           // finds the position of ! in the string
    string command = c.substr(pos);     //takes string starting at ! ###just precaution###
    string index = command.substr(1);   // index is the string after !
    string newCommand = "";

    int cmdIndex = stoi(index);         //converts string index into int

    // references history vector for element at cmdIndex and returns that cmd
    if(history.size() < cmdIndex) {
        cout << "No such command." << endl;
        return "No such command.";
    } else 
        newCommand = history.at(cmdIndex - 1);
    return newCommand;    
}
