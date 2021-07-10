#include <EEPROM.h>

void setup() {
  Serial.begin(9600);
  EEPROM.begin(32);  //Syntax: EEPROM.begin(Size)
  // You need to call EEPROM.begin(size) before you can start reading or writing, where the size parameter is the number of bytes you want to use store
  // Size can be anywhere between a minimum of 4 and maximum of 4096 bytes.
}

void loop() {
  // 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 // 32-bytes of random data
  // EEPROM.put(eeAddress, variable_name);    // Writes a variable type e.g. float = 4-bytes, int = 4-bytes, boolean = 1 byte
  // EEPROM.get(eeAddress, variable_name);    // Reads a variable type  e.g. float = 4-bytes, int = 4-bytes, boolean = 1 byte
  // int value = EEPROM.read(eeAddress);      // Read a single byte
  // EEPROM.write(eeAddress, variable_name);  // Write a single byte
  // EEEPROM.commit() commands such as EEPROM.write or EEPROM.put do not write to flash immediately, to invoke them you must call EEPROM.commit() to save changes to flash/EEPROM
  // EEPROM.end() will also commit, but releases the RAM copy of EEPROM contents.
 
  // Commands to determine variable sizes, needed for storing to EEPROM
  Serial.println("            Floating point variables need: "+String(sizeof(float))+" Bytes");  // Determine how many bytes (4) are needed to save a floating point variable
  Serial.println("Double size floating point variables need: "+String(sizeof(double))+ " Bytes"); // Determine how many bytes (8) are needed to save a floating point variable
  Serial.println("                   Integer variables need: "+String(sizeof(int))+" Bytes");    // Determine how many bytes (4) are needed to save an integer variable
  Serial.println("         Boolean values or variables need: "+String(sizeof(bool))+" Bytes");   // Determine how many bytes (1) are needed to save a boolean variable
  Serial.println("           String variables need at least: "+String(sizeof(String))+" Bytes"); // Determine how many bytes (min. 12) are needed to save a string variable
  Serial.println();
  
  //----------------------------------------------------------------------------  
  // Example-1 Write a value to EEPROM at address = 0
  int EEaddress = 0;
  EEPROM.write(EEaddress,123); // Writes the value 123 to EEPROM
  // 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 // Contents of EEPROM
  // 7B 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 // Contents of RAM copy of EEPROM
  EEPROM.commit();
  // 7B 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 // Contents of EEPROM
  Serial.print("EEPROM contents at Address=0 is  : ");
  Serial.println(EEPROM.read(EEaddress));
  
  //----------------------------------------------------------------------------  
  // Example-2 Write a value to EEPROM at address = 0
  // 7B 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 // Contents of EEPROM
  EEPROM.write(EEaddress,257); // Writes the value 257 to EEPROM
  EEPROM.commit();
  // 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 // Contents of EEPROM
  Serial.print("EEPROM contents at Address=0 is  : ");
  Serial.println(EEPROM.read(EEaddress));
  
  //----------------------------------------------------------------------------  
  // Example-3 Write an integer variable to EEPROM at address = 0
  int integer_variable = 257;
  // 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 // Contents of EEPROM
  EEPROM.put(EEaddress,integer_variable); // Writes the value 257 to EEPROM
  EEPROM.commit();
  // 01 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 // Contents of EEPROM
  Serial.print("EEPROM contents at Address=0 is  : ");
  integer_variable = 0; // To prove it read from EEPROM!
  EEPROM.get(EEaddress,integer_variable);
  Serial.println(integer_variable);
  
  //----------------------------------------------------------------------------  
  // Example-4 Write another integer variable to EEPROM
  int integer_variable2 = 1234;
  // 01 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 // Contents of EEPROM
  EEaddress = EEaddress + sizeof(int); // Moves the address along by 4
  EEPROM.put(EEaddress,integer_variable2); // Writes the value 1234 to EEPROM
  EEPROM.commit();
  // 01 01 00 00 D2 04 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 // Contents of EEPROM
  Serial.print("EEPROM contents at Address=4 is  : ");
  integer_variable2 = 0; // To prove it read from EEPROM!
  EEPROM.get(EEaddress,integer_variable2);
  Serial.println(integer_variable2);
  EEaddress = EEaddress + sizeof(int); // Moves the address along by 4
 
  //----------------------------------------------------------------------------  
  // Example-5 Write a floating point variable to EEPROM
  float floatingpoint_variable = 3.141592654;
  EEPROM.put(EEaddress,floatingpoint_variable); // Writes the value 3.141592654 to EEPROM
  EEPROM.commit();
  Serial.print("EEPROM contents at Address=8 is  : ");
  floatingpoint_variable = 0; // To prove it read from EEPROM!
  EEPROM.get(EEaddress,floatingpoint_variable);
  Serial.println(floatingpoint_variable,8);
  EEaddress = EEaddress + sizeof(float);        // Moves the address along by 4

  //----------------------------------------------------------------------------  
  // Example-6 Write a string variable to EEPROM
  String string_variable = "Hello world";
  EEPROM.put(EEaddress,string_variable); // Writes the value 3.141592654 to EEPROM
  EEPROM.commit();
  Serial.print("EEPROM contents at Address=12 is : ");
  floatingpoint_variable = 0; // To prove it read from EEPROM!
  EEPROM.get(EEaddress,string_variable);
  Serial.println(string_variable);
  EEaddress = EEaddress + sizeof(string_variable);        // Moves the address along by 4

  //----------------------------------------------------------------------------  
  // Example-7 Write a series of values to EEPROM
  for (int i = 1000; i <= 1032; i = i + 4) {
    EEPROM.put(i-1000,i); // Address range 0-32
  }
  EEPROM.commit();
  for (int j = 1000; j <= 1032; j = j + 4) {
    EEPROM.get((j-1000),integer_variable); // Read the 32 values
    Serial.println(integer_variable);
  }

  //----------------------------------------------------------------------------  
  // Example-8 Testing that the EEPROM extent has not been exceeded, remember not to exceed address space
  if (EEaddress == 32) {
    EEaddress = 0;
  }

  //----------------------------------------------------------------------------  
  // Example-9 Compact method of writing and reading values from EEPROM
  EEaddress = 20;
  // Writing
  floatingpoint_variable = 2 * PI;
  EEaddress += EEPROM.put(EEaddress, floatingpoint_variable);
  integer_variable = 123456789;
  EEaddress += EEPROM.put(EEaddress, integer_variable);
  EEPROM.end();

  EEaddress = 20;
  // Reading
  EEaddress += EEPROM.get(EEaddress, floatingpoint_variable);
  EEaddress += EEPROM.get(EEaddress, integer_variable);
  EEPROM.commit();
  Serial.println(floatingpoint_variable,7);
  Serial.println(integer_variable);

  delay(10000);
}

