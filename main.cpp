// Test compilation of CommandMessenger
#include <iostream>

using namespace std;

#include "CmdMessenger.h"

// Attach a new CmdMessenger object to the default Serial port (warning the name must be cmdMessenger)
#include <sstream>

stringstream channel;


CmdMessenger cmdMessenger(channel);


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
  StatusLedCommandList,     // Commands for status led // Not used for nrf52
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

void attach_callbacks(messengerCallbackFunction* callbacks) {
  int i = 0;
  int offset = kSEND_CMDS_END;
  while(callbacks[i]) {
    cmdMessenger.attach(offset+i, callbacks[i]);
    i++;
  }
}


void setup() {
      // Attach default / generic callback methods
  cmdMessenger.attach(kDeviceReady, device_ready);
  cmdMessenger.attach(unknownCmd);

  // Attach my application's user-defined callback methods
  //attach_callbacks(messengerCallbacks);

}  

int main(int argc, char const *argv[]) {
    cout << "Starting..." << endl;
    
    setup();
    channel << "2;" << "\r\n" << "2;" << endl;
    channel.seekg(0, channel.beg);
    cout << " First cmd : " << channel.str() <<  " (" << ")" << endl;
    channel.seekg(0, channel.beg);
    //while (true) {
        for (int i=0; i<3; i++) { 
        cmdMessenger.feedinSerialData();
        cout << "." << endl; 
    } 
    cout << "End" << endl;
    return 0;
}
