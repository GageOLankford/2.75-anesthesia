void setup() {
  Serial.begin(9600);
}
class Medication {
  private:
    String medicationName;
    int medicationPort;
    bool loaded;

  public: 
    Medication(String name, int port, bool load) {
      medicationName = name;
      medicationPort = port;
      loaded = load;
    }
    Medication() {
      //default constructor
      medicationName = "";
      medicationPort = -1;
      loaded = false;
    }
    void dispense(float amount_mL, int dispenseTimeInSeconds) {
      // dispense logic, make sure its non blocking
    }
};

Medication medicationObjects[4]; //change size based on how many meds max

enum State {
  PREPARATION,
  AWAITING_COMMAND,
  DISPENSING,
  SHUTDOWN
};

State state = PREPARATION;


void loop() {
  switch (state) {
    case PREPARATION:
      //dispense carrier fluid, if you have any medication loaded
      //have medicine class input through terminal, and load a medicine physically
      //should attach into medicationObjects list
      state = AWAITING_COMMAND;
      break;
    
    case AWAITING_COMMAND:
      // dispense carrier fluid always

      //use terminal and wait for input
      //have relevant med or carrier fluid dispense
      // input should be medicationName, or port number, + amount and dispense time
      // store input as buffer var
      state = DISPENSING;
      //can also unload and shut down machine
      state = SHUTDOWN;
      //can also load more medication
      state = PREPARATION;
      break;
    
    case DISPENSING:
      // dispense carrier fluid always
      //process buffer var for medicationName, or port number, + amount and dispense time
      //call class method for relevant medication
      state = AWAITING_COMMAND;
      //if we run out of medication (from some sort of closed loop sensing)
      state = PREPARATION;
      break;

    case SHUTDOWN:
      //whatever shutdown process we'd need
      state = PREPARATION;
    break;
  }
}
