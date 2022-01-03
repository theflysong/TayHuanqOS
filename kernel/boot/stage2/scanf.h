// SPDX-License-Identifier: GPL-2.0-only
/* -------------------------------*-TayhuangOS-*-----------------------------------
 *
 *   Copyright (C) 2022, 2022 TayhuangOS Development Team - All Rights Reserved
 *
 * --------------------------------------------------------------------------------
 *
 * Author: Flysong
 *
 * kernel/boot/stage2/scanf.h
 *
 * Input functions are declared here
 */



#pragma once

#include "header.h"

PUBLIC int getchar(void);
PUBLIC int scanf(const char* format, ...);
typedef int(*rdkey_t)(void);
PUBLIC int vscanf(rdkey_t rd_func, const char* format, ...);