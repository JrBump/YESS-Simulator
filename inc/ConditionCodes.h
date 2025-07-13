#ifndef CONDITIONCODES_H
#define CONDITIONCODES_H

class ConditionCodes 
{
   private:
      static ConditionCodes * ccInstance;
      ConditionCodes();
      uint64_t codes;
   public:
      //Be sure to use SF, OF, or ZF to select the bit
      //Don't use a "magic" number
      //I made these strange values so that things will go wrong
      //if you don't use the names SF, OF, and ZF
      static const int SF = 7;
      static const int OF = 3;
      static const int ZF = 2;
      static ConditionCodes * getInstance();      
      bool getConditionCode(int32_t ccNum, bool & error);
      void setConditionCode(bool value, int32_t ccNum, 
                            bool & error);
      void dump();
}; 

#endif
