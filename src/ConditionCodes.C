#include <iostream>
#include <iomanip>
#include "ConditionCodes.h"
#include "Tools.h"

//cc_instance will be initialized to reference the single 
//instance of ConditionCodes
ConditionCodes * ConditionCodes::ccInstance = NULL;

/**
 * ConditionCodes constructor
 * initialize the codes field to 0
 */
ConditionCodes::ConditionCodes()
{
   this->codes = 0;
}

/**
 * getInstance
 * if ccInstance is NULL then getInstance creates the
 * single ConditionCodes instance and sets ccInstance
 * to that. Otherwise, it returns ccInstance
 *
 * @return ccInstance
 */
ConditionCodes * ConditionCodes::getInstance()
{
   if (ConditionCodes::ccInstance == NULL)
   {
      ConditionCodes::ccInstance = new ConditionCodes();
   }
   return ccInstance;
}

/*
 * getConditionCode
 * accepts a condition code number (ConditionCodes::OF, ConditionCodes::SF, or ConditionCodes::ZF) 
 * and returns the value of the condition code from the codes data member
 *
 * @param ccNum equal to either ConditionCodes::OF, ConditionCodes::SF, or ConditionCodes::ZF
 * @return the value of bit ccNum out of codes. if ccNum is 
 *         out of range (not ConditionCodes::OF and not ConditionCodes::SF and 
 *         not ConditionCodes::ZF) then returns false (0)
 * @return error is set to true if ccNum is out of range and
 *         false otherwise
 */
bool ConditionCodes::getConditionCode(int32_t ccNum, bool & error)
{
   //Use your getBits in Tools.C to get the bit out of codes.
   //Don't use "magic" numbers.
   uint64_t hold = 0;
   if (ccNum != SF && ccNum != OF && ccNum != ZF)
   {
      error = true;
   } else
   {
      hold = Tools::getBits(codes, ccNum, ccNum);
      error = false;
   }

   return hold;
}

/*
 * setConditionCode
 * accepts a condition code number (ConditionCodes::OF, ConditionCodes::SF, or 
 * ConditionCodes::ZF) and value (true or false) and sets the condition code bit within
 * the codes data member to that value (1/true or 0/false). 
 * if the ccNum value is out of range (not ConditionCodes::OF and not ConditionCodes::SF 
 * and not ConditionCodes::ZF) then codes does not get modified. 
 *
 * @param value to set the condition code bit to (true/1 or false/0)
 * @param ccNum condition code number, either ConditionCodes::OF, ConditionCodes::SF, or 
 *        ConditionCodes::ZF
 * @return error is set to true if ccNum is out of range and
 *         false otherwise
 */
void ConditionCodes::setConditionCode(bool value, int32_t ccNum, 
                                      bool & error)
{
   //Use your setBits and clearBits in Tools.C. 
   //Don't use "magic" numbers in your code.
      
   if (ccNum != SF && ccNum != OF && ccNum != ZF)
   {
      error = true;
   }  
   else
   {
      error = false;
      if (value)
      {
         codes = Tools::setBits(codes,ccNum,ccNum);
      } else
      {
         codes = Tools::clearBits(codes,ccNum,ccNum);
      }

   }   
   return;
}

/*
 * dump
 * outputs the values of the condition codes
 */
void ConditionCodes::dump()
{
   int32_t zf = Tools::getBits(codes, ConditionCodes::ZF, ConditionCodes::ZF);
   int32_t sf = Tools::getBits(codes, ConditionCodes::SF, ConditionCodes::SF);
   int32_t of = Tools::getBits(codes, ConditionCodes::OF, ConditionCodes::OF);
   std::cout << std::endl;
   std::cout << "ZF: " << std::hex << std::setw(1) << zf << " ";
   std::cout << "SF: " << std::hex << std::setw(1) << sf << " ";
   std::cout << "OF: " << std::hex << std::setw(1) << of << std::endl;
}
