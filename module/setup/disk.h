/*
 * SPDX-License-Identifier: GPL-3.0-only
 * -------------------------------*-TayhuangOS-*-----------------------------------
 *
 *    Copyright (C) 2022, 2022 TayhuangOS Development Team - All Rights Reserved
 *
 * --------------------------------------------------------------------------------
 *
 * 作者: Flysong
 *
 * disk.h
 *
 * 硬盘操作
 *
 */



#pragma once

#include <types.h>

#define DISK_SEL_IDE0 (0)
#define DISK_SEL_IDE1 (1)
#define DISK_SEL_MASTER (0)
#define DISK_SEL_SLAVE (2)
#define DISK_SEL_IDE0_MASTER (DISK_SEL_IDE0 | DISK_SEL_MASTER)
#define DISK_SEL_IDE0_SLAVE (DISK_SEL_IDE0 | DISK_SEL_SLAVE)
#define DISK_SEL_IDE1_MASTER (DISK_SEL_IDE1 | DISK_SEL_MASTER)
#define DISK_SEL_IDE1_SLAVE (DISK_SEL_IDE1 | DISK_SEL_SLAVE)

void load_module(const char *name, void *addr);
void identify_disk(int selector, void *dst);
void read_sector(dword lba, int num, int selector, void *dst);