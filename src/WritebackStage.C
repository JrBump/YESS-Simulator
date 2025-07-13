#include "PipeRegArray.h"
#include "WritebackStage.h"
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
bool WritebackStage::doClockLow(PipeRegArray * pipeRegs)
{
   PipeReg * wreg = pipeRegs->getWritebackReg();

   uint64_t stat = wreg->get(W_STAT);
   

   if (stat != Status::SAOK) return true;
   else return false;
}

/* doClockHigh
 *
 * applies the appropriate control signal to the F
 * and D register intances
 * 
 * @param: pipeRegs - array of the pipeline register (F, D, E, M, W instances)
*/
void WritebackStage::doClockHigh(PipeRegArray * pipeRegs)
{
   PipeReg * wreg = pipeRegs->getWritebackReg();
   uint64_t W_valE = wreg->get(W_VALE);
   uint64_t W_dstE = wreg->get(W_DSTE);
   uint64_t W_valM = wreg->get(W_VALM);
   uint64_t W_dstM = wreg->get(W_DSTM);
   RegisterFile * regFile = RegisterFile::getInstance();
   bool error = false;
   regFile->writeRegister(W_valE, W_dstE, error);
   regFile->writeRegister(W_valM, W_dstM, error);
}



