/**
 * SimpleDebug.h - Library for basic debugging features
 * (c) 2018 Christian Grieger
 * GNU GENERAL PUBLIC LICENSE
 */

#ifndef SIMPLEDEBUG_H
#define SIMPLEDEBUG_H

  #ifdef _SIMPLEDEBUG
    void debugWait(void)
    {
      // empty serial buffer
      char ch;
      while (Serial.available()) {
        ch = Serial.read();
      }
      ch = 0;

    Serial.println(F("DEBUG: Press 'c' to continue."));

    // wait for keypress "c"
    do {
      if (Serial.available() > 0) {
        ch = Serial.read();
      }
    } while (ch != 'c');

    // empty serial buffer
    while (Serial.available()) {
      ch = Serial.read();
    }
    }

    #define DEBUG_INIT(speed) Serial.begin(speed)
    #define DEBUG_PRINT(val) Serial.print("DEBUG: " + String(val))
    #define DEBUG_PRINTLN(val) Serial.println("DEBUG: " + String(val))
    #define DEBUG_DELAY(ms) delay(ms)
    #define DEBUG_WAIT(condition) if (condition) debugWait()
  #else
    // Empty macros bodies if debugging is not needed
    #define DEBUG_INIT(speed)
    #define DEBUG_PRINT(val)
    #define DEBUG_PRINTLN(val)
    #define DEBUG_DELAY(ms)
    #define DEBUG_WAIT(condition)
  #endif

#endif
