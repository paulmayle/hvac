#ifndef Button_h
#define Button_h


#include "Arduino.h"
#include <UTFT.h>
#include <UTouch.h>


extern uint8_t SmallFont[];

class Button
{
public:
  Button();
  Button(UTFT *, UTouch *);
  Button(int x, int y, int l, int h, char *name, uint8_t *, char *debugName);
  void add(int x, int y, int l, int h, char *name, uint8_t *);
  void draw();
  void display(bool);
  void draw(boolean);
  void update(word, word);
  void update(word, word, word);
  void update(char*, word, word);
  void update(char*);
  char * getValue();
  //void draw(char*,boolean);

  //   void update(word, word);
  // void update(word, word, word);

  void flash();
  boolean isButtonPressed();
  boolean isAnyButtonPressed();
  static UTFT _lcd;
  static UTFT *_ptrLcd;
  static UTouch *_ptrTouch;

private:

  void commonInit();
  boolean _changed;
  int _x;
  int _y;
  int _l;
  int _h;
  int _x1;
  int _y1;
  int _tx;
  int _ty;
  char *_name;
  char *_debugName;
  uint8_t * _font;

  word _buttonColor;
  word _textColor;
  word _flashColor;
  word _borderColor;

bool _displayButton;
};

#endif




