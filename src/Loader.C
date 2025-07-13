#include <fstream>
#include <cstdint>
#include "Memory.h"
#include "String.h"
#include "Loader.h"

/* 
 * Loader
 * Initializes the private data members
 */
Loader::Loader(int argc, char * argv[])
{
   //keep track of last mem byte written to for error checking
   this->lastAddress = -1;   
   this->mem = Memory::getInstance();              //memory instance
   this->inputFile = NULL;   
   if (argc > 1) inputFile = new String(argv[1]);  //input file name
}

/*
 * printErrMsg
 * Prints an error message and returns false (load failed)
 * If the line number is not -1, it also prints the line where the error occurred
 *
 * which - indicates error number
 * lineNumber - number of line in input file on which error occurred (if applicable)
 * line - line on which error occurred (if applicable)
 */
bool Loader::printErrMsg(int32_t which, int32_t lineNumber, String * line)
{
   static char * errMsg[Loader::numerrs] = 
   {
      (char *) "Usage: yess <file.yo>\n",                       //Loader::usage
      (char *) "Input file name must end with .yo extension\n", //Loader::badfile
      (char *) "File open failed\n",                            //Loader::openerr
      (char *) "Badly formed data record\n",                    //Loader::baddata
      (char *) "Badly formed comment record\n",                 //Loader::badcomment
   };   
   if (which >= Loader::numerrs)
   {
      std::cout << "Unknown error: " << which << "\n";
   } else
   {
      std::cout << errMsg[which]; 
      if (lineNumber != -1 && line != NULL)
      {
         std::cout << "Error on line " << std::dec << lineNumber
                   << ": " << line->getStdstr() << std::endl;
      }
   } 
   return false; //load fails
}

/*
 * openFile
 * The name of the file is in the data member openFile (could be NULL if
 * no command line argument provided)
 * Checks to see if the file name is well-formed and can be opened
 * If there is an error, it prints an error message and returns false
 * by calling printErrMsg.
 * Otherwise, the file is opened and the function returns true
 *
 * modifies inf data member (file handle) if file is opened
 */
bool Loader::openFile()
{
   //Did user supply a file?
   if (inputFile == NULL) return printErrMsg(Loader::usage, -1, NULL);

   //Is the file name well-formed?
   if (!goodFile(*inputFile)) return printErrMsg(Loader::badfile, -1, NULL);

   //can the file be opened?
   inf.open(inputFile->getCstr());
   if (!inf.is_open()) return printErrMsg(Loader::openerr, -1, NULL);

   return true;  //file name is good and file open succeeded
}

/*
 * load 
 * Opens the .yo file
 * Reads the lines in the file line by line and
 * loads the data bytes in data records into the Memory
 * If a line has an error in it, then NONE of the line will be
 * loaded into memory and the load function will return false
 *
 * Returns true if load succeeded (no errors in the input) 
 * and false otherwise
*/   
bool Loader::load()
{
   if (!openFile()) return false;

   std::string line;
   int lineNumber = 1;  
   while (getline(inf, line))
   {
      String inputLine(line);

      bool dataRec = isDataRecord(inputLine);
      
      //A data record has an address
      if (dataRec && errorInDataRecord(inputLine))
      {
         return printErrMsg(Loader::baddata, lineNumber, &inputLine);
      } else if (!dataRec && errorInCommentRecord(inputLine))
      {
         return printErrMsg(Loader::badcomment, lineNumber, &inputLine);
      }
      //line is free from errors
      //load it into memory if it is a data record and has data
      if (dataRec && hasData(inputLine)) loadLine(inputLine);

      //increment the line number for next iteration
      lineNumber++;
   }
   return true;  //load succeeded
}

/*
 * goodFile
 * returns true if the name of the file passed in is a properly 
 * formed .yo filename. A properly formed .yo file name is at least
 * four characters in length and ends with a .yo extension.
 *
 * @return true - if the filename is properly formed
 *         false - otherwise
 */
bool Loader::goodFile(String filename)
{
   //length must be greater than 3; needs to end with a .yo
   int32_t len = filename.getLength();
   bool hasdotyo = filename.isSubString(".yo", len - 3);
   return (len >= 4) && hasdotyo;
}

/*
 * isDataRecord
 * returns true if the line passed in has an address on it.
 * A line that has an address has a "0x" in columns 0 and 1.
 * Otherwise, the line doesn't have an address on it.
 *
 * @param line - a String containing a line of input from 
 *               a .yo file
 * @return true, if the line has an address on it
 *         false, otherwise
 */
bool Loader::isDataRecord(String line)
{
   //data records start with "0x"
   return line.isSubString("0x", 0);
}

/*
 * hasData
 * returns true if the line passed in has data on it.
 * A line that has an data will have a hex character
 * at index Loader::databegin.
 * Otherwise, the line does not have data on it.
 *
 * @param line - a String containing a line of input from 
 *               a .yo file
 * @return true, if the line has data in it
 *         false, otherwise
 */
bool Loader::hasData(String line)
{
   //if there is a hex character in the databegin column
   //then the record has data
   return line.isNHex(Loader::databegin, 1);
}

/*
 * loadLine
 * The line that is passed in contains an address and data
 * and is free from errors.
 * This method loads that data into memory byte by byte.
 * Also, the method updates Loader::lastAddress with the
 * last address used in a call to Memory::putByte
 *
 * @param line - a String containing a line of valid input from 
 *               a .yo file. The line contains an address and
 *               a variable number of bytes of data (at least one)
 */
void Loader::loadLine(String line)
{
   //get starting address
   int32_t addr = line.convertNHex(Loader::addrbegin, 
                  Loader::addrend - Loader::addrbegin + 1);
   int32_t i = Loader::databegin;
   uint64_t databyte;
   bool mem_error;

   //loop while there is data on the line
   while (line.isNHex(i, 1) == true)
   {
      databyte = line.convertNHex(i, 2);
      mem->putByte(databyte, addr, mem_error);
      this->lastAddress = addr;
      addr++;
      i+=2;
   } 
}

/*
 * errorInDataRecord
 * This is used to check to see if a data record has errors.
 * A valid data record has:
 * 1) a well-formed address field
 * 2) a sequence of 0 to Loader::maxbytes of hex data
 * 3) a | in column Loader::comment
 *
 * @param line from the .yo file
 * @return true if the line is formatted properly
 *         and false otherwise
 */
bool Loader::errorInDataRecord(String line)
{
   //is there an error in the address?
   if (errorAddr(line)) return true;

   //is there an error in the bytes of data?
   if (errorData(line)) return true;

   //is the | character in the correct spot?
   if (!line.isChar('|', Loader::comment)) return true;

   return false;
}

/*
 * errorData
 * Determines whether the data bytes in a data record are formatted properly.
 *
 * A valid sequence of data bytes:
 * 1) contains 0 to Loader::maxBytes of
 *    data in hex starting in column Loader::addrbegin.
 * 2) contains spaces after the last byte of data up to but excluding
 *    the | in column Loader::comment
 * 3) no write of the data bytes to memory will exceed the bounds of
 *    the memory. To check this, you will need to use the address in the
 *    line as well as the number of data bytes on the line. For example,
 *    0xffe: 2030
 *    Since the address is 0xffe and the number of data bytes is 2,
 *    the second byte wouldn't be able to be stored in memory
 *
 * @param line from the .yo file
 * @return true if the line is formatted properly
 *         and false otherwise
 */
bool Loader::errorData(String line)
{
   int32_t i, ct = 0;

   //length of address
   int32_t len = Loader::addrend - Loader::addrbegin + 1;

   //non-space characters must be in hex
   for (i = Loader::databegin; line.isNHex(i, 2); i+=2)
   {
      ct++;
   }

   //Only 10 bytes per line
   //Don't write beyond the size of memory
   int32_t addr = line.convertNHex(Loader::addrbegin, len); 
   if (ct > Loader::maxbytes || ct + addr > Memory::size) return true;

   //There needs to be at least one space after the data
   bool spaces = line.isNChar(' ', i, Loader::comment - i);
   if (!spaces) return true;
      
   return false;
}

/*
 * errorInCommentRecord
 * Checks to make sure a comment line is formatted
 * properly.  A properly formatted comment line contains
 * spaces from column 0 up to the '|' and contains a '|' in
 * column Loader::comment
 *
 * @param line from the .yo file
 * @returns true if the line contains an error and false 
 *          otherwise
 */
bool Loader::errorInCommentRecord(String line)
{
   //there needs to be spaces up to the | column
   if (!line.isNChar(' ', 0, Loader::comment)) return true;

   //there needs to be a |
   if (!line.isChar('|', Loader::comment)) return true;

   return false;
}

/*
 * errorAddr
 * This function is called when the line contains an address in order
 * to check whether the address is properly formed.  An address is of
 * this format: 0xHHH: where HHH are valid hex digits. The colon must be
 * followed by one space.
 *
 * Also, a valid address must be greater than the address that was last
 * passed to a call to Memory::putByte
 * 
 * @param line from a .yo input file
 * @return true if the address is not properly formed and false otherwise
 */
bool Loader::errorAddr(String line)
{
   //calculate the length of the address field
   int32_t len = Loader::addrend - Loader::addrbegin + 1;

   //address starts with 0x
   if (!line.isSubString("0x", 0)) return true;

   //0x followed by 3 hex char
   if (!line.isNHex(Loader::addrbegin, 3)) return true;

   //3 hx chars followed by :
   if (!line.isChar(':', Loader::addrend + 1)) return true;

   //: followed by a space
   if (!line.isChar(' ', Loader::addrend + 2)) return true;

   //calculate the address of the last byte of data
   //in the line; it can't be outside of memory
   int32_t addr = line.convertNHex(Loader::addrbegin, len); 
   if (addr <= this->lastAddress) return true;

   //no errors
   return false;
}

