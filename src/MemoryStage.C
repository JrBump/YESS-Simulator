#include "PipeRegArray.h"
#include "MemoryStage.h"
#include "W.h"
#include "M.h"
#include "Instruction.h"
#include "Status.h"
/*
 * doClockLow
 *
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pipeRegs - array of the pipeline register 
                      (F, D, E, M, W instances)
 */
bool MemoryStage::doClockLow(PipeRegArray * pipeRegs)
{
   PipeReg * mreg = pipeRegs->getMemoryReg();
   PipeReg * wreg = pipeRegs->getWritebackReg();

   uint64_t icode = mreg->get(M_ICODE);
   uint64_t valE = mreg->get(M_VALE);
   uint64_t valA = mreg->get(M_VALA);
   uint64_t dstE = mreg->get(M_DSTE);
   uint64_t dstM = mreg->get(M_DSTM);

   bool error = false;
   uint64_t mem_address = mem_addr(icode, valE, valA);
   
   Stage::m_valM = 0;

   if (mem_read(icode))
   {
      Stage::m_valM = Stage::mem->getLong(mem_address, error);
   }
   if (mem_write(icode))
   {
      Stage::mem->putLong(valA, mem_address, error);
   }

   Stage::m_stat = mreg->get(M_STAT);

   if (error)
   {
      Stage::m_stat = Status::SADR;
   }
   
   setWInput(wreg, Stage::m_stat, icode, valE, Stage::m_valM, dstE, dstM);

   return false;
}

/* setWInput
 * provides the input to potentially be stored in the D register
 * during doClockHigh
 *
 * @param: wreg - pointer to the W register instance
 * @param: stat - value to be stored in the stat pipeline register within W
 * @param: icode - value to be stored in the icode pipeline register within W
 * @param: ifun - value to be stored in the ifun pipeline register within W
 * @param: valE - value to be stored in the valE pipeline register within W
 * @param: valM - value to be stored in the valM pipeline register within W
 * @param: dstE - value to be stored in the dstE pipeline register within W
 * @param: dstM - value to be stored in the dstM pipeline register within W
*/
void MemoryStage::setWInput(PipeReg * wreg, uint64_t stat, uint64_t icode, uint64_t valE,
                                  uint64_t valM, uint64_t dstE, uint64_t dstM)
{
   wreg->set(W_STAT, stat);
   wreg->set(W_ICODE, icode);
   wreg->set(W_VALE, valE);
   wreg->set(W_VALM, valM);
   wreg->set(W_DSTE, dstE);
   wreg->set(W_DSTM, dstM);
}

/* doClockHigh
 *
 * applies the appropriate control signal to the F
 * and D register intances
 * 
 * @param: pipeRegs - array of the pipeline register (F, D, E, M, W instances)
*/
void MemoryStage::doClockHigh(PipeRegArray * pipeRegs)
{
   PipeReg * wreg = pipeRegs->getWritebackReg();

   wreg->normal();
}

/* mem_addr
 * Calculates the memory address needed 
 *
 * @param: icode - Instruction Code
 * @param: M_valE - valE from Memory Stage
 * @param: M_valA - valA from Memory Stage
 * 
*/
uint64_t MemoryStage::mem_addr(uint64_t icode, uint64_t M_valE, uint64_t M_valA)
{
   uint64_t mem_addr;

   if (icode == Instruction::IRMMOVQ ||
       icode == Instruction::IPUSHQ ||
       icode == Instruction::ICALL ||
       icode == Instruction::IMRMOVQ)
   {
      mem_addr = M_valE;
   }
   else if (icode == Instruction::IPOPQ ||
            icode == Instruction::IRET)
   {
      mem_addr = M_valA;
   }
   else
   {
      mem_addr = 0;
   }
   return mem_addr;
}

/* mem_read
 * Tells if the memory needs to be read from 
 *
 * @param: icode - Instruction Code
 *  
*/
bool MemoryStage::mem_read(uint64_t icode)
{
   if (icode == Instruction::IMRMOVQ ||
       icode == Instruction::IPOPQ ||
       icode == Instruction::IRET)
   {
      return true;
   }
   else
   {
      return false;
   }
}

/* mem_read
 * Tells if the memory needs to be written 
 *
 * @param: icode - Instruction Code
 *  
*/
bool MemoryStage::mem_write(uint64_t icode)
{
   if (icode == Instruction::IRMMOVQ ||
       icode == Instruction::IPUSHQ ||
       icode == Instruction::ICALL)
   {
      return true;
   }
   else
   {
      return false;
   }
}


