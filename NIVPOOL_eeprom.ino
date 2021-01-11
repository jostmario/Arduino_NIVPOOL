// =====================================================================
//
// EEPROM Funktionen zum Speichern der Zeiten, etc. vom NIVPOOL.
//
// =====================================================================

// Integer zahl in eeprom schreibben und lesen



void eepromWriteInt(int adr, int wert) {
byte low, high;
  low=wert&0xFF;
  high=(wert>>8)&0xFF;
  EEPROM.write(adr, low); // dauert 3,3ms 
  EEPROM.write(adr+1, high);
  return;
} //eepromWriteInt


int eepromReadInt(int adr) {
byte low, high;
  low=EEPROM.read(adr);
  high=EEPROM.read(adr+1);
  return low + ((high << 8)&0xFF00);
} //eepromReadInt
