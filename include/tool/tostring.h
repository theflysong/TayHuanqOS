/**
 * @file tostring.h
 * @author theflysong (song_of_the_fly@163.com)
 * @brief xx转字符串
 * @version alpha-1.0.0
 * @date 2022-10-22
 *
 * @copyright Copyright (c) 2022 TayhuangOS Development Team
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */



#pragma once

#include <__header.h>

__C_HEADER_START;

EXPORT char *itoa(int val, char *buffer, int base);
EXPORT char *uitoa(unsigned int val, char *buffer, int base);
EXPORT char *lltoa(long long val, char *buffer, int base);
EXPORT char *ulltoa(unsigned long long val, char *buffer, int base);
EXPORT char *ftoa(float val, char *buffer, int round);
EXPORT char *dtoa(double val, char *buffer, int round);
EXPORT char *ftoea(float val, char *buffer, unsigned char upper_e);
EXPORT char *dtoea(double val, char *buffer, unsigned char upper_e);

__C_HEADER_END;