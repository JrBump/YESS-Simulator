#include "PipeRegArray.h"
#include "Stage.h"

#ifndef MEMORYSTAGE_H
#define MEMORYSTAGE_H
class MemoryStage: public Stage
{
   private:
      //TODO: provide declarations for new methods
      uint64_t mem_addr(uint64_t icode, uint64_t M_valE, uint64_t M_valA);
      bool mem_read(uint64_t icode);
      bool mem_write(uint64_t icode);
   public:
      //These are the only methods called outside of the class
      bool doClockLow(PipeRegArray * pipeRegs);
      void doClockHigh(PipeRegArray * pipeRegs);
      void setWInput(PipeReg * mreg, uint64_t stat, uint64_t icode, uint64_t valE, 
                            uint64_t valA, uint64_t dstE, uint64_t dstM);
};
#endif
