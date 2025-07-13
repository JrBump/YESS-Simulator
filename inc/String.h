#include <string>

#ifndef STRING_H
#define STRING_H
//This is our own implementation of a String
//class.  We can create a String object out of a c-string
//or a std::string.

class String
{
   private:
      char * str;     //constructor dynamically allocates array of characters
      int32_t length; //length of array
    
      //methods only used by other methods in this file
      bool badIndex(int32_t idx);
      bool isHex(char hexChar);
   public:
      String(std::string str);
      char * getCstr();
      std::string getStdstr();
      int32_t getLength();
      bool isChar(char what, int32_t idx);
      bool isNChar(char what, int32_t sIdx, int32_t len);
      bool isNHex(int32_t idx, int32_t len);
      uint32_t convert1Hex(int32_t idx);
      uint32_t convertNHex(int32_t sIdx, int32_t len);
      bool isSubString(const char * subStr, int32_t idx);
      bool isSubString(std::string subStr, int32_t idx);
};
#endif
