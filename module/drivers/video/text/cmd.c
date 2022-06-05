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
 * cmd.c
 *
 * 命令处理器
 *
 */



#include "cmd.h"

#include <debug/logging.h>

#include <infomations.h>

#include <ipc/ipc.h>

void write_char(int ch, int color, int x, int y) {
    *(char*)(VIDEO_MEMORY + (y * screen_width + x) * 2) = ch;
    *(char*)(VIDEO_MEMORY + 1 + (y * screen_width + x) * 2) = color;
}

void deal_text_cmd(int caller, int cmd, qword *param) {
    switch (cmd)
    {
    case __TEXT_WRITE_CHAR: {
        int ch = param[0];
        int color = param[1];
        int x = param[2];
        int y = param[3];
        write_char(ch, color, x, y);
        break;
    }
    case __TEXT_WRITE_STRING: {
        int color = param[0];
        int x = param[1];
        int y = param[2];
        char _buffer[256];
        char *buffer = _buffer;
        recv_msg(buffer, caller);

        while (*buffer != '\0') {
            write_char(*buffer, color, x, y);
            buffer ++;
            x ++;
            y += (x / 80);
            x %= 80;
        }
    }
    default:
        break;
    }
}