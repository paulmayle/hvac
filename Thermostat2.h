// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _Thermostat2_H_
#define _Thermostat2_H_
#include "Arduino.h"
//add your includes for the project Thermostat2 here


//end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
void displayTargetTemp(int t);
void updateIdleScreen();
void updateProgramScreen();
void updateCurrentTemp();
void getCurrentTemp(char *buffer);
bool processUserInput();
void toggleMode();
void updateDisplay();
void updateProgramDisplayDetails(char * mode, char * info, word bg, word tx, word br);
void displayTargetTemp(int tt);
bool defineProgramScreen();
bool defineIdleScreen();
void resetScreen();
bool defineAltScreen();

#ifdef __cplusplus
} // extern "C"
#endif

//add your function definitions for the project Thermostat2 here




//Do not add code below this line
#endif /* _Thermostat2_H_ */
