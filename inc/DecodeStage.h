#include "PipeRegArray.h"
#include "Stage.h"

#ifndef DECODESTAGE_H
#define DECODESTAGE_H
class DecodeStage: public Stage
{
   private:
      //TODO: provide declarations for new methods
      bool E_bubble;
      uint64_t getSrcA(uint64_t icode, PipeReg * dreg);
      uint64_t getSrcB(uint64_t icode, PipeReg * dreg);
      uint64_t getDstE(uint64_t icode, PipeReg * dreg);
      uint64_t getDstM(uint64_t icode, PipeReg * dreg);
      uint64_t getSelPlusFwdA(uint64_t icode, uint64_t srcA, bool & error, uint64_t e_dstE, 
                              uint64_t e_valE, uint64_t M_dstE, uint64_t M_valE, 
                              uint64_t W_dstE, uint64_t W_valE, uint64_t M_dstM,
                              uint64_t m_valM, uint64_t W_dstM, uint64_t W_valM,
                              uint64_t d_valP);
      uint64_t getFwdB(uint64_t srcB, bool & error, uint64_t e_dstE, 
                              uint64_t e_valE, uint64_t M_dstE, uint64_t M_valE, 
                              uint64_t W_dstE, uint64_t W_valE, uint64_t M_dstM,
                              uint64_t m_valM, uint64_t W_dstM, uint64_t W_valM);
      bool getE_bubble(uint64_t E_icode, uint64_t E_dstM);
      void calculateControlSignals(uint64_t E_icode, uint64_t E_dstM);
   public:
      //These are the only methods called outside of the class
      bool doClockLow(PipeRegArray * pipeRegs);
      void doClockHigh(PipeRegArray * pipeRegs);
      void setEInput(PipeReg * ereg, uint64_t stat, uint64_t icode,
                           uint64_t ifun, uint64_t valC, uint64_t valA,
                           uint64_t valB, uint64_t dstE, uint64_t dstM, uint64_t srcA,
                           uint64_t srcB);
};
#endif
