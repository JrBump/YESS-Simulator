#ifndef MEMORYTESTER_H
#define MEMORYTESTER_H

class MemoryTester
{
   private:
      void doClearMemoryTests();
      void doPutGetByteTests();
      void doPutGetLongTests();
      void doByteErrorTests();
      void doLongErrorTests();
      void doPutByteGetLongTests1();
      void doPutByteGetLongTests2();
      void doPutLongGetByteTests();
      Memory * mem;
   public:
      MemoryTester();
      void doMemoryTests();
};

#endif
