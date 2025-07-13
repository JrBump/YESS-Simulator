#include <iostream>
#include <assert.h>
#include "Memory.h"
#include "MemoryTester.h"

/** 
 * MemoryTester constructor
 * Initializes the Memory instance 
 *
*/
MemoryTester::MemoryTester()
{
   this->mem = Memory::getInstance();
}

/** 
 * doMemoryTests
 * Calls each individual method to perform tests of the Memory methods
 *
*/
void MemoryTester::doMemoryTests()
{
   assert(this->mem != NULL);
   //call getInstance again to make sure the
   //same instance is returned.  If this fails,
   //you haven't set your singleton up properly
   assert(this->mem == Memory::getInstance());

   doClearMemoryTests();
   doPutGetByteTests();
   doPutGetLongTests();
   doByteErrorTests();
   doLongErrorTests();
   doPutByteGetLongTests1();
   doPutByteGetLongTests2();
   doPutLongGetByteTests();
   std::cout << "All Memory tests passed.\n"; 
}

/**
 * doClearMemoryTests
 * Makes sure that the memory has been initialized to 0
*/
void MemoryTester::doClearMemoryTests()
{
   int32_t addr;
   bool error;
   uint8_t value;
   for (addr = 0; addr < Memory::size; addr++)
   {
      value = this->mem->getByte(addr, error);
      //byte retrieved should be 0
      assert(value == 0);
      //address is valid so error should be false
      assert(error == false);
   }
}

/** 
 * doPutGetByteTests
 * Perform tests on Memory::putByte and Memory::getByte
 *
*/
void MemoryTester::doPutGetByteTests()
{
   int32_t addr;
   bool error;
   uint8_t svalue = 0x88;
   uint8_t lvalue;
   for (addr = 0; addr < Memory::size; addr++)
   {
      this->mem->putByte(svalue, addr, error);
      //address is valid so error should be false
      assert(error == false);
      lvalue = this->mem->getByte(addr, error);
      //byte retrieved should be the same as that stored
      assert(lvalue == svalue);
      //address is valid so error should be false
      assert(error == false);
   }
}

/** 
 * doPutGetLongTests
 * Perform tests on Memory::putLong and Memory::getLong
 *
*/
void MemoryTester::doPutGetLongTests()
{
   int32_t addr;
   bool error;
   uint64_t svalue = 0x1122334455667788;
   uint64_t lvalue;
   for (addr = 0; addr < Memory::size; addr+=8)
   {
      this->mem->putLong(svalue, addr, error);
      //address is valid so error should be false
      assert(error == false);
      lvalue = this->mem->getLong(addr, error);
      //long retrieved should be the same as that stored
      assert(lvalue == svalue);
      //address is valid so error should be false
      assert(error == false);
   }
}

/** 
 * doByteErrorTests
 * Perform error tests on Memory::getByte and Memory::putByte
 *
*/
void MemoryTester::doByteErrorTests()
{
   int32_t addr;
   bool error;
   uint8_t svalue = 0x88;
   uint8_t lvalue;
   //test on a random range of addresses
   for (addr = -0x2000; addr < 0x2000; addr++)
   {
      if (addr < 0 || addr >= Memory::size)
      {
         this->mem->putByte(svalue, addr, error);
         //address is invalid so error should be true
         assert(error == true);
         lvalue = this->mem->getByte(addr, error);
         //address is invalid so getByte should return 0
         assert(lvalue == 0);
         //address is invalid so error should be true
         assert(error == true);
      }
   }
}

/** 
 * doLongErrorTests
 * Perform error tests on Memory::getLong and Memory::putLong
 *
*/
void MemoryTester::doLongErrorTests()
{
   int32_t addr;
   bool error;
   uint64_t svalue = 0x1122334455667788;
   uint64_t lvalue;
   for (addr = -0x2000; addr < 0x2000; addr++)
   {
      if (addr < 0 || addr >= Memory::size || addr % 8 != 0)
      {
         //address is invalid so error should be true
         this->mem->putLong(svalue, addr, error);
         assert(error == true);
         lvalue = this->mem->getLong(addr, error);
         //address is invalid so 0 returned by getLong
         assert(lvalue == 0);
         //address is invalid so error should be true
         assert(error == true);
      }
   }
}

/**
 * doPutByteGetLongTests
 * Modifies one byte and then gets the Long containing that
 * byte to see it was modified properly
*/
void MemoryTester::doPutByteGetLongTests1()
{
   int32_t addr;
   uint64_t result;
   uint8_t byte;
   bool error;
   uint64_t svalue = 0x1122334455667788;
   uint64_t lvalue;
   for (addr = 0; addr < Memory::size; addr+=8)
   {  
      //zero out the memory
      this->mem->putLong(0, addr, error);
      //address is valid so error should be false
      assert(error == false);
      result = 0;
      for (int32_t i = 0; i < 8; i++)
      {
         byte = (svalue >> (i * 8)) & 0xff;
         result = result + (((uint64_t) byte) << (i * 8));
         this->mem->putByte(byte, addr+i, error);
         lvalue = this->mem->getLong(addr, error);
         assert(lvalue == result);
         assert(error == false);
      }
   }
}

/**
 * doPutByteGetLongTests2
 * Modifies eight bytes and then gets it as a long 
 *
*/
void MemoryTester::doPutByteGetLongTests2()
{
   int32_t addr;
   uint8_t byte;
   bool error;
   uint64_t svalue = 0x1122334455667788;
   uint64_t lvalue;
   for (addr = 0; addr < Memory::size; addr+=8)
   {
      for (int32_t i = 0; i < 8; i++)
      {
         byte = (svalue >> (i * 8)) & 0xff;
         this->mem->putByte(byte, addr+i, error);
         assert(error == false);
      }
      lvalue = this->mem->getLong(addr, error);
      assert(error == false);
      assert(lvalue == svalue);
   }
}

/**
 * doPutLongGetByteTests
 * Modifies eight bytes with a putLong and then
 * gets each byte of the long individually 
 *
*/
void MemoryTester::doPutLongGetByteTests()
{
   int32_t addr;
   uint8_t byte;
   uint8_t mem_byte;
   bool error;
   uint64_t svalue = 0x1122334455667788;
   for (addr = 0; addr < Memory::size; addr+=8)
   {
      this->mem->putLong(svalue, addr, error);
      assert(error == false);
      for (int32_t i = 0; i < 8; i++)
      {
         byte = (svalue >> (i * 8)) & 0xff;
         mem_byte = this->mem->getByte(addr+i, error);
         assert(mem_byte == byte);
         assert(error == false);
      }
   }
}

