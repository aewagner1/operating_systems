#include "myShell.h"

using namespace std;

void myShell::execute(){
    while(true) {
        cout << greeting;
        cin >> command;

        parse_and_execute(command);
    }
}

void myShell::parse_and_execute(string c) {
    cout << "That sounds like a good idea." << endl;
}