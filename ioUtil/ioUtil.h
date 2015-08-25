/* $URL: svn+ssh://aaron@birenboim.com/home/svn/arduino/sketchbook/libraries/ioUtil/ioUtil.h $
$Id: ioUtil.h 39 2012-02-04 01:36:14Z aaron $
*/

#ifndef SERIAL_BUF_LEN
#define SERIAL_BUF_LEN 63  // default buf len
#endif

class SerialLineBufferPrivates;
class SerialLineBuffer
{
public:
  SerialLineBuffer();
  //~SerialLineBuffer();
  bool isComplete();  // reads from serial, return true if 0 or EOLN
  void clear();
  void begin();
  int length() const;
  int maxLength() const;
  char *get();  // retrieve current buffer and clear
  char buf[SERIAL_BUF_LEN+1];
protected:
  int _maxLength, _len;
  bool _complete;
private:
  //class SerialLineBufferPrivates *Priv;
};


char lowCase(const char a);
int caseCmp(const char a0, const char b0);
char *extractKey(char *cmdStr, char **val);
bool keyMatch(const char *key, const char *key1);
