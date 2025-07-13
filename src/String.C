#include <cstdint>
#include <string> //for std::string functions
#include <string.h> //for c-string functions (strlen, strcpy, ..)
#include <assert.h>
#include "String.h"

/*
 * String
 *
 * Constructs a String object out of a std::string 
 * This constructor would also be called on a c-str
 *
 * Modifies: str and length data members
 */
String::String(std::string str)
{
   assert(str.length() > 0);
   length = str.length();
   this->str = new char[length];
   for (int32_t i = 0; i < length; i++) this->str[i] = str[i];    
}

/*
 * getCstr
 *
 * Builds and returns a c-str from the str array of characters.
 */
char * String::getCstr()
{
   char * newStr = new char[length + 1];
   for (int32_t i = 0; i < length; i++) newStr[i] = str[i];    
   newStr[length] = '\0';
   return newStr;
}

/*
 * getStdstr
 *
 * Build a return a std::string from the array of characters.
 */
std::string String::getStdstr()
{
   std::string newStr;
   for (int32_t i = 0; i < length; i++) newStr += str[i];    
   return newStr;
}

/*
 * getLength
 *
 * return the value of the length data member
 */
int32_t String::getLength()
{
   return length;
}

/*
 * badIndex
 *
 * Returns true if this index into the str array is
 * invalid (negative or greater than array size)
 */
bool String::badIndex(int32_t idx)
{
   if (idx < 0 || idx >= length) return true;
   return false;
}

/*
 * isHex
 * Returns true if the character parameter is hex.
 * Hex characters: '0', ... '9', 'a' ... 'f', 'A', ... 'F'
 */
bool String::isHex(char hexChar)
{
   if ((hexChar >= '0' && hexChar <= '9') ||
       (hexChar >= 'a' && hexChar <= 'f') ||
       (hexChar >= 'A' && hexChar <= 'F'))
      return true;
   return false;
} 

/*
 * isChar
 * Returns true if str[idx] is equal to what
 *
 * Three cases are possible:
 * 1) returns false if idx is invalid
 * 2) returns true if idx is valid and str[idx] is equal to what
 * 3) returns false if idx is valid and str[idx] is not equal to what
 */
bool String::isChar(char what, int32_t idx)
{
   if (badIndex(idx)) return false;
   return str[idx] == what;
}

/*
 * isNChar
 *
 * Returns true if the characters in the array from
 * startIdx to endIdx are all the character what.
 *
 * Three cases are possible:
 * 1) returns false if indices are invalid
 * 2) returns false if indices are valid and the characters in 
 *    the array at the specified indices are not all the character
 *    what (for example, not all the character ' ')
 * 3) returns true if indices are valid and the characters in the
 *    array at the specified indices are all the character what
 *    (for example, are all the character ' ')
 */

bool String::isNChar(char what, int32_t startIdx, int32_t len)
{
   //calculate the ending index based upon starting index and len
   //use badIndex method to check starting and ending index
   int32_t i;
   int32_t endIdx = startIdx + len - 1;
   if (badIndex(startIdx) || badIndex(endIdx) || startIdx > endIdx) 
   { 
      return false;
   }
   for (i = startIdx; i <= endIdx; i++) 
   {
      if (str[i] != what) return false;
   }
   return true;
}

/*
 * isNHex
 * Returns true if the sequence of len characters starting at index 
 * idx are hex
 *
 * Three cases are possible:
 * 1) returns false if starting or ending index is invalid
 * 2) returns false if starting and ending indices are valid and
 *    any of the characters in the range are not hex
 * 3) returns true if starting and ending indices are valid and
 *    all of the characters in the range are hex
 */
bool String::isNHex(int32_t startIdx, int len)
{
   //calculate the ending index based upon starting index and len
   //use badIndex method to check starting and ending index
   int endIdx = startIdx + len - 1;
   int i;
   if (badIndex(startIdx) || badIndex(endIdx) || startIdx > endIdx)
   {
      return false;
   }
   for (i = startIdx; i <= endIdx; i++)
   {
      if (!isHex(str[i])) return false;
   }
   return true;
}

/*
 * convert1Hex
 *
 * Takes as input an index to the str array. If that character is a hex
 * char, it returns the equivalent hex value. For example, if it is 'a',
 * it returns 0xa (or 10).
 *
 * Three cases are possible:
 * 1) returns 0xff if the index is invalid
 * 2) returns 0xff if the index is valid and the character
 *    in the str array is not a valid hex char
 * 3) returns the numeric equivalent if the character in the str array
 *    is a valid hex char (and the index is valid)
 */
uint32_t String::convert1Hex(int32_t idx)
{
   //use badIndex and isHex methods
   if (badIndex(idx) || !isHex(str[idx])) return 0xff;
   char digit = str[idx];
   uint8_t hexVal = 0;
   hexVal = (digit >= '0' && digit <= '9') ? digit - 0x30: hexVal;
   hexVal = (digit >= 'a' && digit <= 'f') ? digit - 0x57: hexVal;
   hexVal = (digit >= 'A' && digit <= 'F') ? digit - 0x37: hexVal;
   return hexVal;
}


/*
 * convertNHex
 *
 * Builds and returns a number from the values in the array from the starting
 * index to the ending index (calculated from startIdx and len)
 * treating those characters as if they are hex digits.
 * Returns 0 if the indices are invalid or if the characters are not hex.
 *
 * Valid characters for conversion are:
 *  '0', '1' through '9'
 *  'a', 'b' through 'f'
 *  'A', 'B' through 'F'
 *
 *  Three cases are possible:
 *  1) returns converted number if the characters in the specified
 *     indices are valid hex characters
 *  2) returns 0 if the indices are invalid
 *  3) returns 0 if the indices are valid but the characters are not all hex
 */
uint32_t String::convertNHex(int32_t startIdx, int32_t len)
{
   //calculate the ending index based upon starting index and len
   //use badIndex method to check starting and ending index
   int32_t endIdx = startIdx + len - 1;
   if (badIndex(startIdx) || badIndex(endIdx) || startIdx > endIdx) 
   {
      return 0;
   }

   //loop through the hex characters, building the hex value
   int32_t value = 0, i;
   for (i = startIdx; i <= endIdx; i++)
   {
      uint32_t hexVal = convert1Hex(i);
      if (hexVal == 0xff)
      {
         return 0;
      }
      value = (value << 4) + hexVal;
   }
   return value;
}

/*
 * isSubString
 *
 * Returns true if the c-str subStr is in the str array starting
 * at index idx 
 *
 * Three cases are possible:
 * 1) one or both indices are invalid: return false
 * 2) indices are valid and subStr is in str array: return true 
 * 3) indices are valid and subStr is not in str array:  return false
 */

bool String::isSubString(const char * subStr, int32_t idx)
{
   //check the indices of the starting and ending character
   if (badIndex(idx) || badIndex(idx + strlen(subStr) - 1))
   {
      return false;
   }
   int32_t i;
   //check to see if subStr is in str
   for (i = 0; i < (int32_t) strlen(subStr); i++)
   {
      if (subStr[i] != str[idx]) return false;
      idx++;
   }
   return true;
}



/*
 * isSubString
 *
 * Returns true if the std::string subStr is in the str array starting
 * at index idx 
 *
 * Three cases are possible:
 * 1) one or both indices are invalid: return false
 * 2) indices are valid and subStr is in str array: return true
 * 3) indices are valid and subStr is not in str array: return false
 */
bool String::isSubString(std::string subStr, int32_t idx)
{  
   //check the indices of the starting and ending character
   if (badIndex(idx) || badIndex(idx + subStr.length() - 1))
   {  
      return false;
   }
   int32_t i;
   //check to see if subStr is in str
   for (i = 0; i < (int32_t) subStr.length(); i++)
   {  
      if (subStr[i] != str[idx]) return false;
      idx++;
   }
   return true;
}

