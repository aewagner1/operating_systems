// Program to print "ls" output in a pretty format

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

void WaitFor(int pid){
  int gotpid, status;

  while( (gotpid = wait(&status)) != pid)
    cout << "Process [" << gotpid << "] Finished..." << endl;
}

int main(int argc, char **argv) {

    //intialize variables
    int pipefd[2];
    int pipe2fd[2];
    string s;


    pipe(pipefd);

    int a = fork();

    if (a == 0) { // Child 1

        //allow writing at write end of pipe
        close(pipefd[0]);
        close(1);
        dup(pipefd[1]);

        // uses argv[1] as argument
        // &argv[1] -> references argv starting at index 1
        execvp(argv[1], &argv[1]);

        //prints line if execvp() fails to run
        cout << "That was not fun!!!" << endl;

    } else { // Parent

        //allow reading at read end of pipe
        close(pipefd[1]);
        close(0);
        dup(pipefd[0]);

        //feed in & print output of child 1
        cout << "child1:" << endl;
        while (getline(cin, s))
            cout << "CHILD 1 " << " *** " << s << " ---" << endl;

        WaitFor(a);

        //prints when finished
        cout << "That was fun!!!" << endl;

        pipe(pipe2fd);
        int b = fork();


        if (b == 0) { // Child 2

            cout << "Arvg 1 = " << argv[1] << endl;

            //allow writing at write end of pipe
            close(pipe2fd[0]);
            close(1);
            dup(pipe2fd[1]);

            // uses argv[1] as argument
            // &argv[1] -> references argv starting at index 1
            execvp(argv[1], &argv[1]);

            //prints line if execvp() fails to run
            cout << "That was not fun!!!" << endl;
        }

        //allow reading at read end of pipe
        close(pipe2fd[1]);
        close(0);
        dup(pipe2fd[0]);

        //feed in & print output of child 1
        cout << "child2:" << endl;
        while (getline(cin, s))
            cout << "CHILD 2 " << " ### " << s << " @@@" << endl;

        //prints when finished
        cout << "That was fun!!!" << endl;
    }
}
