#include "PipeRegArray.h"
#include "DecodeStage.h"
#include "Instruction.h"
#include "RegisterFile.h"
#include "Stage.h"
#include "D.h"
#include "E.h"
#include "M.h"
#include "W.h"

/*
 * doClockLow
 *
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pipeRegs - array of the pipeline register 
                      (F, D, E, M, W instances)
 */
bool DecodeStage::doClockLow(PipeRegArray * pipeRegs)
{
   PipeReg * dreg = pipeRegs->getDecodeReg();
   PipeReg * ereg = pipeRegs->getExecuteReg();
   PipeReg * mreg = pipeRegs->getMemoryReg();
   PipeReg * wreg = pipeRegs->getWritebackReg();
   uint64_t stat = dreg->get(D_STAT);
   uint64_t icode = dreg->get(D_ICODE);
   uint64_t ifun = dreg->get(D_IFUN);
   uint64_t valC = dreg->get(D_VALC);
   uint64_t valP = dreg->get(D_VALP);
   uint64_t dstE = RegisterFile::RNONE;
   uint64_t dstM = RegisterFile::RNONE;
   uint64_t M_dstE = mreg->get(M_DSTE);
   uint64_t M_dstM = mreg->get(M_DSTM);
   uint64_t W_dstM = wreg->get(W_DSTM);
   uint64_t W_valM = wreg->get(W_VALM);
   uint64_t M_valE = mreg->get(M_VALE);
   uint64_t W_dstE = wreg->get(W_DSTE);
   uint64_t W_valE = wreg->get(W_VALE);
   uint64_t E_icode = ereg->get(E_ICODE);
   uint64_t E_dstM = ereg->get(E_DSTM);
   bool imem_error = false;

   Stage::d_srcA = getSrcA(icode, dreg);
   Stage::d_srcB = getSrcB(icode, dreg);

   dstE = getDstE(icode, dreg);
   dstM = getDstM(icode, dreg);

   uint64_t valB = getFwdB(Stage::d_srcB, imem_error, Stage::e_dstE, Stage::e_valE, M_dstE, M_valE, W_dstE, W_valE, M_dstM, Stage::m_valM, W_dstM, W_valM);
   uint64_t valA = getSelPlusFwdA(icode, Stage::d_srcA, imem_error, Stage::e_dstE, Stage::e_valE, M_dstE, M_valE, W_dstE, W_valE, M_dstM, Stage::m_valM, W_dstM, W_valM, valP);

   calculateControlSignals(E_icode, E_dstM);

   setEInput(ereg, stat, icode, ifun, valC, valA, valB, dstE, dstM, Stage::d_srcA, Stage::d_srcB);

   return false;
}

/* 
 * 
 * Sets all needed information for the next stage of the pipeline * 
 * 
*/
void DecodeStage::setEInput(PipeReg * ereg, uint64_t stat, uint64_t icode,
                           uint64_t ifun, uint64_t valC, uint64_t valA,
                           uint64_t valB, uint64_t dstE, uint64_t dstM, uint64_t srcA,
                           uint64_t srcB)
{
   ereg->set(E_STAT, stat);
   ereg->set(E_ICODE, icode);
   ereg->set(E_IFUN, ifun);
   ereg->set(E_VALC,valC);
   ereg->set(E_VALA, valA);
   ereg->set(E_VALB, valB);
   ereg->set(E_DSTE, dstE);
   ereg->set(E_DSTM, dstM);
   ereg->set(E_SRCA, srcA);
   ereg->set(E_SRCB, srcB);
}
/* doClockHigh
 *
 * applies the appropriate control signal to the F
 * and D register intances
 * 
 * @param: pipeRegs - array of the pipeline register (F, D, E, M, W instances)
*/
void DecodeStage::doClockHigh(PipeRegArray * pipeRegs)
{
   PipeReg * ereg = pipeRegs->getExecuteReg();
   if (E_bubble)
   {
      ((E *)ereg)->bubble();
   }
   else
   {
      ereg->normal();
   }
}
/* 
 * 
 * Gathers source A information from previous stage * 
 * 
*/
uint64_t DecodeStage::getSrcA(uint64_t icode, PipeReg * dreg)
{
   uint64_t d_rA = dreg->get(D_RA);
   uint64_t d_srcA;
   if (icode == Instruction::IRRMOVQ ||
       icode == Instruction::IRMMOVQ ||
       icode == Instruction::IOPQ ||
       icode == Instruction::IPUSHQ)
   {
         d_srcA = d_rA;
   }
   else if (icode == Instruction::IPOPQ ||
            icode == Instruction::IRET)
   {
      d_srcA = RegisterFile::rsp;
   }
   else 
   {
      d_srcA = RegisterFile::RNONE;
   }
   return d_srcA;
}


/* 
 * 
 * Gathers source B information from previous stage 
 * 
*/
uint64_t DecodeStage::getSrcB(uint64_t icode, PipeReg * dreg)
{
   uint64_t d_rB = dreg->get(D_RB);
   uint64_t d_srcB;
   
   if (icode == Instruction::IOPQ ||
       icode == Instruction::IRMMOVQ ||
       icode == Instruction::IMRMOVQ ||
       icode == Instruction::IADDQ)
   {
      d_srcB = d_rB;
   }
   else if (icode == Instruction::IPUSHQ ||
            icode == Instruction::IPOPQ ||
            icode == Instruction::ICALL ||
            icode == Instruction::IRET)
   {
      d_srcB = RegisterFile::rsp;
   }
   else
   {
      d_srcB = RegisterFile::RNONE;
   }
   return d_srcB;
}

/* 
 * 
 * Gathers destination E information from previous stage * 
 * 
*/
uint64_t DecodeStage::getDstE(uint64_t icode, PipeReg * dreg)
{
   uint64_t d_rB = dreg->get(D_RB);
   uint64_t dstE;
   if (icode == Instruction::IRRMOVQ ||
       icode == Instruction::IIRMOVQ ||
       icode == Instruction::IOPQ ||
       icode == Instruction::IADDQ)
   {
      dstE = d_rB;
   }
   else if (icode == Instruction::IPUSHQ ||
            icode == Instruction::IPOPQ ||
            icode == Instruction::ICALL ||
            icode == Instruction::IRET)
   {
      dstE = RegisterFile::rsp;
   }
   else 
   {
      dstE = RegisterFile::RNONE;
   }
   return dstE;
}

/* 
 * 
 * Gathers destination M information from previous stage * 
 * 
*/
uint64_t DecodeStage::getDstM(uint64_t icode, PipeReg * dreg)
{
   uint64_t d_rA = dreg->get(D_RA);
   uint64_t dstM;
   if(icode == Instruction::IMRMOVQ ||
      icode == Instruction::IPOPQ)
   {
         dstM = d_rA;
   }
   else
   {
      dstM = RegisterFile::RNONE;
   }
   return dstM;
}

/* 
 * 
 * Gathers the Forwarding A information to help select for next stage from previous stage * 
 * 
*/
uint64_t DecodeStage::getSelPlusFwdA(uint64_t icode, uint64_t srcA, bool &error, uint64_t e_dstE, 
                              uint64_t e_valE, uint64_t M_dstE, uint64_t M_valE, 
                              uint64_t W_dstE, uint64_t W_valE, uint64_t M_dstM,
                              uint64_t m_valM, uint64_t W_dstM, uint64_t W_valM, uint64_t D_valP)
{
   uint64_t d_valA = 0;
   if (icode == Instruction::ICALL || icode == Instruction::IJXX)
   {
      return D_valP;
   }
   if (srcA == RegisterFile::RNONE) 
   {
      return d_valA;
   }
   if (srcA == e_dstE)
   {
      d_valA = e_valE;
   }
   else if (srcA == M_dstM)
   {
      d_valA = m_valM;
   }
   else if (srcA == M_dstE)
   {
      d_valA = M_valE;
   }
   else if (srcA == W_dstM)
   {
      d_valA = W_valM;
   }
   else if (srcA == W_dstE)
   {
      d_valA = W_valE;
   }
   else
   {
      d_valA = Stage::rf->readRegister(srcA, error);
   }

   return d_valA;
}

/* 
 * 
 * Gathers the Forwarding B information to help select for next stage from previous stage * 
 * 
*/
uint64_t DecodeStage::getFwdB(uint64_t srcB, bool & error, uint64_t e_dstE, 
                              uint64_t e_valE, uint64_t M_dstE, uint64_t M_valE, 
                              uint64_t W_dstE, uint64_t W_valE, uint64_t M_dstM,
                              uint64_t m_valM, uint64_t W_dstM, uint64_t W_valM)
{
   uint64_t d_valB;
   if (srcB == RegisterFile::RNONE) return 0;
   if (srcB == e_dstE)
   {
      d_valB = e_valE;
   }
   else if (srcB == M_dstM)
   {
      d_valB = m_valM;
   }
   else if (srcB == M_dstE)
   {
       d_valB = M_valE;
   }
   else if (srcB == W_dstM)
   {
      d_valB = W_valM;
   }
   else if (srcB == W_dstE)
   {
       d_valB = W_valE;
   }
   else
   {
      d_valB = Stage::rf->readRegister(srcB, error);
   }
   return d_valB;
}

/* 
 * 
 * Gathers the function used to help determine if E needs to bubble or not* 
 * 
*/
bool DecodeStage::getE_bubble(uint64_t E_icode, uint64_t E_dstM)
{
   if ((E_icode == Instruction::IJXX && !Stage::e_Cnd) ||
      ((E_icode == Instruction::IMRMOVQ || E_icode == Instruction::IPOPQ) &&
       (E_dstM == Stage::d_srcA || E_dstM == Stage::d_srcB)))
   {
      return true;
   }
   return false;
}

/* 
 * 
 * Gathers the preps the next stage with needed Control Signals based on info provided * 
 * 
*/
void DecodeStage::calculateControlSignals(uint64_t E_icode, uint64_t E_dstM)
{
   E_bubble = getE_bubble(E_icode, E_dstM);
}