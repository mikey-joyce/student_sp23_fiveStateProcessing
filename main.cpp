#include "process.h"
#include "ioModule.h"
#include "processMgmt.h"

#include <chrono> // for sleep
#include <thread> // for sleep

int main(int argc, char* argv[])
{
    // single thread processor
    // it's either processing something or it's not
    //    bool processorAvailable = true;

    // vector of processes, processes will appear here when they are created by
    // the ProcessMgmt object (in other words, automatically at the appropriate time)
    list<Process> processList;
    
    // this will orchestrate process creation in our system, it will add processes to 
    // processList when they are created and ready to be run/managed
    ProcessManagement processMgmt(processList);

    // this is where interrupts will appear when the ioModule detects that an IO operation is complete
    list<IOInterrupt> interrupts;   

    // this manages io operations and will raise interrupts to signal io completion
    IOModule ioModule(interrupts);  

    // Do not touch
    long time = 1;
    long sleepDuration = 50;
    string file;
    stringstream ss;
    enum stepActionEnum {noAct, admitNewProc, handleInterrupt, beginRun, continueRun, ioRequest, complete} stepAction;

    // Do not touch
    switch(argc)
    {
        case 1:
            file = "./procList.txt";  // default input file
            break;
        case 2:
            file = argv[1];         // file given from command line
            break;
        case 3:
            file = argv[1];         // file given
            ss.str(argv[2]);        // sleep duration given
            ss >> sleepDuration;
            break;
        default:
            cerr << "incorrect number of command line arguments" << endl;
            cout << "usage: " << argv[0] << " [file] [sleepDuration]" << endl;
            return 1;
            break;
    }

    processMgmt.readProcessFile(file);


    time = 0;
    bool processorAvailable = true;
    Process front, next; // current running process
    IOEvent front_io; // current running processes front IO event
    int remaining = 1; //amount of remaining time in process
    bool admitted = false;
    std::list<Process>::size_type nextIndex = 1;

    //keep running the loop until all processes have been added and have run to completion
    while(processMgmt.moreProcessesComing() || remaining >= 0  /* TODO add something to keep going as long as there are processes that arent done! */ )
    {
      //Update our current time step
      ++time;

      //let new processes in if there are any
      processMgmt.activateProcesses(time);

      //update the status for any active IO requests
      ioModule.ioProcessing(time);

      //If the processor is tied up running a process, then continue running it until it is done or blocks
      //   note: be sure to check for things that should happen as the process continues to run (io, completion...)
      //If the processor is free then you can choose the appropriate action to take, the choices (in order of precedence) are:
      // - admit a new process if one is ready (i.e., take a 'newArrival' process and put them in the 'ready' state)
      // - address an interrupt if there are any pending (i.e., update the state of a blocked process whose IO operation is complete)
      // - start processing a ready process if there are any ready

      //init the stepAction, update below
      stepAction = noAct;

      //TODO add in the code to take an appropriate action for this time step!
      //you should set the action variable based on what you do this time step. you can just copy and paste the lines below and uncomment them, if you want.
      //stepAction = continueRun;  //runnning process is still running
      //stepAction = ioRequest;  //running process issued an io request
      //stepAction = complete;   //running process is finished
      //stepAction = admitNewProc;   //admit a new process into 'ready'
      //stepAction = handleInterrupt;   //handle an interrupt
      //stepAction = beginRun;   //start running a process

        //if there are processes and availibility
      if(!processList.empty() && processorAvailable){
        //check to see if a process was recently admitted
        if(admitted){ 
          // if there is more than one process in the list
          if(processList.size() > 1){
            if(nextIndex > processList.size()){
              list<Process>::iterator it = processList.begin();
              advance(it, nextIndex);
              nextIndex++;
              next = *it; 
            }
            //cout << next.arrivalTime;
            //cout << processList.size();

            if(next.arrivalTime <= time && next.state != ready){
              stepAction = admitNewProc; //admit process
              next.state = ready; // STATES ARE NOT WORKING RIGHT NOW
            }
            else{
              //start processing if there is nobody else to admit
              front.state = processing; // STATES ARE NOT WORKING RIGHT NOW
              processorAvailable = false;
              stepAction = beginRun;
              admitted = false;
            }
          }
          else{
            //start processing if new admit and no others to be admitted
            front.state = processing; // STATES ARE NOT WORKING RIGHT NOW
            processorAvailable = false;
            admitted = false;
            stepAction = beginRun;
            front.doneTime = front.reqProcessorTime - 1;
            remaining = front.doneTime;
          }
        }
        //if a process wasnt just admitted
        else{
          front = processList.front(); //get first process in 
          stepAction = admitNewProc; //admit process
          admitted = true;
          front.state = ready; // STATES ARE NOT WORKING RIGHT NOW
        }

      }
      // if there is a process running
      else if(!processorAvailable){
        // if there are io events within our process
        if(!front.ioEvents.empty()){
          front_io = front.ioEvents.front();
        }
        // if there are no io events within our process
        else{
          //if there is still time left
          if (front.doneTime > 0){
            front.doneTime--;
            remaining--;
            stepAction = continueRun;
          }
          //if process is finished
          else{
            stepAction = complete;
            front.state = done;
            processList.pop_front();
            processorAvailable = true;
            nextIndex = 1;
            front = processList.front();

            if(processList.empty()){
              remaining--;
            }
          }
        }
      }
        //cout << front.doneTime;

        // Leave the below alone (at least for final submission, we are counting on the output being in expected format)
        cout << setw(5) << time << "\t"; 
        
      switch(stepAction)
      {
          case admitNewProc:
            cout << "[  admit]\t";
            break;
          case handleInterrupt:
            cout << "[ inrtpt]\t";
            break;
          case beginRun:
            cout << "[  begin]\t";
            break;
          case continueRun:
            cout << "[contRun]\t";
            break;
          case ioRequest:
            cout << "[  ioReq]\t";
            break;
          case complete:
            cout << "[ finish]\t";
            break;
          case noAct:
            cout << "[*noAct*]\t";
            break;
      }

        // You may wish to use a second vector of processes (you don't need to, but you can)
        printProcessStates(processList); // change processList to another vector of processes if desired

        this_thread::sleep_for(chrono::milliseconds(sleepDuration));
    }

    return 0;
}
