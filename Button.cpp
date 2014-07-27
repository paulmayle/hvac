#include "Button.h"

Button::Button() {
	_x = 0;
	_y = 0;
	_l = 100;
	_h = 100;
	_ty = 0;
	_tx = 0;
	_font = SmallFont;
	commonInit();
	char * n = "NOT DEFINED";
	_name = (char *) malloc(strlen(n) + 1);
	strcpy(_name, n);
}

Button::Button(int x, int y, int l, int h, char *name, uint8_t *font, char *debugName) {

	_x = x;
	_y = y;
	_l = l;
	_h = h;
	_font = font;
	_debugName = debugName;

	_name = (char *) malloc(strlen(name) + 1);
	strcpy(_name, name);
	_ptrLcd->setFont(font);
	commonInit();
	// calculate y position
	int ySpace = h - _ptrLcd->getFontYsize();
	_ty = y + (ySpace / 2) + 1;
	// calculate x position
	int nameLength = strlen(name) * _ptrLcd->getFontXsize();
	int xSpace = l - nameLength;
	_tx = x + (xSpace / 2);

	Serial.print("Button: ");
	Serial.print(_debugName);
	Serial.print("  Constructor: Name [");
	Serial.print(name);
	Serial.print("]     @ [");
	Serial.print(_x);
	Serial.print(",");
	Serial.print(_y);
	Serial.print("] to [");
	Serial.print(_x1);
	Serial.print(",");
	Serial.print(_y1);
	Serial.print("] Text @ [");
	Serial.print(_tx);
	Serial.print(",");
	Serial.print(_ty);
	Serial.println("]");
}




void Button::commonInit() {

	_buttonColor = VGA_BLACK;
	_textColor = VGA_YELLOW;
	_borderColor = VGA_YELLOW;
	_changed = true;

	_x1 = _x + _l;
	_y1 = _y + _h;

	_flashColor = VGA_RED;
	_displayButton=true;
}

void Button::display(bool showButton) {

		_displayButton=showButton;

}

void Button::add(int x, int y, int l, int h, char *name, uint8_t *font) {
	// add a new button to the screen at location x,y with size of l,h
	Serial.print("In Add [");
	Serial.print(name);
	Serial.println("] ");

	_x = x;
	_y = y;
	_l = l;
	_h = h;
	_x1 = x + l;
	_y1 = y + h;

	_font = font;
	//  _name=name;
	Serial.print("About to free Name [");
	Serial.print(name);
	Serial.println("] ");

	free(_name);
	_name = (char *) malloc(strlen(name) + 1);
	strcpy(_name, name);
	Serial.print("Malloc Done  [");
	Serial.print(name);
	Serial.println("] ");

	_ptrLcd->setFont(font);

	// calculate y position

	int ySpace = h - _ptrLcd->getFontYsize();
	_ty = y + (ySpace / 2) + 1;
	// calculate x position
	int nameLength = strlen(name) * _ptrLcd->getFontXsize();
	int xSpace = l - nameLength;
	_tx = x + (xSpace / 2);

	Serial.print("Add [");
	Serial.print(name);

	Serial.print("] @ [");
	Serial.print(_x);
	Serial.print(",");
	Serial.print(_y);
	Serial.print("] to [");
	Serial.print(_x1);
	Serial.print(",");
	Serial.print(_y1);
	Serial.print("] Text @ [");
	Serial.print(_tx);
	Serial.print(",");
	Serial.print(_ty);
	Serial.println("]");
}

void Button::draw(boolean force) {
	if(force){	// only set the changed flag to true (never over write it with false!)
	_changed = force;
	}
	draw();
}
void Button::draw() {


	if (_changed && _displayButton) {
		Serial.print("Draw: ");
		Serial.print(_debugName);
		Serial.print("  [");

		Serial.print(_name);
		Serial.print("] @ [");
		Serial.print(_x);
		Serial.print(",");
		Serial.print(_y);
		Serial.print("] to [");
		Serial.print(_x1);
		Serial.print(",");
		Serial.print(_y1);
		Serial.print("] Text @ [");
		Serial.print(_tx);
		Serial.print(",");
		Serial.print(_ty);
		Serial.println("]");

		// add a new button to the screen at location x,y with size of l,h
		_ptrLcd->setFont(_font);
		uint8_t xf = _ptrLcd->getFontXsize();
		uint8_t yf = _ptrLcd->getFontYsize();

		_ptrLcd->setBackColor(_buttonColor);
		_ptrLcd->setColor(_buttonColor);
		_ptrLcd->fillRect(_x, _y, _x1, _y1);
		_ptrLcd->setColor(_borderColor);
		_ptrLcd->drawRoundRect(_x, _y, _x1, _y1);
		_ptrLcd->setColor(_textColor);
		_ptrLcd->print(_name, _tx, _ty);
		_changed = false;
	} else {

	//	Serial.println(" nothing changed so do not update ");

	}
}

char * Button::getValue() {
	return _name;
}

void Button::update(char *txt, word buttonColor, word textColor) {
	if (_buttonColor != buttonColor) {
		_buttonColor = buttonColor;
		_changed = true;
	}
	if (_textColor != textColor) {
		_textColor = textColor;
		_changed = true;
	}
	update(txt);
}

void Button::update(word buttonColor, word textColor, word borderColor) {
	if (_borderColor != borderColor) {
		_borderColor = borderColor;
		_changed = true;
	}
	update(buttonColor, textColor);
}

void Button::update(word buttonColor, word textColor) {
	if (_buttonColor != buttonColor) {
		_buttonColor = buttonColor;
		_changed = true;
	}
	if (_textColor != textColor) {
		_textColor = textColor;
		_changed = true;
	}
}

void Button::update(char *t) {

	if (strcmp(t, _name)) {
		//Serial.println("Name changed  ");
		Serial.print("Button: ");
		Serial.print(_debugName);
		Serial.print("  Update: [");
		Serial.print(_name);
		Serial.print("] to-> [");
		Serial.print(t);
		Serial.println("] ");
		_changed = true;
		free(_name);
		_name = (char *) malloc(strlen(t) + 1);
		strcpy(_name, t);
	} else {
		//Serial.println(" No change ");
	}
}

void Button::flash() {
	_ptrLcd->setFont(_font);
	uint8_t xf = _ptrLcd->getFontXsize();
	uint8_t yf = _ptrLcd->getFontYsize();
	_ptrLcd->setBackColor(_flashColor);
	_ptrLcd->setColor(_flashColor);
	_ptrLcd->fillRect(_x, _y, _x1, _y1);
	_ptrLcd->setColor(_textColor);
	_ptrLcd->drawRoundRect(_x, _y, _x1, _y1);
	_ptrLcd->setColor(_textColor);
	_ptrLcd->print(_name, _tx, _ty);
	_changed = true;
}

boolean Button::isButtonPressed() {
	// Serial.print("Checking button press .....");

	if (_ptrTouch->dataAvailable()) {
		_ptrTouch->read();
		int x = _ptrTouch->getX();
		int y = _ptrTouch->getY();
		/*
		Serial.print("Button: ");
		Serial.println(_debugName);
		Serial.print("Screen press at x=");
		Serial.print(x);

		Serial.print(" [");
		Serial.print(_x);
		Serial.print("-");
		Serial.print(_x1);
		Serial.print("] y=");
		Serial.print(y);

		Serial.print(" [");
		Serial.print(_y);
		Serial.print("-");
		Serial.print(_y1);

		Serial.print("] ");
*/
		// check the boundries of our button
		if (x >= _x && x <= _x1 && y >= _y && y <= _y1) {
			Serial.print("Button: [ ");
			Serial.print(_debugName);
			Serial.println(" ] was pressed)");
			flash();
			delay(300);
			draw();
			return true;
		} else {
			//Serial.println("... no, not for me :(");
			return false;
		}
	} else {
		// Serial.println("... nothing pressed.");
		return false;
	}
}

boolean Button::isAnyButtonPressed() {
	 //Serial.print("Checking button press .....");

	if (_ptrTouch->dataAvailable()) {
		_ptrTouch->read();
		int x = _ptrTouch->getX();
		int y = _ptrTouch->getY();
		/*
		Serial.print("Screen press at x=");
		Serial.print(x);

		Serial.print(" [");
		Serial.print(_x);
		Serial.print("-");
		Serial.println(_x1);
		Serial.print("] y=");
		Serial.print(y);

		Serial.print(" [");
		Serial.print(_y);
		Serial.print("-");
		Serial.println(_y1);

		Serial.print("]");
*/
			return true;

	} else {
		// Serial.println("... nothing pressed.");
		return false;
	}
}


