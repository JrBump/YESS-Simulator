#include <cstdint>
#include <cstdlib>
#include <string>
#include "Tools.h"

/*
 * Hints/Notes:
 * 1) Pay attention to what the comments say. 
 * 2) Sometimes a function returns the source input if input validation
 *    fails and sometimes it returns 0.
 * 3) Pay attention to the "rules".
 * 4) You won't get credit for a method if it passes the tests but
 *    breaks the "rules".
 * 5) if the rule is no loops then you cannot use while, do-while, for
 * 6) if the rule is no conditionals then you cannot use if, if-else, 
 *    switch or ternary statements 
 * 7) The last 6 function restrict the operators that can be used. If
 *    used any other operators than those, you will not receive credit.
 * 8) If you shift a variable by an amount that is equal to or
 *    larger than its size, then the shift doesn't happen. For example, 
 *    if n is a 64 bit unsigned int then n >> 64 is n (not 0).
 * 9) If you need a 64 bit constant, you need to follow the constant
 *    by either ul (unsigned long), l (long), ull (unsigned long long),
 *    or ll (long long). For example, 3ul, 3l, 3ull, 3ll are all eight bytes.
 *    By default, 3 without the suffix is a 32 bit int.
 * 10) >> (right shift) shifts off low order bits
 * 11) << (left shift) shifts off high order bits
 * 12) right shift of a signed variable maintains the sign bit; 
 *     right shift of an unsigned variable brings in 0 as the sign bit
 *
 */

/** 
 * builds a 64-bit long out of an array of 8 bytes
 *
 * @param array of 8 bytes
 * @return uint64_t where the low order byte is bytes[0] and
 *         the high order byte is bytes[7]
 *
 * RULES: 
 *   1) no more than ten lines of code
 *   2) use a loop
*/
uint64_t Tools::buildLong(uint8_t bytes[LONGSIZE])
{
  uint64_t value = 0;
  for (int i = LONGSIZE - 1; i >= 0; i --) 
  {
      value = value | ((uint64_t)bytes[i]) << (8 * i);
  }
  return value;
}

/** 
 * accepts as input an uint64_t and returns the designated byte
 * within the uint64_t; returns 0 if the indicated byte number
 * is out of range 
 *
 * for example, getByte(0x1122334455667788, 7) returns 0x11
 *              getByte(0x1122334455667788, 1) returns 0x77
 *              getByte(0x1122334455667788, 8) returns 0
 *
 * @param uint64_t source that is the source data
 * @param int32_t byteNum that indicates the byte to return (0 through 7)
 *                byte 0 is the low order byte
 * @return 0 if byteNum is out of range
 *         byte 0, 1, .., or 7 of source if byteNum is within range
 *
 * RULES:
 * 1) you can use an if to handle error checking on input
 * 2) no loops or conditionals (other than for 1) or switch
*/
uint64_t Tools::getByte(uint64_t source, int32_t byteNum)
{
  if ((byteNum < 0) || (byteNum > 7)) return 0;
  return ((uint64_t(source >> byteNum * 8) & 0xFF));
}

/**
 * accepts as input an uint64_t and returns the bits low through 
 * high of the uint64_t. bit 0 is the low order bit and bit 63
 * is the high order bit. returns 0 if the low or high bit numbers 
 * are out of range
 *
 * for example, getBits(0x8877665544332211, 0, 7) returns 0x11
 *              getBits(0x8877665544332211, 4, 11) returns 0x21
 *              getBits(0x8877665544332211, 0, 63) returns 0x8877665544332211
 *              getBits(0x8877665544332211, 0, 64) returns 0
 *
 * @param uint64_t source that holds the bits to be grabbed and 
 *        returned
 * @param int32_t low that is the bit number of the lowest numbered
 *        bit to be returned
 * @param int32_t high that is the bit number of the highest numbered
 *        bit to be returned
 * @return an uint64_t that holds a subset of the source bits
 *         that is returned in the low order bits; returns 0 if low or high 
 *         is out of range
 *
 * RULES:
 * 1) you can use an if to handle error checking on input
 * 2) no loops or conditionals (other than for 1) or switch
 */
uint64_t Tools::getBits(uint64_t source, int32_t low, int32_t high)
{
  if (low < 0 || high > 63 || low > high) 
    return 0;

  uint64_t numBits = high - low + 1;
  uint64_t mask = ~0ULL >> (64 - numBits);
  uint64_t shifted = source >> low;

  return shifted & mask;
}



/**
 * sets the bits of source in the range specified by the low and high
 * parameters to 1 and returns that value. returns source if the low or high
 * bit numbers are out of range
 *
 * for example, setBits(0x1122334455667788, 0, 7) returns 0x11223344556677ff
 *              setBits(0x1122334455667788, 8, 0xf) returns 0x112233445566ff88
 *              setBits(0x1122334455667788, 8, 64) returns 0x1122334455667788
 *                      note: 64 is out of range
 *
 * @param uint64_t source 
 * @param int32_t low that is the bit number of the lowest numbered
 *        bit to be set to 1
 * @param int32_t high that is the bit number of the highest numbered
 *        bit to be set to 1
 * @return an uint64_t that holds the modified source
 *
 * RULES:
 * 1) you can use an if to handle error checking on input
 * 2) no loops or conditionals (other than for 1) or switch
 * 3) you can use other functions you have written, for example, getBits
 */
uint64_t Tools::setBits(uint64_t source, int32_t low, int32_t high)
{
  if (low < 0 || high > 63 || low > high) 
        return source;

  int numBits = high - low + 1;
  uint64_t mask = ((1ULL << (numBits - 1)) - 1) << low;
  mask |= (1ULL << (high));

  return source |= mask;
}

/**
 * clears the bits of source in the range low to high to 0 (clears them) and
 * returns that value. returns source if the low or high
 * bit numbers are out of range
 *
 * for example, clearBits(0x1122334455667788, 0, 7) returns 0x1122334455667700
 *              clearBits(0x1122334455667788, 8, f) returns 0x1122334455660088
 *
 * @param uint64_t source 
 * @param int32_t low that is the bit number of the lowest numbered
 *        bit to be set to 0
 * @param int32_t high that is the bit number of the highest numbered
 *        bit to be set to 0
 * @return an uint64_t that holds the modified source
 *
 * RULES:
 * 1) you can use an if to handle error checking on input
 * 2) no loops or conditionals (other than for 1) or switch
 * 3) you can use other functions you have written, for example, getBits
 */
uint64_t Tools::clearBits(uint64_t source, int32_t low, int32_t high)
{
  if (low < 0 || high > 63 || low > high) 
        return source;
  int numBits = high - low + 1;
  uint64_t mask = ((1ULL << (numBits - 1)) - 1) << low;
  mask |= (1ULL << (high));
  mask = ~mask;
  return source &= mask;
}

/**
 * assumes source contains a 64 bit two's complement value and
 * returns the sign (1 or 0)
 *
 * for example, sign(0xffffffffffffffff) returns 1
 *              sign(0x0000000000000000) returns 0
 *              sign(0x8000000000000000) returns 1
 *
 * @param uint64_t source
 * @return 1 if source is negative when treated as a two's complement 
 *         value and 0 otherwise
 *
 * RULES:
 * 1) no loops or conditionals
 * 2) you can use other functions you have written, for example, getBits
 */
uint64_t Tools::sign(uint64_t source)
{
  return source >> 63;
}

/**
 * assumes that op1 and op2 contain 64 bit two's complement values
 * and returns true if an overflow would occur if they are summed
 * and false otherwise
 *
 * for example, addOverflow(0x8000000000000000, 0x8000000000000000) returns 1
 *              addOverflow(0x7fffffffffffffff, 0x7fffffffffffffff) returns 1
 *              addOverflow(0x8000000000000000, 0x7fffffffffffffff) returns 0
 *
 * @param uint64_t op1 that is one of the operands of the addition
 * @param uint64_t op2 that is the other operand of the addition
 * @return true if op1 + op2 would result in an overflow assuming that op1
 *         and op2 contain 64-bit two's complement values
 *
 * RULES:
 * 1) no loops 
 * 2) you can use other functions you have written, for example, sign
 * 3) no more than 10 lines of code
 */
bool Tools::addOverflow(uint64_t op1, uint64_t op2)
{
  //Hint: If an overflow occurs then it overflows by just one bit.
  //      In other words, 65 bits would be needed to store the arithmetic 
  //      result instead of 64 and the sign bit in the stored result (bit 63) is incorrect. 
  //      Thus, the way to check for an overflow is to compare the signs of the
  //      operand and the result.  For example, if you add two positive numbers, 
  //      the result should be positive, otherwise an overflow occurred.
  uint64_t resultSign = sign(op1 + op2);
  uint64_t op1Sign = sign(op1);
  uint64_t op2Sign = sign(op2);
  return ((op1Sign == op2Sign) && (resultSign != op1Sign));
}

/**
 * assumes that op1 and op2 contain 64 bit two's complement values
 * and returns true if an overflow would occur from op2 - op1
 * and false otherwise
 *
 * for example, subOverflow(0x8000000000000000, 0x8000000000000000) returns 0
 *              subOverflow(0x7fffffffffffffff, 0x7fffffffffffffff) returns 0
 *              subOverflow(0x8000000000000000, 0x7fffffffffffffff) returns 1
 *
 * @param uint64_t op1 that is one of the operands of the subtraction
 * @param uint64_t op2 that is the other operand of the subtraction
 * @return true if op2 - op1 would result in an overflow assuming that op1
 *         and op2 contain 64-bit two's complement values
 *
 * RULES:
 * 1) no loops 
 * 2) you can use other functions you have written, for example, sign
 * 3) you cannot use addOverflow (it doesn't work in all cases).
 * 4) no more than 10 lines of code
 */
bool Tools::subOverflow(uint64_t op1, uint64_t op2)
{
  //See hint for addOverflow
  //Note: you can not simply use addOverflow in this function.  If you negate
  //op1 in order to an add, you may get an overflow. 
  //NOTE: the subtraction is op2 - op1 (not op1 - op2).
  uint64_t resultSign = sign(op2 - op1);
  uint64_t op1Sign = sign(op1);
  uint64_t op2Sign = sign(op2);
  return ((op1Sign != op2Sign) && (op1Sign == resultSign));
}


//All of the functions above are needed later this semester for
//your simulator. This next set is for fun and to help you better
//understand two's complement and bit operations.

/**
 * returns the largest magnitude positive value that can be stored
 * in a 64-bit two's complement number. (largest magnitude means furthest
 * from 0)
 *
 * RULES:
 * 1) no loops or conditionals
 * 2) no casts
 * 3) any constants you use must be between 0 and 255 (inclusive)
 * 4) can only use these operators: !, ~, >>, <<, |, ||, &, &&
*/
uint64_t Tools::maxPos()
{
   return ~(1Ull << 63);
}

/**
 * returns the largest magnitude negative value that can be stored
 * in a 64-bit two's complement number. (largest magnitude means furthest
 * from 0)
 *
 * RULES:
 * 1) no loops or conditionals
 * 2) no casts
 * 3) any constants you use must be between 0 and 255 (inclusive)
 * 4) can only use these operators: !, ~, >>, <<, |, ||, &, &&
*/
uint64_t Tools::maxNeg()
{
   return 1Ull << 63;
}

/**
 * returns true (1) if op is <= 0 when treated as
 * as a 64 bit two's complement values
 *
 * for example, isLessThanOrEq0(0x8000000000000000) returns 1
 *              isLessThanOrEq0(0x7fffffffffffffff) returns 0
 *              isLessThanOrEq0(0x0000000000000000) returns 1
 *              isLessThanOrEq0(0xFFFFFFFFFFFFFFFF) returns 1
 *
 * @param uint64_t op contains a 64-bit two's complement number
 * @return true if (int64_t) op <= 0 and false otherwise
 *
 * RULES:
 * 1) no loops or conditionals
 * 2) can use a cast
 * 3) any constants you use must be between 0 and 255 (inclusive)
 * 4) can only use these operators: !, ~, >>, <<, |, ||, &, &&
 */
bool Tools::isLessThanOrEq0(uint64_t op)
{
   return (op >> 63 | (!op));
}

/**
 * returns true (1) if op is > 2 when treated as
 * as a 64 bit two's complement values
 *
 * for example, isGreaterThanTwo(0x8000000000000000) returns 0
 *              isGreaterThanTwo(0x7fffffffffffffff) returns 1
 *              isGreaterThanTwo(0x0000000000000002) returns 0
 *              isGreaterThanTwo(0x0000000000000003) returns 1
 *
 * @param uint64_t op contains a 64-bit two's complement number
 * @return true if (int64_t) op > 2 and false otherwise
 *
 * RULES:
 * 1) no loops or conditionals
 * 2) can use a cast
 * 3) any constants you use must be between 0 and 255
 * 4) can only use these operators: !, ~, >>, <<, |, ||, &, &&
 */
bool Tools::isGreaterThanTwo(uint64_t op)
{
    return static_cast<int64_t>(op) > 2;
}

/**
 * takes as input a 64-bit two's complement number and returns that
 * number with its even numbered bits (bits 0, 2, 4, 6, ..., 60, 62) flipped.
 * In other words, if the bit is 0 it becomes 1 and if the bit is 1, it
 * becomes 0.
 *
 * for example, flipEvenBits(0xFFFFFFFFFFFFFFFF) returns 0xAAAAAAAAAAAAAAAA
 *              flipEvenBits(0xFAFAFAFAFAFAFAFA) returns 0xAFAFAFAFAFAFAFAF
 *              flipEvenBits(0) returns 0x5555555555555555
 *
 * @param uint64_t op contains a 64-bit two's complement number
 * @return op with even numbered bits flipped
 *
 * RULES:
 * 1) no loops or conditionals
 * 2) can use a cast
 * 3) any constants you use must be between 0 and 255
 * 4) can only use these operators: >>, <<, |, ||, &, &&, ^
 */
uint64_t Tools::flipEvenBits(uint64_t op)
{
   uint64_t base = 0x55;
   uint64_t mask = base | base << 8 | base << 16| base << 24;
   mask |= mask << 32;
   return op ^ mask;
}

/**
 * takes as input a 64-bit two's complement number and returns that
 * true if 2 can be added to the number without an overflow occurring.
 *
 * for example, addTwoOK(0xFFFFFFFFFFFFFFFF) returns 1
 *              addTwoOK(0x7FFFFFFFFFFFFFFF) returns 0
 *
 * @param uint64_t op contains a 64-bit two's complement number
 * @return true if (int64_t) op + 2 doesn't overflow and false otherwise
 *
 * RULES:
 * 1) no loops or conditionals
 * 2) can use a cast
 * 3) any constants you use must be between 0 and 255
 * 4) can only use these operators: >>, <<, |, ||, &, &&, ^, !, ~
 */
bool Tools::addTwoOK(uint64_t op)
{
   return !addOverflow(op, 2);
}

