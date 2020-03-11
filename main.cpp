// Test compilation of CommandMessenger
#include <iostream>

using namespace std;

#include "CmdMessenger.h" // This is template - must include all

// Attach a new CmdMessenger object to the default Serial port (warning the name must be cmdMessenger)
#include <sstream>

stringstream channelin;
stringstream channelout;


int getStreamSize(stringstream *comms) {
	int currentpos = comms->cur;
	comms->seekg(0, std::ios::end);
	int size = comms->tellg();
	comms->seekg(currentpos-1, std::ios::beg);
	return size;
} 

class InputManager : public CMInputManager {
  public:
    stringstream &_channelin;
    InputManager(stringstream &channelin) : _channelin(channelin) {
    }

    int available() {
      int currentpos = _channelin.cur;
      _channelin.seekg(0, std::ios::end);
      int size = _channelin.tellg();
      _channelin.seekg(currentpos-1, std::ios::beg);
      return size;
    }

    int get() {
      return _channelin.get();
    }  

    void read(char * buffer, size_t& size) {
      _channelin.read(buffer, size);
    } 
} ;

InputManager inputMgr(channelin);

class OutputManager : public CMOutputManager {
  public:
    stringstream &_channelout;
    OutputManager(stringstream &channelout) : _channelout(channelout) {
    }
  	virtual void print(const char val)
    {
      _channelout << val;
    }  
  	virtual void print(const int val)
    {
      _channelout << val;
    }  
  	virtual void print(const float val)
    {
      _channelout << val;
    }  
  	virtual void print(const double val)
    {
      _channelout << val;
    }  
  	virtual void print(const char* val)
    {
      _channelout << val;
    }
} ;


template <class T> OutputManager& operator<<(OutputManager &oMgr, T val) {
    oMgr._channelout << val;
    return oMgr;
}

OutputManager outputMgr(channelout);

CmdMessenger cmdMessenger(inputMgr, outputMgr);


#define StatusLedCommandList \
  kLedCmd

// This is the list of recognized commands. These can be commands that can either be sent or received. 
// In order to receive, attach a callback function to these events
// This partial enum
#define DHTCommandList \
  kTempHumStatus

/**
 * Place Holder for configuration NYI
 */
#define configurationCommandlist \
  kConfCmd1, \
  kConfCmd2

// This is the list of recognized commands. These can be commands that can either be sent or received. 
// In order to receive, attach a callback function to these events
enum {
  // Commands
  kAcknowledge         , // Common Command to acknowledge that cmd was received
  kError               , // Common Command to report errors
  kDeviceReady         , // Common Command to ready device
      // end of generic action
  configurationCommandlist, // Commands for configuration NYI
  DHTCommandList,           // Commands for DHT Device

};

const unsigned int kSEND_CMDS_END= kDeviceReady+1; // Must be set after the number of the last generic action 


// ------------------ D E F A U L T  C A L L B A C K S -----------------------

void device_ready() {
  // In response to ping. We just send a throw-away Acknowledgement to say "i m alive"
  // Short string for constained protocole

  cmdMessenger.sendCmd(kDeviceReady, 
    __TIMESTAMP__ "," "ready" "," "1" "," "0" "," "tok" "," "_SD_NODE_ID" "," "_SD_SOFTWARE_ID" "," 
    "_SD_VERSION_DESC" ","
    "arduino:avr:diecimila:cpu=atmega328"
  );
}

void unknownCmd() {
  // Default response for unknown commands and corrupt messages
  cmdMessenger.sendCmd(kError,
              "Unknown command"
        );
}



// ------------------ S E T U P ----------------------------------------------
// template class void CmdMessenger<InputManager, OutputManager>::attach(unsigned char, void (*)());
void attach_callbacks(messengerCallbackFunction* callbacks) {
  int i = 0;
  int offset = kSEND_CMDS_END;
  while(callbacks[i]) {
    cmdMessenger.attach(offset+i, callbacks[i]);
    i++;
  }
}

/**
 * Place Holder for configuration NYI
 */

#define configurationCommandCallbackFunction \
  unknownCmd, \
  unknownCmd

// Ask for status
void OnDHTStatus() {
  cmdMessenger.sendCmdStart(kTempHumStatus);
  // Convert to string by hand (better for NRF 52 WString)    
  cmdMessenger.sendCmdArg(25.3);
  cmdMessenger.sendCmdArg(50.1);
  cmdMessenger.sendCmdEnd();
}

// Commands we send from the PC and want to recieve on the Arduino.
// We must define a callback function in our Arduino program for each entry in the list below vv.
// They start at the address kXXX defined in  defined DHTCommandList above DHTActions
#define DHTCallbackFunction \
  OnDHTStatus

// Commands we send from the PC and want to recieve on the Arduino.
// We must define a callback function in our Arduino program for each entry in the list below vv.
// They start at the address kSEND_CMDS_END defined ^^ above as 004
messengerCallbackFunction messengerCallbacks[] =  {
  configurationCommandCallbackFunction, // Callbacks for configuration commande NYI
  unknownCmd, // No call back for led
  DHTCallbackFunction,                  // Callbacks for DHT Device

  NULL   // Must keep that
};



void setup() {
      // Attach default / generic callback methods
  cmdMessenger.attach(kDeviceReady, device_ready);
  cmdMessenger.attach(unknownCmd);

  // Attach my application's user-defined callback methods
  attach_callbacks(messengerCallbacks);

}  

int main(int argc, char const *argv[]) {
    cout << "Starting..." << endl;
    
    setup();
    //channelin.seekg(-1, channelin.beg);
    while (true) {
        string userInput;
        cin >> userInput;
        channelin.str(userInput);
        cmdMessenger.feedinSerialData();
        cout << " ==>"  << channelout.str() << endl;
        channelout.str(""); // consume all 
    } 
    cout << "End" << endl;
    return 0;
}
