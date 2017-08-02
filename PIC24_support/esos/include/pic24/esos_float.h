#pragma once
#include <esos.h>
#include <esos_pic24_rs232.h>
uint32_t ticksToNs(uint32_t u32_ticks, uint16_t u16_tmrPre);
uint16_t getTimerPrescaleBits(uint8_t u8_TCKPS);
uint32_t computeDeltaTicksLong(uint16_t u16_start, uint16_t u16_end, uint16_t u16_tmrPR, uint16_t u16_oflows);
uint16_t computeDeltaTicks(uint16_t u16_start, uint16_t u16_end, uint16_t u16_tmrPR);
uint32_t roundFloatToUint32(float f_x);
uint16_t roundFloatToUint16(float f_x);

