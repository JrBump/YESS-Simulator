#include "PipeRegArray.h"
#include "ExecuteStage.h"
#include "Instruction.h"
#include "RegisterFile.h"
#include "ConditionCodes.h"
#include "Tools.h"
#include "M.h"
#include "E.h"
#include "W.h"
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
bool ExecuteStage::doClockLow(PipeRegArray * pipeRegs)
{
   PipeReg * ereg = pipeRegs->getExecuteReg();
   PipeReg * mreg = pipeRegs->getMemoryReg();
   PipeReg * wreg = pipeRegs->getWritebackReg();

   uint64_t stat = ereg->get(E_STAT);
   uint64_t icode = ereg->get(E_ICODE);
   uint64_t ifun = ereg->get(E_IFUN);
   uint64_t valC =  ereg->get(E_VALC);
   uint64_t valA = ereg->get(E_VALA);
   uint64_t valB = ereg->get(E_VALB);
   uint64_t dstE = ereg->get(E_DSTE);
   uint64_t dstM = ereg->get(E_DSTM);
   uint64_t W_stat = wreg->get(W_STAT);
   bool error = false;

   
   uint64_t alua = aluA(icode, valA, valC);
   uint64_t alub = aluB(icode, valB);
   uint64_t alufun = aluFun(icode, ifun);

   bool setcc = set_cc(icode, Stage::m_stat, W_stat);


   M_bubble = calculateControlSignal(Stage::m_stat, W_stat);
   Stage::e_Cnd = getECond(icode, ifun, error);

   Stage::e_dstE = e_dstE(icode, Stage::e_Cnd, dstE);
   Stage::e_valE = alu(alufun, alua, alub);
   
   if(setcc)
   {
      cC(alufun, alua, alub, Stage::e_valE);
   }

   setMInput(mreg, stat, icode, Stage::e_Cnd, Stage::e_valE, valA, Stage::e_dstE, dstM);
   return false;
}

/* setMInput
 * provides the input to potentially be stored in the D register
 * during doClockHigh
 *
 * @param: mreg - pointer to the M register instance
 * @param: stat - value to be stored in the stat pipeline register within M
 * @param: icode - value to be stored in the icode pipeline register within M
 * @param: cnd - value to be stored in the cnd pipeline register within M
 * @param: valE - value to be stored in the valE pipeline register within M
 * @param: valA - value to be stored in the valA pipeline register within M
 * @param: dstE - value to be stored in the dstE pipeline register within M
 * @param: dstM - value to be stored in the dstM pipeline register within M
*/
void ExecuteStage::setMInput(PipeReg * mreg, uint64_t stat, uint64_t icode,
                           uint64_t cnd, uint64_t valE, uint64_t valA, 
                           uint64_t dstE, uint64_t dstM)
{
   mreg->set(M_STAT, stat);
   mreg->set(M_ICODE, icode);
   mreg->set(M_CND,cnd);
   mreg->set(M_VALE, valE);
   mreg->set(M_VALA, valA);
   mreg->set(M_DSTE, dstE);
   mreg->set(M_DSTM, dstM);
}

/* doClockHigh
 *
 * applies the appropriate control signal to the F
 * and D register intances
 * 
 * @param: pipeRegs - array of the pipeline register (F, D, E, M, W instances)
*/
void ExecuteStage::doClockHigh(PipeRegArray * pipeRegs)
{
   PipeReg * mreg = pipeRegs->getMemoryReg();
   if (M_bubble)
   {
      ((M *)mreg)->bubble();
   }
   else
   {
      mreg->normal();
   }
}

/* calculateControlSignal
 * 
 * Checks to calculate the needed Control Signals * 
 *
 * @param: m_stat - Status from Memory Stage
 * @param: W_stat - Status from Writeback Stage
*/
bool ExecuteStage::calculateControlSignal(uint64_t m_stat, uint64_t W_stat)
{
   if ((m_stat == Status::SADR || m_stat == Status::SINS || m_stat == Status::SHLT) ||
       (W_stat == Status::SADR || W_stat == Status::SINS || W_stat == Status::SHLT)) 
   {
        return true;
   }
    return false;
}

/* getECond
 * Sets the Conditions needed for the stage
 *
 * @param: icode - value to be stored in the icode pipeline register within M
 * @param: ifun - value to be stored in the ifun pipeline register within M
 * @param: error - boolean to check for an error
*/
uint64_t ExecuteStage::getECond(uint64_t icode, uint64_t ifun, bool &error)
{
   bool sf = Stage::cc->getConditionCode(ConditionCodes::SF, error);
   bool of = Stage::cc->getConditionCode(ConditionCodes::OF, error);
   bool zf = Stage::cc->getConditionCode(ConditionCodes::ZF, error);

   if (icode != Instruction::IJXX && icode != Instruction::ICMOVXX)
   {
      Stage::e_Cnd = 0;
   }
   else if (ifun == Instruction::UNCOND)
   {
      Stage::e_Cnd = 1;
   }
   else if (ifun == Instruction::LESSEQ)
   {
      Stage::e_Cnd = (sf ^ of) | zf;
   }
   else if (ifun == Instruction::LESS)
   {
      Stage::e_Cnd = sf ^ of;
   }
   else if (ifun == Instruction::EQUAL)
   {
      Stage::e_Cnd = zf;
   }
   else if (ifun == Instruction::NOTEQUAL)
   {
      Stage::e_Cnd = !zf;
   }
   else if (ifun == Instruction::GREATEREQ)
   {
      Stage::e_Cnd = !(sf ^ of);
   }
   else if (ifun == Instruction::GREATER)
   {
      Stage::e_Cnd = !(sf ^ of) & !zf;
   }
   return Stage::e_Cnd;
}

/* aluA
 * Does calculations needed for the ALU
 *
 * @param: icode - value to be stored in the icode pipeline register within M
 * @param: valA - value to be stored in the valA pipeline register within M
 * @param: valC - value to be stored in the valC pipeline register within M
*/
uint64_t ExecuteStage::aluA(uint64_t E_icode, uint64_t E_valA, uint64_t E_valC)
{
   uint64_t aluA;
   if (E_icode == Instruction::IRRMOVQ ||
       E_icode == Instruction::IOPQ)
   {
         aluA = E_valA;
   }
   else if (E_icode == Instruction::IIRMOVQ ||
            E_icode == Instruction::IRMMOVQ ||
            E_icode == Instruction::IMRMOVQ ||
            E_icode == Instruction::IADDQ)
   {
      aluA = E_valC;
   }
   else if (E_icode == Instruction::ICALL ||
            E_icode == Instruction::IPUSHQ)
   {
      aluA = NEGEIGHT;
   }
   else if (E_icode == Instruction::IRET ||
            E_icode == Instruction::IPOPQ)
   {
      aluA = POSEIGHT;
   }
   else
   {
      aluA = 0;
   }
   return aluA;
}

/* aluB
 * Does calculations needed for the ALU
 *
 * @param: E_icode - value to be stored in the icode pipeline register within M
 * @param: E_valB - value to be stored in the valB pipeline register within M
*/
uint64_t ExecuteStage::aluB(uint64_t E_icode, uint64_t E_valB)
{
   uint64_t aluB;
   if (E_icode == Instruction::IRMMOVQ ||
       E_icode == Instruction::IMRMOVQ ||
       E_icode == Instruction::IOPQ ||
       E_icode == Instruction::ICALL ||
       E_icode == Instruction::IPUSHQ || 
       E_icode == Instruction::IRET ||
       E_icode == Instruction::IPOPQ ||
       E_icode == Instruction::IADDQ)
   {
      aluB = E_valB;
   }
   else if (E_icode == Instruction::IRRMOVQ ||
            E_icode == Instruction::IIRMOVQ)
   {
      aluB = 0;
   }
   else
   {
      aluB = 0;
   }
   return aluB;
}

/* aluFun
 * Tells if ALU needs to have fun
 *
 * @param: E_icode - value to be stored in the icode pipeline register within M
 * @param: E_ifun - value to be stored in the valA pipeline register within M
*/
uint64_t ExecuteStage::aluFun(uint64_t E_icode, uint64_t E_ifun)
{
   uint64_t alufun;
   if (E_icode == Instruction::IOPQ)
   {
      alufun = E_ifun;
   }
   else if (E_icode == Instruction::IADDQ)
    {
        alufun = Instruction::ADDQ;
    }
   else
   {
      alufun = Instruction::ADDQ;
   }
   return alufun;
}

/* set_cc
 * Set the Condition Codes needed
 *
 * @param: icode - value to be stored in the icode pipeline register within M
 * @param: m_stat - Status from Memory Stage
 * @param: W_stat - Status from Writeback Stage
*/
bool ExecuteStage::set_cc(uint64_t icode, uint64_t m_stat, uint64_t W_stat)
{
   if ((icode == Instruction::IOPQ || icode == Instruction::IADDQ) && 
        (m_stat != Status::SADR && m_stat != Status::SINS && m_stat != Status::SHLT) &&
        (W_stat != Status::SADR && W_stat != Status::SINS && W_stat != Status::SHLT))
   {
      return true;
   }
   return false;
}

/* e_dstE
 * calculates the calculated information
 *
 * @param: E_icode - value to be stored in the icode pipeline register within M
 * @param: e_cnd - Condition Code
 * @param: E_dstE - Destination of calculation
*/
uint64_t ExecuteStage::e_dstE(uint64_t E_icode, uint64_t e_cnd, uint64_t E_dstE)
{
   uint64_t e_dstE;
   if (E_icode == Instruction::IRRMOVQ && !e_cnd)
   {
      e_dstE = RegisterFile::RNONE;
   }
   else
   {
      e_dstE = E_dstE;
   }
   return e_dstE;
}

/* cC
 * Yes
 *
 * @param: aluFun - Comes from previous methods
 * @param: aluA - Comes from previous methods
 * @param: aluB - Comes from previous methods
 * @param: valE - Comes from previous methods
*/
void ExecuteStage::cC(uint64_t alufun, uint64_t aluA, uint64_t aluB, uint64_t valE)
{
   bool error = false;
   if ((alufun == Instruction::ADDQ && Tools::addOverflow(aluA, aluB)) ||
       (alufun == Instruction::SUBQ && Tools::subOverflow(aluA, aluB)))
   {
      Stage::cc->setConditionCode(true, ConditionCodes::OF, error);
   }
   else
   {
      Stage::cc->setConditionCode(false, ConditionCodes::OF, error);
   }

   if (Tools::sign(valE) == 1)
   {
      Stage::cc->setConditionCode(true, ConditionCodes::SF, error);
   }
   else
   {
      Stage::cc->setConditionCode(false, ConditionCodes::SF, error);
   }

   if (valE == 0)
   {
      Stage::cc->setConditionCode(true, ConditionCodes::ZF, error);
   }
   else
   {
      Stage::cc->setConditionCode(false, ConditionCodes::ZF, error);
   }
}


/* alu
 * Does the actual computations
 *
 * @param: aluFun - Comes from previous methods
 * @param: aluA - Comes from previous methods
 * @param: aluB - Comes from previous methods
*/
uint64_t ExecuteStage::alu(uint64_t aluFun, uint64_t aluA, uint64_t aluB)
{
   if (aluFun == Instruction::ADDQ)
   {
      return aluA + aluB;
   }
   else if (aluFun == Instruction::SUBQ)
   {
      return aluB - aluA;
   }
   else if (aluFun == Instruction::ANDQ)
   {
      return aluA & aluB;
   }
   else
   {
      return aluA ^ aluB;
   }
}

