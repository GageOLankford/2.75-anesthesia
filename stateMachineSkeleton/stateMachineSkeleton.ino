void setup() {
  Serial.begin(9600);
  while (!Serial) {}      // Wait for serial (important on some boards)
  Serial.println("Serial established, starting program");
}
class Medication {
  public: 
    String medicationName;
    int medicationPort;

    Medication(String name, int port) {
      medicationName = name;
      medicationPort = port;
    }
    Medication() {
      //default constructor
      medicationName = "";
      medicationPort = -1;
    }
    void dispense(float amount_mL, int dispenseTimeInSeconds) {
      // dispense logic, make sure its non blocking
    }
};
const int maxMeds = 4;
Medication medicationObjects[maxMeds];

enum State {
  PREPARATION,
  AWAITING_COMMAND,
  DISPENSING,
  SHUTDOWN
};

State state = PREPARATION;

String input = "";


// Flags and buffer variables
bool sentMedNameRequest = false;
bool sentPortNumberRequest = false;
int medNum = -1;

bool sentCommandRequest = false;

int dispenseAmount = -1; //mL
int dispenseTime = -1; //seconds
String dispenseName = "";
bool sentNameRequest = false;
bool sentAmountRequest = false;
bool sentTimeRequest = false;

void loop() {
  switch (state) {
    case PREPARATION: {
      /*finds the unoccupied medication slot, not really robust,
      as in it will break if we try to go over med slot and there
      is no way to empty a slot for now*/
      if (medNum == -1) {
        for (int i = 0; i < maxMeds; i++) {
          if (medicationObjects[i].medicationName == "") {
              medNum = i;
              break;
          }
        }
      }

      /*
      This block is lowkey bs, but its necessary bs. It uses some boolean flags to 
      print request once while not blocking the flow of the code. This isn't essential right now,
      but, I imagine we don't want dispensing to halt for carrier fluid, which we can put outside of switch
      statement so it runs every loop
      */
      if (sentMedNameRequest != true) {
        Serial.println("Fill in medicationName, then click enter");
        sentMedNameRequest = true;
      }
      else if (sentPortNumberRequest != true &&  medicationObjects[medNum].medicationName != "") {
        Serial.println("Fill in portNumber, then click enter");
        sentPortNumberRequest = true;
      }

      if (Serial.available() > 0) {
        char c = Serial.read();

        if (c == '\n') {
          input.trim();
          if (sentPortNumberRequest == false) {
            medicationObjects[medNum].medicationName = input;
          }
          else {
            medicationObjects[medNum].medicationPort = input.toInt();
          }
          input = "";
        } else {
          input += c;
        }
      };
      
      // Makes sure the relevant med object is filled out before letting state terminate
      if (medicationObjects[medNum].medicationName == "" || medicationObjects[medNum].medicationPort == -1) {
        break;
      };

      //Transitions to new state, and resets all the flags
      state = AWAITING_COMMAND;
      sentMedNameRequest = false;
      sentPortNumberRequest = false;
      medNum = -1;
      break;
    }
    case AWAITING_COMMAND: {
      // Literally just a transition state, just waits to be told what you want to do next

      if (sentCommandRequest == false) {
        Serial.println("Awaiting command of format \'dispense\', \'load\' or \'shutdown\':");
        sentCommandRequest = true;
      }

      if (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '\n') {
          input.trim();
          if (input == "dispense") {
            state = DISPENSING;
          }
          else if (input == "load") {
            state = PREPARATION;
          }
          else if (input == "shutdown") {
            state = SHUTDOWN;
          }
          input = "";
          sentCommandRequest = false;
        } else {
          input += c;
        }
      }
      break;
    }
    case DISPENSING: {
        if (!sentNameRequest) {
            Serial.println("Enter name of medication to dispense:");
            sentNameRequest = true;
        }

        if (Serial.available() > 0 && dispenseName == "") {
            char c = Serial.read();
            if (c == '\n' || c == '\r') {
                input.trim();
                dispenseName = input;
                input = "";

                for (int i = 0; i < maxMeds; i++) {
                    if (medicationObjects[i].medicationName == dispenseName) {
                        medNum = i;  // save selected med index
                        break;
                    }
                }
            } else {
                input += c;
            }
        }

        if (dispenseName != "" && dispenseAmount == -1) {
            if (!sentAmountRequest) {
                Serial.println("Enter amount to dispense (mL):");
                sentAmountRequest = true;
            }

            if (Serial.available() > 0) {
                char c = Serial.read();
                if (c == '\n' || c == '\r') {
                    input.trim();
                    dispenseAmount = input.toInt();
                    input = "";
                } else {
                    input += c;
                }
            }
        }

        if (dispenseName != "" && dispenseAmount != -1 && dispenseTime == -1) {
            if (!sentTimeRequest) {
                Serial.println("Enter dispense time (seconds):");
                sentTimeRequest = true;
            }

            if (Serial.available() > 0) {
                char c = Serial.read();
                if (c == '\n' || c == '\r') {
                    input.trim();
                    dispenseTime = input.toInt();
                    input = "";
                } else {
                    input += c;
                }
            }
        }

        // Step 4: Perform dispensing once all inputs are valid
        if (dispenseName != "" && dispenseAmount != -1 && dispenseTime != -1 && medNum != -1) {
            Serial.print("Dispensing ");
            Serial.print(dispenseAmount);
            Serial.print(" mL of ");
            Serial.print(dispenseName);
            Serial.print(" (port ");
            Serial.print(medicationObjects[medNum].medicationPort);
            Serial.print(") over ");
            Serial.print(dispenseTime);
            Serial.println(" seconds.");

            // Call dispense method (non-blocking placeholder)
            medicationObjects[medNum].dispense(dispenseAmount, dispenseTime);

            // Reset flags for next dispense
            medNum = -1;
            dispenseName = "";
            dispenseAmount = -1;
            dispenseTime = -1;
            sentNameRequest = false;
            sentAmountRequest = false;
            sentTimeRequest = false;

            // Return to awaiting command
            state = AWAITING_COMMAND;
        }
        break;
    }
    case SHUTDOWN:
      //whatever shutdown process we'd need
      break;
  }
  /*
  Will need to have dispensing logic outside of state machine, but have the dispensing state modified within
  this allows for input calls and staying in state, while still dispensing carrier fluid + medications
  */
}
