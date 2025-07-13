//TODO add more #includes as you need them
#include <cstdint>
#include "PipeRegArray.h"
#include "PipeReg.h"
#include "Memory.h"
#include "FetchStage.h"
#include "Instruction.h"
#include "RegisterFile.h"
#include "Status.h"
#include "F.h"
#include "D.h"
#include "E.h"
#include "M.h"
#include "W.h"
#include "Tools.h"
#include "Stage.h"


/*
 * doClockLow
 *
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pipeRegs - array of the pipeline register 
                      (F, D, E, M, W instances)
 */
bool FetchStage::doClockLow(PipeRegArray * pipeRegs)
{
   
   PipeReg * freg = pipeRegs->getFetchReg();
   PipeReg * dreg = pipeRegs->getDecodeReg();
   PipeReg * ereg = pipeRegs->getExecuteReg();
   PipeReg * mreg = pipeRegs->getMemoryReg();
   uint64_t E_icode = ereg->get(E_ICODE);
   uint64_t E_dstM = ereg->get(E_DSTM);
   uint64_t D_icode = dreg->get(D_ICODE);
   uint64_t M_icode = mreg->get(M_ICODE);
   bool mem_error = false;
   uint64_t icode = Instruction::INOP, ifun = Instruction::FNONE;
   uint64_t rA = RegisterFile::RNONE, rB = RegisterFile::RNONE;
   uint64_t valC = 0, valP = 0, stat = Status::SAOK, predPC = 0;
   bool needvalC = false;
   bool needregId = false;

   //select PC value and read byte from memory
   //set icode and ifun using byte read from memory
   //uint64_t f_pc =  .... call your select pc function
   uint64_t f_pc = selectPC(pipeRegs);
   uint64_t instruction = mem->getByte(f_pc,mem_error);

   icode = Tools::getBits(instruction, 4, 7);
   ifun = Tools::getBits(instruction, 0, 3);

   if (mem_error)
   {
      icode = Instruction::INOP;
      ifun = Instruction::FNONE;
   }

   bool valid = instr_valid(icode);
   stat = f_stat(mem_error, icode, valid);

   needvalC = needValC(icode);
   needregId = needRegIds(icode);

   if (needregId)
   {
      getRegsIds(f_pc, rA, rB);
   }
   if (needvalC)
   {
      valC = getValC(f_pc, needregId);
   }

   valP = PCincrement(f_pc, needvalC, needregId);

   //calculate the predicted PC value
   predPC = predictPC(icode, valC, valP);

   //set the input for the PREDPC pipe register field in the F register
   freg->set(F_PREDPC, predPC);

   calculateControlSignals(E_icode, E_dstM, D_icode, M_icode);

   //set the inputs for the D register
   setDInput(dreg, stat, icode, ifun, rA, rB, valC, valP);
   return false;
}

/* doClockHigh
 *
 * applies the appropriate control signal to the F
 * and D register intances
 * 
 * @param: pipeRegs - array of the pipeline register (F, D, E, M, W instances)
*/
void FetchStage::doClockHigh(PipeRegArray * pipeRegs)
{
   PipeReg * freg = pipeRegs->getFetchReg();  
   PipeReg * dreg = pipeRegs->getDecodeReg();
   if (!f_stall)
   {
      freg->normal();
   } 
   if (D_bubble) 
   {
      ((D *)dreg)->bubble();
   }
   else if (!d_stall)
   {
      dreg->normal();
   }
}

/* setDInput
 * provides the input to potentially be stored in the D register
 * during doClockHigh
 *
 * @param: dreg - pointer to the D register instance
 * @param: stat - value to be stored in the stat pipeline register within D
 * @param: icode - value to be stored in the icode pipeline register within D
 * @param: ifun - value to be stored in the ifun pipeline register within D
 * @param: rA - value to be stored in the rA pipeline register within D
 * @param: rB - value to be stored in the rB pipeline register within D
 * @param: valC - value to be stored in the valC pipeline register within D
 * @param: valP - value to be stored in the valP pipeline register within D
*/
void FetchStage::setDInput(PipeReg * dreg, uint64_t stat, uint64_t icode,
                           uint64_t ifun, uint64_t rA, uint64_t rB,
                           uint64_t valC, uint64_t valP)
{
   dreg->set(D_STAT, stat);
   dreg->set(D_ICODE, icode);
   dreg->set(D_IFUN, ifun);
   dreg->set(D_RA, rA);
   dreg->set(D_RB, rB);
   dreg->set(D_VALC, valC);
   dreg->set(D_VALP, valP);
}

/* 
 * 
 * Sets all needed information for the next stage of the pipeline * 
 *
 * @param: f_pc - current program counter
 * @param: needV -  boolean to decide if valC is being used
 * @param: needsRegs - boolean to decide if registers are needed
*/
uint64_t FetchStage::PCincrement(uint64_t f_pc, bool needV, bool needRegs)
{
   int increment = REG_ID_SIZE;

   if (needRegs)
   {
      increment += REG_ID_SIZE;
   }
   if (needV)
   {
      increment += VALC_SIZE;
   }

   return f_pc + increment;
}

/* 
 * 
 * Helps the pipeline predict where the Program counter will go to * 
 *
 * @param: F_icode - Icode from fetch
 * @param: F_valC - ValC from fetch
 * @param: F_valP - ValP from Fetch
*/
uint64_t FetchStage::predictPC(uint64_t F_icode, uint64_t F_valC, uint64_t F_valP)
{
   uint64_t F_predPC;
   if (F_icode == Instruction::IJXX || F_icode == Instruction::ICALL)
   {
      F_predPC = F_valC;
   }
   else
   {
      F_predPC = F_valP;
   }
   return F_predPC;
}

/* 
 * 
 * Calculates if Val C is needed for the instruction * 
 *
 * @param: F_icode - The icode from fetch
 * 
*/
bool FetchStage::needValC(uint64_t F_icode)
{
   if (F_icode == Instruction::IIRMOVQ ||
       F_icode == Instruction::IRMMOVQ ||
       F_icode == Instruction::IMRMOVQ ||
       F_icode == Instruction::IJXX ||
       F_icode == Instruction::ICALL ||
       F_icode == Instruction::IADDQ)
       {
         return true;
       }
   return false;
}

/* 
 * 
 * Calculates if Registers are needed for the instruction * 
 *
 * @param: F_icode - The icode from fetch
 * 
*/
bool FetchStage::needRegIds(uint64_t F_icode)
{
   if (F_icode == Instruction::IRRMOVQ ||
       F_icode == Instruction::IOPQ ||
       F_icode == Instruction::IPUSHQ ||
       F_icode == Instruction::IPOPQ ||
       F_icode == Instruction::IIRMOVQ ||
       F_icode == Instruction::IRMMOVQ ||
       F_icode == Instruction::IMRMOVQ ||
       F_icode == Instruction::IADDQ)
   {
      return true;
   }
   return false;
}

/* 
 * 
 * Gathers needed information to select the PC needed for * 
 *
 * @param: pipeRegs - Pipeline with needed registers 
 * 
*/
uint64_t FetchStage::selectPC(PipeRegArray * pipeRegs)
{
      PipeReg * mreg = pipeRegs->getMemoryReg();
      PipeReg * wreg = pipeRegs->getWritebackReg();
      PipeReg * freg = pipeRegs->getFetchReg();

      uint64_t M_cnd = mreg->get(M_CND);
      uint64_t M_valA = mreg->get(M_VALA);
      uint64_t M_icode = mreg->get(M_ICODE);

      uint64_t W_icode = wreg->get(W_ICODE);
      uint64_t W_valM = wreg->get(W_VALM);

      uint64_t f_pc;
      if (M_icode == Instruction::IJXX && !M_cnd)
      {
         f_pc = M_valA;
      }
      else if (W_icode == Instruction::IRET)
      {
         f_pc = W_valM;
      }
      else 
      {
         f_pc = freg->get(F_PREDPC);
      }
      return f_pc;
}

/* 
 * 
 * Gathers needed registers ids * 
 *
 * @param: f_pc - current program counter
 * @param: rA - Register 1
 * @param: rB - Register 2
*/
void FetchStage::getRegsIds(uint64_t f_pc,uint64_t & rA, uint64_t & rB)
{
   bool mem_error = false;
   uint64_t hold = mem->getByte(f_pc + 1, mem_error);
   rA = Tools::getBits(hold,4,7);
   rB = Tools::getBits(hold,0,3);
}

/* 
 * 
 * Gathers needed registers ids * 
 *
 * @param: f_pc - current program counter
 * @param: needRegIds - Boolean to select what is needed
*/
uint64_t FetchStage::getValC(uint64_t f_pc, bool needRegIds)
{
   uint64_t valC = 0;
   bool mem_error = false;
   uint8_t tempArray[8];
   f_pc++;
   if (needRegIds)
   {
      f_pc = f_pc + 1;
   }
   for (int i = 0; i < 8; i++)
   {
      tempArray[i] = mem->getByte(f_pc + i, mem_error);
   }
   valC = Tools::buildLong(tempArray);
   return valC;

}

/* 
 * 
 * Confirms if the Instruction is an instruction * 
 *
 * @param: F_icode - Instruction Code
*/
bool FetchStage::instr_valid(uint64_t F_icode)
{
    switch (F_icode)
    {
        case Instruction::INOP:
        case Instruction::IHALT:
        case Instruction::IRRMOVQ:
        case Instruction::IIRMOVQ:
        case Instruction::IRMMOVQ:
        case Instruction::IMRMOVQ:
        case Instruction::IOPQ:
        case Instruction::IJXX:
        case Instruction::ICALL:
        case Instruction::IRET:
        case Instruction::IPUSHQ:
        case Instruction::IPOPQ:
        case Instruction::IADDQ:
            return true;

        default:
            return false;
    }
}

/* 
 * 
 * Gathers needed Status for further stages * 
 *
 * @param: mem_error - Memory Check
 * @param: icode - Instruction Code
 * @param: instr_valid - Instruction Check Bool
*/
uint64_t FetchStage::f_stat(bool mem_error, uint64_t icode, bool instr_valid)
{
   uint64_t f_stat;

   if (mem_error)
   {
      f_stat = Status::SADR;
   }
   else if (!instr_valid)
   {
      f_stat = Status::SINS;
   }
   else if (icode == Instruction::IHALT)
   {
      f_stat = Status::SHLT;
   }
   else
   {
      f_stat = Status::SAOK;
   }
   return f_stat;
}

/* 
 * 
 * Checks to see if a stall is needed * 
 *
 * @param: E_icode - Instruction Code from Execute Stage
 * @param: E_dstM - Destination for Memory from Execute Stage
 * @param: D_icode - Instruction Code from Decode Stage
 * @param: M_icode - Instruction Code from Memory Stage 
*/
bool FetchStage::F_stall(uint64_t E_icode, uint64_t E_dstM, uint64_t D_icode, uint64_t M_icode)
{
   if (((E_icode == Instruction::IMRMOVQ || E_icode == Instruction::IPOPQ) &&
        (E_dstM == Stage::d_srcA || E_dstM == Stage::d_srcB)) ||
         (D_icode == Instruction::IRET ||
          E_icode == Instruction::IRET ||
          M_icode == Instruction::IRET))
   {
      return true;
   }
   return false;
}

/* 
 * 
 * Checks to see if a stall is needed * 
 *
 * @param: E_icode - Instruction Code from Execute Stage
 * @param: E_dstM - Destination for Memory from Execute Stage
*/
bool FetchStage::D_stall(uint64_t E_icode, uint64_t E_dstM)
{
   if ((E_icode == Instruction::IMRMOVQ || E_icode == Instruction::IPOPQ) &&
       (E_dstM == Stage::d_srcA || E_dstM == Stage::d_srcB))
   {
      return true;
   }
   return false;
}

/* 
 * 
 * Checks to calculate the needed Control Signals * 
 *
 * @param: E_icode - Instruction Code from Execute Stage
 * @param: E_dstM - Destination for Memory from Execute Stage
 * @param: D_icode - Instruction Code from Decode Stage
 * @param: M_icode - Instruction Code from Memory Stage 
*/
void FetchStage::calculateControlSignals(uint64_t E_icode, uint64_t E_dstM, uint64_t D_icode, uint64_t M_icode)
{
   f_stall = F_stall(E_icode, E_dstM, D_icode, M_icode);
   d_stall = D_stall(E_icode, E_dstM);
   D_bubble = getD_bubble(E_icode, E_dstM, D_icode, M_icode);
}

/* 
 * 
 * Checks if bubble is needed * 
 *
 * @param: E_icode - Instruction Code from Execute Stage
 * @param: E_dstM - Destination for Memory from Execute Stage
 * @param: D_icode - Instruction Code from Decode Stage
 * @param: M_icode - Instruction Code from Memory Stage 
*/
bool FetchStage::getD_bubble(uint64_t E_icode, uint64_t E_dstM, uint64_t D_icode, uint64_t M_icode)
{
   if ((E_icode == Instruction::IJXX && !Stage::e_Cnd) ||
       ((!((E_icode == Instruction::IMRMOVQ ||
          ((E_icode == Instruction::IPOPQ) && 
          E_dstM == Stage::d_srcA )||
          E_dstM == Stage::d_srcB)) &&
          (D_icode == Instruction::IRET ||
          E_icode == Instruction::IRET ||
          M_icode == Instruction::IRET))))
   {
      return true;
   }
   return false;
}