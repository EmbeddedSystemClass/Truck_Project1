#include <esos_float.h>

/** Converts timer ticks to nanoseconds
 *  \param u32_ticks  Timer ticks
 *  \param u16_tmrPre Timer prescale value
 *  \return time in nanoseconds
 */
uint32_t ticksToNs(uint32_t u32_ticks, uint16_t u16_tmrPre) {
  //because of the wide range of the numbers, use a float for precision
  float f_ticks;
  uint32_t u32_timeNs;

  f_ticks = u32_ticks;   //convert to float
  f_ticks = ((f_ticks*u16_tmrPre)/FCY)*1E9;
  u32_timeNs = roundFloatToUint32(f_ticks);  //back to int32_t
  return u32_timeNs;
}


/** Given the TCKPS bitfield, return the timer prescale encoded
 *  by these bits. Use \ref getTimerPrescale as a convenient
 *  way to extract the TCKPS bitfield from a TxCONbits SFT
 *  then call this function.
 *  \param u8_TCKPS TCKPS bitfield from the timer in question
 *  \return Prescale value.
 */
uint16_t getTimerPrescaleBits(uint8_t u8_TCKPS) {
  const uint16_t au16_prescaleValue[] = { 1, 8, 64, 256 };
//  ASSERT(u8_TCKPS <= 3);
  return au16_prescaleValue[u8_TCKPS];
}

/** Computes delta ticks between two Timer register captures
 * Assumes long time interval and thus has a parameter for tracking timer overflows
 *  \param u16_start  start tick
 *  \param u16_end    end tick
 *  \param u16_tmrPR  Timer period register
 *  \param u16_oflows number of timer overflows
 *  \return delta ticks
 */
uint32_t computeDeltaTicksLong(uint16_t u16_start, uint16_t u16_end, uint16_t u16_tmrPR, uint16_t u16_oflows) {
  uint32_t u32_deltaTicks;
  if (u16_oflows == 0) u32_deltaTicks = u16_end - u16_start;
  else {
    //compute ticks from start to timer overflow
    u32_deltaTicks = (u16_tmrPR + 1) - u16_start;
    //add ticks due to overflows =  (overflows -1) * ticks_per_overflow
    u32_deltaTicks += ((((uint32_t) u16_oflows)- 1) * (((uint32_t)u16_tmrPR) + 1)) ;
    //now add in the delta due to the last capture
    u32_deltaTicks += u16_end;
  }
  return u32_deltaTicks;
}

/** Computes delta ticks between two Timer register captures
 * Assumes the delta time does not exceeds the timer period
 *  \param u16_start  start tick
 *  \param u16_end    end tick
 *  \param u16_tmrPR  Timer period register
 *  \return delta ticks
 */
uint16_t computeDeltaTicks(uint16_t u16_start, uint16_t u16_end, uint16_t u16_tmrPR) {
  uint16_t u16_deltaTicks;
  if (u16_end >= u16_start) u16_deltaTicks = u16_end - u16_start;
  else {
    //compute ticks from start to timer overflow
    u16_deltaTicks = (u16_tmrPR + 1) - u16_start;
    //now add in the delta from overflow to u16_end
    u16_deltaTicks += u16_end;
  }
  return u16_deltaTicks;
}

/** Round a floating-point number to the nearest integer.
 *  \param f_x Floating-point value to round
 *  \return The nearest uint32_t to f_x.
 */
uint32_t roundFloatToUint32(float f_x) {
  uint32_t u32_y;

  u32_y = f_x;
  if ((f_x - u32_y) < 0.5) return u32_y;
  else return u32_y+1;
}

/** Round a floating-point number to the nearest integer.
 *  \param f_x Floating-point value to round
 *  \return The nearest uint16_t to f_x.
 */
uint16_t roundFloatToUint16(float f_x) {
  uint16_t u16_y;

  u16_y = f_x;
  if ((f_x - u16_y) < 0.5) return u16_y;
  else return u16_y+1;
}

