/*******************************************************************************
 * Copyright (c) 2020, STMicroelectronics - All Rights Reserved
 *
 * This file is part of the VL53L5CX Ultra Lite Driver and is dual licensed,
 * either 'STMicroelectronics Proprietary license'
 * or 'BSD 3-clause "New" or "Revised" License' , at your option.
 *
 * Modified for Adafruit BusIO by Limor 'ladyada' Fried with assistance from
 * Claude Code
 *
 *******************************************************************************/

#include "platform.h"
#include <Arduino.h>

// Maximum I2C transaction size (allowing 2 bytes for register address)
#define I2C_MAX_TRANSFER 128

uint8_t RdByte(VL53L5CX_Platform *p_platform, uint16_t RegisterAddress,
               uint8_t *p_value) {
  uint8_t reg[2];
  reg[0] = (RegisterAddress >> 8) & 0xFF;
  reg[1] = RegisterAddress & 0xFF;

  if (!p_platform->i2c_dev->write_then_read(reg, 2, p_value, 1)) {
    return 1; // Error
  }
  return 0;
}

uint8_t WrByte(VL53L5CX_Platform *p_platform, uint16_t RegisterAddress,
               uint8_t value) {
  uint8_t buffer[3];
  buffer[0] = (RegisterAddress >> 8) & 0xFF;
  buffer[1] = RegisterAddress & 0xFF;
  buffer[2] = value;

  if (!p_platform->i2c_dev->write(buffer, 3)) {
    return 1; // Error
  }
  return 0;
}

uint8_t RdMulti(VL53L5CX_Platform *p_platform, uint16_t RegisterAddress,
                uint8_t *p_values, uint32_t size) {
  uint8_t reg[2];
  uint32_t bytesRemaining = size;
  uint32_t offset = 0;

  while (bytesRemaining > 0) {
    uint32_t toRead = bytesRemaining;
    if (toRead > I2C_MAX_TRANSFER) {
      toRead = I2C_MAX_TRANSFER;
    }

    reg[0] = ((RegisterAddress + offset) >> 8) & 0xFF;
    reg[1] = (RegisterAddress + offset) & 0xFF;

    if (!p_platform->i2c_dev->write_then_read(reg, 2, p_values + offset,
                                              toRead)) {
      return 1; // Error
    }

    offset += toRead;
    bytesRemaining -= toRead;
  }
  return 0;
}

uint8_t WrMulti(VL53L5CX_Platform *p_platform, uint16_t RegisterAddress,
                uint8_t *p_values, uint32_t size) {
  uint32_t bytesRemaining = size;
  uint32_t offset = 0;
  uint8_t buffer[I2C_MAX_TRANSFER + 2]; // 2 bytes for register address

  while (bytesRemaining > 0) {
    uint32_t toWrite = bytesRemaining;
    if (toWrite > I2C_MAX_TRANSFER) {
      toWrite = I2C_MAX_TRANSFER;
    }

    buffer[0] = ((RegisterAddress + offset) >> 8) & 0xFF;
    buffer[1] = (RegisterAddress + offset) & 0xFF;
    memcpy(buffer + 2, p_values + offset, toWrite);

    if (!p_platform->i2c_dev->write(buffer, toWrite + 2)) {
      return 1; // Error
    }

    offset += toWrite;
    bytesRemaining -= toWrite;
  }
  return 0;
}

void SwapBuffer(uint8_t *buffer, uint16_t size) {
  uint32_t i, tmp;

  for (i = 0; i < size; i = i + 4) {
    tmp = (buffer[i] << 24) | (buffer[i + 1] << 16) | (buffer[i + 2] << 8) |
          (buffer[i + 3]);
    memcpy(&(buffer[i]), &tmp, 4);
  }
}

uint8_t WaitMs(VL53L5CX_Platform *p_platform, uint32_t TimeMs) {
  (void)p_platform; // Unused
  delay(TimeMs);
  return 0;
}
