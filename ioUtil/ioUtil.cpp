/* $URL: svn+ssh://aaron@birenboim.com/home/svn/arduino/sketchbook/libraries/ioUtil/ioUtil.cpp $
$Id: ioUtil.cpp 40 2012-02-04 03:07:54Z aaron $
*/
//#define SERIAL_BUF_LEN 127  // change buf size if desired
#include <ioUtil.h>

#define NULL 0

// don't want to depend on ctype.h, just for this!
bool isBlank(int c)
{
  if(c == 7) return(false); // bell
  return( (c <= ' ') || (c > '~') );
}

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#warning ARDUINO
#else
#error ARDUINO not >= 100
#include <WProgram.h>
#endif

//#include <stdlib.h> // malloc.  No proper new/delete for AVR


void SerialLineBuffer::begin()
{
  //Serial.println("initializing serial command buffer");
  _maxLength = SERIAL_BUF_LEN;  // AVR dynamic mem is tricky
  _len = 0;
  _complete = false;
  ////if (buf == NULL) Serial.println("Error allocating serial command buffer");
}
SerialLineBuffer::SerialLineBuffer() { begin(); }
//SerialLineBuffer::~SerialLineBuffer(){ free(buf); free(Priv); }

bool isTerminator(int c)
{
  if (c == 0) return(true);
  if (c == ';') return(true);  // sending \n to serial is tricky.  accept this too.
  //if ((c=='\n') || (c=='\r') || (c=='\m')) return(true);
  if ((c>=10) && (c<=13)) return(true);  // \r, \n, form feed, vert tab
  return(false);
}

/// read from serial, return true if 0 or EOLN
bool SerialLineBuffer::isComplete()
{
  if (_complete)
    return(true);  // don't read more until this line is consumed

  // add characters from serial
  while(Serial.available() > 0)
    {
      int nextByte = Serial.read();
      //Serial.print("Got ");Serial.println(nextByte);
      if ((nextByte < 0) || (nextByte >= 256))
        return(_complete);
      if (isTerminator(nextByte))
        {
	  //Serial.print("terminator ");Serial.println(nextByte);
          buf[_len] = 0;
          _complete = (_len > 0);
          return(_complete);
        }

      if (isBlank(nextByte))
        {
	  //Serial.print("blank ");Serial.println(nextByte);
          if (_len > 0) // ignore leading whitespace
            {
              if (buf[_len-1] != ' ')  // compact space to 1 space
		{
		  buf[_len++] = ' '; // convert all space to ' '
		}
            }
        }
      else
	{
	  buf[_len++] = (char)nextByte;
	}

      // don't allow overflow
      if (_len >= _maxLength)
	{
	  Serial.println("\nOverflow.  truncating command string");
	  _complete = true;
	}
    }
  return(_complete);
}

void SerialLineBuffer::clear()
{
  _len = 0;
  _complete = false;
}
int SerialLineBuffer::length() const { return(_len); }
int SerialLineBuffer::maxLength() const { return(_maxLength); }

/// retrieve current buffer and clear
char *SerialLineBuffer::get()
{
  buf[_len]=0;
  clear();
  return(buf);
}

//-----------------------------------------------------------
/// split a keyword-value pair string into a key string and value string
const char nullChar = 0;  // static is scary on AVR
char *extractKey(char *cmdStr, char **val)
{
  *val = (char *)&nullChar;
  if (cmdStr == NULL) return(NULL);
  char *key = cmdStr;
  while (*key)  // process comments
    {
      if (*key == '#') *key=0;  // comment
      else key++;
    }
  key = cmdStr;
  while(*key &&  isBlank(*key)) key++; // trim leading space
  *val = key;
  while(**val && !isBlank(**val)) *val += 1; // skip key
  **val = 0;
  *val += 1;
  while(**val &&  isBlank(**val)) *val += 1; // skip whitespace
  return(key);
}

char lowCase(const char a)
{
  if ((a >= 'A') && (a <= 'Z'))
    {
      int sft = ((int)'a') - ((int)'A');
      int b = (int)a + sft;
      return((char)b);
    }
  return(a);
}

int caseCmp(const char a0, const char b0)
{
  char a = lowCase(a0);
  char b = lowCase(b0);
  if (a < b) return(-1);
  return((a>b)?1:0);
}

bool keyMatch(const char *key0, const char *key1)
{
  //Serial.print("keyMatch(");Serial.print(key0);Serial.print(",");Serial.print(key1);Serial.print(")=");
  while(*key0 || *key1)
    {
      if (caseCmp(*key0, *key1))
      {
        //Serial.println("false");
        return(false);
      }
      if (*key0) key0++;
      if (*key1) key1++;
    }
  //Serial.println("true");
  return(true);
}
