#include "process.h"

void printProcessStates(list<Process>& processVector)
{
    char stateChar;
    for(auto & Proc : processVector)
    {
        switch (Proc.state)
        {
            case ready:
                stateChar = 'r';
                break;
            case processing:
                stateChar = 'p';
                break;
            case blocked:
                stateChar = 'b';
                break;
            case newArrival:
                stateChar = 'n';
                break;
            case done:
                stateChar = 'd';
                break;
        }
        /*if(Proc.id != 999999){
            cout << Proc.id << stateChar << ' ';
        }*/

        cout << Proc.id << stateChar << ' ';
    }
    cout << endl;
}

void printProcessSet(vector<Process>& processVector)
{
    cout << "AT | DT | RQPT | PT | S | IO" << endl;
    for(auto & Proc : processVector)
    {
        Proc.printProcess();
    }
}
