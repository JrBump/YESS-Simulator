#include "PipeRegArray.h"
#include "PipeReg.h"
#include "Stage.h"

#ifndef FETCHSTAGE_H
#define FETCHSTAGE_H
class FetchStage: public Stage
{
   private:
      //TODO: provide declarations for new methods
      int REG_ID_SIZE = 1;
      int VALC_SIZE = 8;
      bool f_stall;
      bool d_stall;
      bool D_bubble;
      void setDInput(PipeReg * dreg, uint64_t stat, uint64_t icode, uint64_t ifun, 
                     uint64_t rA, uint64_t rB,
                     uint64_t valC, uint64_t valP);
      uint64_t selectPC(PipeRegArray * pipeRegs);
      bool needRegIds(uint64_t F_icode);
      bool needValC(uint64_t F_icode);
      uint64_t predictPC(uint64_t F_icode, uint64_t F_valC, uint64_t F_valP);
      uint64_t PCincrement(uint64_t f_pc, bool needRegs, bool needV);
      void getRegsIds(uint64_t f_pc,uint64_t & rA, uint64_t & rB);
      uint64_t getValC(uint64_t f_pc, bool needRegIds);
      bool instr_valid(uint64_t icode);
      uint64_t f_stat(bool mem_error, uint64_t icode, bool instr_valid);
      bool F_stall(uint64_t E_icode, uint64_t E_dstM, uint64_t D_icode, uint64_t M_icode);
      bool D_stall(uint64_t E_icode, uint64_t E_dstM);
      void calculateControlSignals(uint64_t E_icode, uint64_t E_dstM, uint64_t D_icode, uint64_t M_icode);
      bool getD_bubble(uint64_t E_icode, uint64_t E_dstM, uint64_t D_icode, uint64_t M_icode);
   public:
      //These are the only methods called outside of the class
      bool doClockLow(PipeRegArray * pipeRegs);
      void doClockHigh(PipeRegArray * pipeRegs);

};
#endif
