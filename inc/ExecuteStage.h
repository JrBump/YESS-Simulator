#include "PipeRegArray.h"
#include "Stage.h"

#ifndef EXECUTESTAGE_H
#define EXECUTESTAGE_H
class ExecuteStage: public Stage
{
   private:
      //TODO: provide declarations for new methods
      bool M_bubble;
      int POSEIGHT = 8;
      int NEGEIGHT = -8;
      uint64_t aluA(uint64_t E_icode,uint64_t E_valA, uint64_t E_valC);
      uint64_t aluB(uint64_t E_icode, uint64_t E_valB);
      uint64_t aluFun(uint64_t E_icode, uint64_t E_ifun);
      bool set_cc(uint64_t E_icode, uint64_t m_stat, uint64_t W_stat);
      uint64_t e_dstE(uint64_t E_icode, uint64_t e_cnd, uint64_t E_dstE);
      void cC(uint64_t alufun, uint64_t aluA, uint64_t aluB, uint64_t valE);
      uint64_t alu(uint64_t aluFun, uint64_t aluA, uint64_t aluB);
      uint64_t getECond(uint64_t icode, uint64_t ifun, bool &error);
      bool calculateControlSignal(uint64_t m_stat, uint64_t W_stat);
   public:
      //These are the only methods called outside of the class
      bool doClockLow(PipeRegArray * pipeRegs);
      void doClockHigh(PipeRegArray * pipeRegs);
      void setMInput(PipeReg * mreg, uint64_t stat, uint64_t icode,
                           uint64_t cnd, uint64_t valE, uint64_t valA,
                           uint64_t dstE, uint64_t dstM);
};
#endif
