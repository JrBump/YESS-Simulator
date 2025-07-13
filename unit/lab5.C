#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <string.h>
#include <assert.h>
#include "String.h"

//perform unit tests on the String class
int main(int argc, char * argv[])
{

   // test the constructor and the getters
   String * str1 = new String("hello there");
   assert(strcmp("hello there", str1->getCstr()) == 0);
   assert(strlen("hello there") == str1->getLength());
  
   std::string stdstr2("hello world");
   String * str2 = new String(stdstr2);
   assert(stdstr2.compare(str2->getStdstr()) == 0);
   assert(stdstr2.length() == (uint32_t) str2->getLength());

   //If the line below is uncommented, it should cause the String
   //constructor assert to fail.
   //std::string s;
   //String * strtmp = new String("");
   //String strtmp("");

   //test the isChar method
   String str8("0xabc: abc123");
   assert(str8.isChar('0', 0) == true);
   assert(str8.isChar('x', 1) == true);
   assert(str8.isChar(':', 5) == true);
   assert(str8.isChar(';', 5) == false);
   assert(str8.isChar(';', -1) == false);
   assert(str8.isChar(';', 13) == false);

   //test the isNChar method
   String str3("aaaaaphrase with    spaces ");
   assert(str3.isNChar('a', 1, 4) == true);
   assert(str3.isNChar(' ', 0, 5) == false);
   assert(str3.isNChar('a', 1, 5) == false);
   assert(str3.isNChar(' ', 11, 1) == true);
   assert(str3.isNChar(' ', 4, 8) == false);
   assert(str3.isNChar(' ', 11, 2) == false);
   assert(str3.isNChar(' ', 17, -3) == false);
   assert(str3.isNChar(' ', 26, 1) == true);
   assert(str3.isNChar(' ', 26, 2) == false);
   assert(str3.isNChar(' ', -2, -1) == false);

   String str4("0x2aA: 123e5678");
   //test the isNHex method
   assert(str4.isNHex(2, 3) == true);
   assert(str4.isNHex(7, 2) == true);
   assert(str4.isNHex(9, 2) == true);
   assert(str4.isNHex(11, 2) == true);
   assert(str4.isNHex(13, 2) == true);
   assert(str4.isNHex(14, 2) == false);
   assert(str4.isNHex(16, 2) == false);
   assert(str4.isNHex(8, -1) == false);
   assert(str4.isNHex(-1, 7) == false);
   assert(str4.isNHex(-2, -1) == false);

   String str5("0xabc: def01234");
   //test the isNHex method
   assert(str5.isNHex(2, 3) == true);
   assert(str5.isNHex(7, 2) == true);
   assert(str5.isNHex(9, 2) == true);
   assert(str5.isNHex(11, 2) == true);
   assert(str5.isNHex(13, 2) == true);

   String str7("0xAgC: DEi56789");
   //test the isNHex method
   assert(str7.isNHex(2, 3) == false);
   assert(str7.isNHex(9, 2) == false);

   //test the convert1Hex method
   assert(str7.convert1Hex(2) == 0xa);
   assert(str7.convert1Hex(3) == 0xff);
   assert(str7.convert1Hex(4) == 0xc);
   assert(str7.convert1Hex(5) == 0xff);
   assert(str7.convert1Hex(23) == 0xff);

   //test the convert2Hex method
   assert(str4.convertNHex(2, 3) == 0x2aA);
   assert(str4.convertNHex(7, 2) == 0x12);
   assert(str4.convertNHex(9, 2) == 0x3e);
   assert(str4.convertNHex(11, 2) == 0x56);
   assert(str4.convertNHex(13, 2) == 0x78);
   assert(str4.convertNHex(14, 2) == 0);
   assert(str4.convertNHex(16, 2) == 0);
   assert(str4.convertNHex(8, -1) == 0);
   assert(str4.convertNHex(-1, 7) == 0);
   assert(str4.convertNHex(-2, -1) == 0);

   assert(str5.convertNHex(2, 3) == 0xabc);
   assert(str5.convertNHex(7, 2) == 0xde);
   assert(str5.convertNHex(9, 2) == 0xf0);
   assert(str5.convertNHex(11, 2) == 0x12);
   assert(str5.convertNHex(13, 2) == 0x34);

   String str6("0xABC: DEF56789");
   assert(str6.convertNHex(2, 3) == 0xabc);
   assert(str6.convertNHex(7, 2) == 0xde);
   assert(str6.convertNHex(9, 2) == 0xf5);
   assert(str6.convertNHex(11, 2) == 0x67);
   assert(str6.convertNHex(13, 2) == 0x89);
   assert(str6.convertNHex(15, 2) == 0); //out of bounds
   assert(str6.convertNHex(4, 2) == 0);  //not hex
   assert(str6.convertNHex(5, 2) == 0);  //not hex

   //test the isSubString method passing it a char *
   assert(str8.isSubString("0x", 0) == true);
   assert(str8.isSubString("abc", 2) == true);
   assert(str8.isSubString("abc123", 7) == true);
   assert(str8.isSubString("abc123", 5) == false);
   assert(str8.isSubString("abc123", 8) == false);
   assert(str8.isSubString("abc123", -1) == false);

   //test the isSubString method passing it a std::string
   assert(str8.isSubString(std::string("0x"), 0) == true);
   assert(str8.isSubString(std::string("abc"), 2) == true);
   assert(str8.isSubString(std::string("abc123"), 7) == true);
   assert(str8.isSubString(std::string("abc123"), 5) == false);
   assert(str8.isSubString(std::string("abc123"), 8) == false);
   assert(str8.isSubString(std::string("abc123"), -1) == false);

   //test the isNHex method
   std::cout << "Congratuations!  All tests pass!\n"; 
}
