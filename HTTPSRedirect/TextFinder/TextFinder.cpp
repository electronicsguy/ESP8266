/*
  TextFinder.cpp - a simple text parser for Arduino data streams
  Copyright (c) 2009 2010 Michael Margolis.  All right reserved.
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
 
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
 
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
   Version 1.1 - 21 August 2010 : added conditional compile for Stream class
 */
#include "TextFinder.h"

#define NO_SKIP_CHAR  1  // a magic char not found in a valid numeric field

#if ARDUINO < 19
// the constructors allow one and only one of the streams to be initialized
  // constructors for versions prior to 19
  
  //Ethernet
  TextFinder::TextFinder(Client &stream, int timeout) :
                  clientStream(&stream), serialStream(NULL) 
  { 
    this->timeout = timeout * 1000L;     
  }

  //Serial
  TextFinder::TextFinder(HardwareSerial &stream, int timeout) : 
                           clientStream(NULL), serialStream(&stream)
  {
    this->timeout = timeout * 1000;     
  }
  
  int TextFinder::myRead()
  {   
    startMillis = millis();  
    if (clientStream != NULL)
    {     
      while(millis() < (startMillis + timeout)) 
      {
        if (clientStream->available() > 0)
          return clientStream->read();           
      }
    }
    else // serialStream must be valid
    {
      while(millis() < (startMillis + timeout)) 
      {
        if (serialStream->available() > 0)
          return serialStream->read();                   
      }   
    } 
    return 0;     // 0 indicates timeout     
  }

#else
  // this version is for Arduino versions that have Serial and Ethernet derived from stream
  TextFinder::TextFinder(Stream &stream, int timeout) :
                  streamer(&stream)          
  { 
    this->timeout = timeout * 1000L;     
  }

  int TextFinder::myRead()
  {   
    startMillis = millis();  
    if (streamer != NULL)
    {     
      while(millis() < (startMillis + timeout)) 
      {
        if (streamer->available() > 0)
		{
//#define DEBUG_OUTPUT			
#ifdef DEBUG_OUTPUT		
		 char c = streamer->read();
		 Serial.print(c);
         return c;
#endif		   
         return streamer->read(); 
		  
		}
         		  
      }
    }
    return 0;     // 0 indicates timeout     
  }
#endif
  
  // public methods
  //
  // find returns true if the target string is found
  boolean  TextFinder::find(char *target)
  {
    return findUntil(target, NULL);
  }
 
  // as above but search ends if the terminate string is found
  boolean  TextFinder::findUntil(char *target, char *terminate)
  {
    byte targetLen = strlen(target); 
    byte index = 0;  // maximum target string length is 255 bytes
    byte termIndex = 0;
    byte termLen = (terminate == NULL ? 0 : strlen(terminate));
    char c;
 
    if( *target == 0)
       return true;   // return true if target is a null string 
    while( (c = myRead()) != 0){   
      if( c == target[index]){        
        if(++index >= targetLen){ // return true if all chars in the target match
          return true;    
        }
      }
      else{        
        index = 0;  // reset index if any char does not match   
      }
      if(termLen > 0 && c == terminate[termIndex]){
         if(++termIndex >= termLen)  
           return false;       // return false if terminate string found before target string
      }
      else
          termIndex = 0;
    }
    return false;
  }

    // places the string between the prestring  and poststring in the given buffer
    // buffer must be one more than the longest string to get
    // the string will be truncated to fit the buffer length
    // end of string determined by a single character match to the first char of poststring
    // returns the number of characters placed in the buffer (0 means no valid data found)
  int TextFinder::getString( char *pre_string, char *post_string, char *buffer, int length)
  {  
    if( find(pre_string) ){ 
      int index = 0;   
      *buffer = 0;
      while(index < length-1 ){
        char c = myRead();
        if( c == 0 ){
          return 0;   // timeout returns 0 !
        }
        else if( c == *post_string ){
          buffer[index] = 0; // terminate the string
          return index;               // data got successfully
        }
        else{
          buffer[index++] = c;   
        }
      }
      buffer[index] = 0;
      return index; // Note: buffer full before the closing post_string encountered
    }   
    return 0;    //failed to find the prestring
  } 

  // getValue method:
  // returns the first valid (long) integer value from the current position.
  // initial characters that are not digits (or the minus sign) are skipped
  // function is terminated by the first character that is not a digit.
  long TextFinder::getValue()
  {
      return getValue(NO_SKIP_CHAR); // terminate on first non-digit character 
  }
  
  // as above but a given skipChar is ignored
  // this allows format characters (typically commas) in values to be ignored
  long TextFinder::getValue(char skipChar)
  {
      boolean isNegative = false;
      long value = 0;   
      char c;

      while( (c = myRead()) != '-' && (c < '0' || c > '9') )
         ;// ignore non numeric leading characters  
      do{ 
        if(c == skipChar)
           ; // ignore this charactor        
        else if(c == '-') 
           isNegative = true;          
        else if(c >= '0' && c <= '9')        // is c a digit?  
           value = value * 10 + c - '0';         
        c = myRead();        
      }
      while( (c >= '0' && c <= '9')  || c == skipChar );
      if(isNegative)
        value = -value;   
     return value;
  }

  // float version of getValue method:
  // as integer version but returns a floating point value
    float TextFinder::getFloat()
    {
       getFloat(NO_SKIP_CHAR);
    }

  // as above but the given skipChar is ignored
  // this allows format characters (typically commas) in values to be ignored  
  float TextFinder::getFloat(char skipChar){
      boolean isNegative = false;
      boolean isFraction = false;
      long value = 0;  
      float fValue;       
      char c;
      float fraction = 1.0;

      while( (c = myRead()) != '-' && (c < '0' || c > '9') )
         ; // ignore non numeric leading characters
      do{    
        if(c == skipChar)
           ; // ignore 
        else if(c == '-') 
           isNegative = true;     
        else if (c == '.')
            isFraction = true;
        else if(c >= '0' && c <= '9')  {      // is c a digit?  
           value = value * 10 + c - '0'; 
           if(isFraction)
              fraction *= 0.1;
        }           
        c = myRead();     
      }
      while( (c >= '0' && c <= '9')  || c == '.' || c == skipChar );
      if(isNegative)
        value = -value;      
      if(isFraction)
        return value * fraction;        
      else  
        return value;
  }
