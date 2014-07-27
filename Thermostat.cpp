// Do not remove the include below
#include "Thermostat.h"

#include <UTFT.h>
#include <UTouch.h>
#include "Timer.h"
#include "Button.h";

unsigned int mode = 0;
unsigned int lastMode = 0;

unsigned int displayMode = 0;

#define  OFF 0
#define  HEAT 1
#define  COOL 2
#define MAX_TARGET 99
#define MIN_TARGET 55

#define NULL_SCREEN 0
#define IDLE_SCREEN 1
#define PROG_SCREEN 2
#define ALT_SCREEN 3

#define UP_ARROW "0"
#define DOWN_ARROW "1"


// VGA color palette
const word LCD_BLACK = 0x0000;
const word LCD_WHITE = 0xFFFF;
const word LCD_RED = 0xF800;
const word LCD_GREEN = 0x0400;
const word LCD_BLUE = 0x001F;
const word LCD_SILVER = 0xC618;
const word LCD_GRAY = 0x8410;
const word LCD_MAROON = 0x8000;
const word LCD_YELLOW = 0xFFE0;
const word LCD_OLIVE = 0x8400;
const word LCD_LIME = 0x07E0;
const word LCD_AQUA = 0x07FF;
const word LCD_TEAL = 0x0410;
const word LCD_NAVY = 0x0010;
const word LCD_FUCHSIA = 0xF81F;
const word LCD_PURPLE = 0x8010;
const word LCD_TRANSPARENT = 0xFFFFFFFF;
int currentTemp = 78;

int sensorPin = 0;

int targetTemp = 0;
int minCount = 0;
int hourCount = 0;
int count = 0;
int lastVal = 0;

// Declare which fonts we will be using
//extern uint8_t Inconsola[];
extern uint8_t BigFont[];
extern uint8_t SmallFont[];
extern uint8_t SevenSegNumFont[];
//extern uint8_t arrows[];
extern uint8_t arrows7[];
//extern uint8_t DotMatrix_L_Num[];
//extern uint8_t DotMatrix_XL_Num[];
//extern uint8_t UbuntuBold[];
//extern uint8_t franklingothic_normal[];
extern uint8_t SevenSeg_XXXL_Num[];
//extern uint8_t arial_normal[];

//extern unsigned int arrowDown[0x2748];
//extern unsigned int arrowUp[0x2748];
//extern unsigned int tux[0x1000];

Button *ptrIdleScreenButton;
Button *ptrModeButton;
Button *ptrProgramButton;
Button *ptrControlButton;
Button *ptrUpArrow;
Button *ptrDownArrow;
Button *ptrCurrentTemp;
Button *ptrDesiredTemp;
Button *ptrInfoBox;
Button *ptrMainDisplayArea;

UTFT *ptrDisplay;
UTouch *ptrTouch;

int x, y;
char stCurrent[20] = "";
int stCurrentLen = 0;
char stLast[20] = "";
int idleScreenDisplayState = 0;
int programScreenDisplayState = 0;
int screenMode = 0;

//boolean screenHasBeenTouched=false;

Timer updateDisplayTime(5000); // update the display once every 5 seconds to avoid redrawing every time round the main loop
Timer idleScreenTimer(10000); // sets time to revert to idle screen after last keypress

// Uncomment the next two lines for the Arduino 2009/UNO
//UTFT        myGLCD(ITDB24D,19,18,17,16);   // Remember to change the model parameter to suit your display module!
//UTouch      myTouch(15,10,14,9,8);

// Uncomment the next two lines for the Arduino Mega
UTouch * Button::_ptrTouch;
UTFT * Button::_ptrLcd;

void setup() {
	Serial.begin(115200);
	Serial.println("Starting Thermostat");

// Set the i/o pins used :
	ptrDisplay = new UTFT((byte) SSD1289, 38, 39, 40, 41, 0); // Remember to change the model parameter to suit your display module!
	ptrTouch = new UTouch(6, 5, 4, 3, 2);

	analogReference(EXTERNAL);
	// Initial setup
	ptrDisplay->InitLCD();
	ptrDisplay->clrScr();

	ptrTouch->InitTouch(LANDSCAPE);
	ptrTouch->setPrecision(PREC_MEDIUM);

	Button::_ptrTouch = ptrTouch;
	Button::_ptrLcd = ptrDisplay;
	defineProgramScreen();

	updateDisplayTime.Interval(5000, true);
	idleScreenTimer.Interval(10000, true);
}

void loop() {

	if (processUserInput()) {
		idleScreenTimer.Reset();
		updateProgramScreen();
		idleScreenDisplayState = 0;
	} else {
		if (idleScreenTimer.isFinishedNoRestart()) { // Do not display the idle screen while user is programming
			programScreenDisplayState = 0;
			if (updateDisplayTime.isFinished()) {
				updateIdleScreen();
			}
		}
	}
}

void updateIdleScreen() {
	bool forceRedraw = false;

	if (defineIdleScreen()) {
		forceRedraw = true;
	}

	const char* noTempSet = "::";
	if (strcmp(ptrDesiredTemp->getValue(), noTempSet) != 0) { // only show if a target is set
		//Serial.println("Not set to ::");
		ptrDesiredTemp->draw(forceRedraw);
	} else {
		//Serial.println("Did not draw - desired temp is set to ::");
	}
	ptrDesiredTemp->draw(forceRedraw);
	updateCurrentTemp();
	ptrInfoBox->draw(forceRedraw);
}

void updateProgramScreen() {
	bool forceRedraw = false;

	if (defineProgramScreen()) {
		forceRedraw = true;
	}
	updateCurrentTemp();
	updateDisplay();
	ptrModeButton->draw(forceRedraw);
	ptrInfoBox->draw(forceRedraw);
	ptrUpArrow->draw(forceRedraw);
	ptrDownArrow->draw(forceRedraw);
	ptrDesiredTemp->draw(forceRedraw);
	ptrCurrentTemp->draw(forceRedraw);
	ptrInfoBox->draw(forceRedraw);
}

void updateCurrentTemp() {

	char t[4];
	getCurrentTemp(t);
	//char t2[2];
	//t2[0] = t[2];  // the decimal part of the temperature
	//t2[1] = 0;
	t[2] = 0;  // cut off the 3rd (decimal) part of the display .
	ptrCurrentTemp->update(t);
	ptrCurrentTemp->draw();
}

void getCurrentTemp(char *buffer) {
	int reading = analogRead(sensorPin);
	/*
	 Serial.print("A to D count: [");
	 Serial.print(reading);
	 Serial.print("] ");
	 */
	float voltage = reading / 1023.0 * 1.23;
	/*
	 Serial.print(voltage * 1000);
	 Serial.print(" mV  ");
	 */
	// 1023 (max count) = 1.23v which at 20mV per degree C is a temperature of: 61.5 C so each count is about 0.06 degrees C
	float temperatureC = voltage / 0.020;  //converting from 20 mv per degree
	//Serial.print(temperatureC);
	//Serial.print(" C   ");
	float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
	//Serial.print(temperatureF);
	//Serial.print(" F  ");
	int iTemp = (int) (temperatureF * 100.0);
	iTemp += 5;  // to round up or down
	currentTemp = iTemp / 100;
	int decimalTemp = iTemp % 100;
	decimalTemp /= 10;
	//char *t = (char *) malloc(sizeof(char [4]));
	sprintf(buffer, "%d%d", currentTemp, decimalTemp);
	//Serial.print("Display:  ");
	//Serial.println(buffer);
}

bool processUserInput() {

	bool inputDetected = false;

	if (ptrModeButton->isButtonPressed()) {
		// The mode change button
		toggleMode();
		inputDetected = true;
	} else if (ptrUpArrow->isButtonPressed()) {
		// Increase the temperature
		if (targetTemp == 0) {
			// if heat / cool was off - start from the current temp.
			targetTemp = currentTemp;
		}
		if (++targetTemp > MAX_TARGET)
			targetTemp = MAX_TARGET;
		//arrowPressed=1;
		if (targetTemp > currentTemp) {
			mode = HEAT;
		}
		displayTargetTemp(targetTemp);
		inputDetected = true;
	} else if (ptrDownArrow->isButtonPressed()) {
		if (targetTemp == 0) {
			// if heat / cool was off - start from the current temp.
			targetTemp = currentTemp;
		}
		if (--targetTemp < MIN_TARGET)
			targetTemp = MIN_TARGET;

		if (targetTemp < currentTemp) {
			mode = COOL;
		}
		displayTargetTemp(targetTemp);

		inputDetected = true;
	} else if (ptrMainDisplayArea->isAnyButtonPressed()) {
		inputDetected = true;
	}

	delay(10);

	return inputDetected;
}

void toggleMode() {
	// go through off each time
	if (mode == OFF) {
		if (lastMode == HEAT) {
			mode = COOL;
		} else {
			mode = HEAT;
		}
		lastMode = mode;
	} else {
		mode = OFF;
		targetTemp = 0;
	}
}

void updateDisplay() {
	Serial.println("=== Update Display ===");
	switch (mode) {
	case OFF:
		updateProgramDisplayDetails("OFF", "Off", LCD_BLACK, LCD_WHITE, 0x73EF);
		break;
	case HEAT:
		updateProgramDisplayDetails("HEAT", "Heating", LCD_BLACK, LCD_RED, 0x73EF);
		break;
	case COOL:
		updateProgramDisplayDetails("COOL", "Cooling", LCD_BLACK, LCD_BLUE, 0x73EF);
		break;
	}
}

void updateProgramDisplayDetails(char * mode, char * info, word bg, word tx, word br) {
	Serial.println("Update Program Display ");
	Serial.println("=== Update Program Display ===");
	ptrMainDisplayArea->update(bg, bg, bg);
	ptrModeButton->update(mode, bg, tx);
	ptrCurrentTemp->update(bg, tx, br);
	ptrDesiredTemp->update(bg, tx, br);
	ptrUpArrow->update(bg, tx, br);
	ptrDownArrow->update(bg, tx, br);
	ptrInfoBox->update(bg, tx, br);
	ptrInfoBox->update(info);

}

void displayTargetTemp(int tt) {
	char t[5];
	if (tt < MIN_TARGET || tt > MAX_TARGET) {
		tt = 0;
	}
	if (tt == 0) {
		strcpy(t, "::");
	} else {
		sprintf(t, "%d", tt);
	}
	ptrDesiredTemp->update(t);
	//updateDisplay();
}

void resetScreen() {
	delete ptrModeButton;
	delete ptrProgramButton;
	delete ptrControlButton;
	delete ptrUpArrow;
	delete ptrDownArrow;
	delete ptrDesiredTemp;
	delete ptrCurrentTemp;
	delete ptrMainDisplayArea;
	delete ptrInfoBox;

	ptrModeButton = NULL;
	ptrProgramButton = NULL;
	ptrControlButton = NULL;
	ptrUpArrow = NULL;
	ptrDownArrow = NULL;
	ptrDesiredTemp = NULL;
	ptrCurrentTemp = NULL;
	ptrMainDisplayArea = NULL;
	ptrInfoBox = NULL;
}

bool defineProgramScreen() {
	if (screenMode != PROG_SCREEN) {
		resetScreen();
		screenMode = PROG_SCREEN;

		ptrDisplay->setBackColor(0);
		ptrDisplay->clrScr();	// clear the screen the first time
		Serial.println("=== Screen cleared ===");
		ptrModeButton = new Button(240, 105, 70, 25, "ABC", SmallFont, "Mode");
		ptrUpArrow = new Button(260, 10, 50, 60, UP_ARROW, arrows7, "UP Arrow");
		ptrDownArrow = new Button(260, 170, 50, 60, DOWN_ARROW, arrows7, "DOWN Arrow");
		ptrDesiredTemp = new Button(129, 0, 70, 60, "::", SevenSegNumFont, "Target Temp");
		displayTargetTemp(targetTemp);
		Serial.println("=== TT ===");
		ptrCurrentTemp = new Button(0, 0, 70, 60, "00", SevenSegNumFont, "Current Temp");
		updateCurrentTemp();
		ptrMainDisplayArea = new Button(0, 0, 319, 239, "", SmallFont, "Main Display Area");
		ptrInfoBox = new Button(0, 120, 129, 119, "Info Box", SmallFont, "Info Box");
		ptrInfoBox->update(LCD_BLACK, LCD_WHITE, LCD_LIME);
		ptrDisplay->print("Current Temp", 0, 65, 0);
		ptrDisplay->print("Target Temp", 129, 65, 0);

		// Timer clock
		Serial.println("=== Timer Clock ===");



		return true;
	} else {
		return false;
	}
}

bool defineIdleScreen() {
	if (screenMode != IDLE_SCREEN) {
		resetScreen();
		screenMode = IDLE_SCREEN;

		ptrDisplay->setBackColor(0);
		ptrDisplay->clrScr();	// clear the screen the first time
		Serial.println("=== Screen cleared ===");
		// x,y,x1,y1

		ptrDesiredTemp = new Button(200, 10, 70, 60, "", SevenSegNumFont, "Target Temp");
		//char displayOFF[5] = "OFF";
		//char displayHEAT[5] = "Heat";
		//char displayCOOL[5] = "Cool";
		char *hvacMode = "Off";
		unsigned int hvacColor = LCD_WHITE;
		switch (mode) {
		case OFF:
			ptrDesiredTemp->display(false); // don't show the button
			ptrDisplay->setColor(LCD_YELLOW);
			ptrDisplay->setFont(BigFont);
			ptrDisplay->print("Off", 215, 40, 0);
			break;
		case HEAT:
			ptrDesiredTemp->update(LCD_BLACK, LCD_RED, LCD_BLACK);
			//displayMode =displayHEAT;
			hvacMode = "Heating";
			hvacColor = LCD_RED;
			break;
		case COOL:
			ptrDesiredTemp->update(LCD_BLACK, LCD_LIME, LCD_BLACK);
			//displayMode =displayCOOL;
			hvacMode = "Cooling";
			hvacColor = LCD_BLUE;
			break;
		}

		ptrCurrentTemp = new Button(0, 10, 200, 120, "00", SevenSeg_XXXL_Num, "Current Temp");
		ptrCurrentTemp->update(LCD_BLACK, LCD_WHITE, LCD_BLACK);

		ptrMainDisplayArea = new Button(0, 0, 319, 239, "::", SmallFont, "Main Display Area");
		displayTargetTemp(targetTemp);

		ptrDisplay->setColor(LCD_YELLOW);
		ptrDisplay->print("Temperature", 50, 0, 0);

		ptrDisplay->print("Target", 225, 0, 0);

		ptrInfoBox = new Button(0, 160, 129, 79, hvacMode, SmallFont, "Info Box");
		ptrInfoBox->update(LCD_BLACK, hvacColor, LCD_LIME);

		return true;
	} else {
		return false;
	}
}

bool defineAltProgramScreen() {
	if (screenMode != ALT_SCREEN) {
		screenMode = ALT_SCREEN;

		ptrDisplay->setBackColor(0);
		ptrDisplay->clrScr();	// clear the screen the first time
		Serial.println("=== Screen cleared ===");
		//ptrIdleScreenButton = new Button(0, 0, 70, 25, "", SmallFont);
		ptrModeButton = new Button(240, 105, 70, 25, "ABC", SmallFont, "Mode");
		ptrProgramButton = new Button(240, 5, 70, 25, "PROGRAM", SmallFont, "Program");
		ptrControlButton = new Button();
		ptrUpArrow = new Button(260, 10, 50, 60, UP_ARROW, arrows7, "UP Arrow");
		ptrDownArrow = new Button(260, 170, 50, 60, DOWN_ARROW, arrows7, "DOWN Arrow");
		ptrDesiredTemp = new Button(129, 120, 70, 60, "::", SevenSegNumFont, "Target Temp");
		ptrCurrentTemp = new Button(0, 0, 200, 120, "00", SevenSeg_XXXL_Num, "Current Temp");
		ptrMainDisplayArea = new Button(0, 0, 319, 239, "", SmallFont, "Main Display Area");
		ptrInfoBox = new Button(0, 120, 129, 119, "Info Box", SmallFont, "Info Box");
		/*ptrModeButton->add(240, 45, 70, 25, "OFF",SmallFont );
		 ptrProgramButton->add(240, 5, 70, 25, "PROGRAM",SmallFont );
		 ptrUpArrow->add(240, 90, 50, 60, "0",arrows7 );
		 ptrDownArrow->add(240, 170, 50, 60, "1",arrows7 );

		 ptrDesiredTemp->add(5,5,100,90,"::",SevenSegNumFont);
		 ptrCurrentTemp->add(5,100,150,138,"00",SevenSeg_XXXL_Num);
		 */
		ptrUpArrow->update(LCD_BLUE, LCD_YELLOW);
		ptrDownArrow->update(LCD_BLUE, LCD_YELLOW);
		ptrDesiredTemp->update(LCD_BLACK, LCD_WHITE, LCD_BLACK);
		ptrCurrentTemp->update(LCD_BLACK, LCD_LIME, LCD_BLACK);
		ptrInfoBox->update(LCD_BLUE, LCD_YELLOW, LCD_BLACK);
		// ptrDesiredTemp->update(LCD_BLACK,LCD_WHITE,LCD_BLACK);
		// ptrCurrentTemp->update(LCD_BLACK,LCD_LIME,LCD_BLACK);

		ptrUpArrow->draw();
		ptrDownArrow->draw();
		ptrDesiredTemp->draw();
		ptrCurrentTemp->draw();
		ptrInfoBox->draw();
		return true;
	} else {
		return false;
	}
}
