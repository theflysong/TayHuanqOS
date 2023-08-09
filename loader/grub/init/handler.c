/**
 * @file handler.c
 * @author theflysong (song_of_the_fly@163.com)
 * @brief 中断处理器
 * @version alpha-1.0.0
 * @date 2023-06-07
 * 
 * @copyright Copyright (c) 2022 TayhuangOS Development Team
 * SPDX-License-Identifier: LGPL-2.1-only
 * 
 */

#include <init/handler.h>
#include <basec/logger.h>
#include <stddef.h>
#include <tay/ports.h>
#include <tay/io.h>

/**
 * @brief 启用中断
 * 
 */
void EnableInterrupt(void) {
    asm volatile ("sti");
}

/**
 * @brief 禁止中断
 * 
 */
void DisableInterrupt(void) {
    asm volatile ("cli");
}

//禁用/启用 IRQ
static void DisableIRQ(int irq) {
    if (irq < 8) {
        //主片
        byte i = inb(M_PIC_BASE + PIC_DATA); 
        i |= (1 << irq);
        outb(M_PIC_BASE + PIC_DATA, i);
    }
    else {
        //从片
        byte i = inb(S_PIC_BASE + PIC_DATA); 
        i |= (1 << (irq - 8));
        outb(S_PIC_BASE + PIC_DATA, i);
    }
}

static void EnableIRQ(int irq) {
    if (irq < 8) {
        //主片
        byte i = inb(M_PIC_BASE + PIC_DATA); 
        i &= ~(1 << irq);
        outb(M_PIC_BASE + PIC_DATA, i);
    }
    else {
        //从片
        byte i = inb(S_PIC_BASE + PIC_DATA); 
        i &= ~(1 << (irq - 8));
        outb(S_PIC_BASE + PIC_DATA, i);
    }
}

#define PIC_EOI (0x20)

//发送EOI
static void SendEOI(int irq) {
    if (irq > 8) {
        //从片EOI
        outb (S_PIC_BASE + PIC_CONTROL, PIC_EOI); 
    }
    //主片EOI
    outb (M_PIC_BASE + PIC_CONTROL, PIC_EOI); 
}

static int ticks = 0;

static bool clockHandler(int irq, IStack *stack) {
    ticks ++;
    LogInfo("Ticks=%d", ticks);

    return false;
}

/**
 * @brief IRQ处理器
 * 
 */
IRQHandler irqHandlers[32] = {
    [0] = clockHandler
};

/**
 * @brief IRQ主处理程序
 * 
 * @param irq irq号
 * @param stack 堆栈
 */
void PrimaryIRQHandler(int irq, IStack *stack) {
    DisableIRQ(irq); 

    //发送EOI
    SendEOI(irq);

    LogInfo("接收到IRQ=%02X", irq);

    if (irqHandlers[irq] != NULL) {
        if (! irqHandlers[irq](irq, stack)) {
            LogError("解决IRQ=%02X失败!", irq);
            return; //不再开启该中断
        }
    }

    //启用同类中断接收
    EnableIRQ(irq); 
}

//---------------------------------------------

int errcode;

// 异常信息
static const char *exceptionMessage[] = {
    "[#DE] 除以0!",
    "[#DB] 单步调试",
    "[无] NMI中断!",
    "[#BP] 断点",
    "[#OF] 溢出!",
    "[#BR] 越界!",
    "[#UD] 无效的操作码(未定义的指令)!",
    "[#NM] 设备不可用(没有数学协处理器)!",
    "[#DF] 双重错误!",
    "[无] 协处理器段溢出!",
    "[#TS] 无效TSS!",
    "[#NP] 缺少段!",
    "[#SS] 缺少栈段!",
    "[#GP] 通用保护错误!",
    "[#PF] 缺页中断!",
    "[保留] 保留!",
    "[#MF] x87数学协处理器浮点运算错误!",
    "[#AC] 对齐检测!",
    "[#MC] 机器检测!",
    "[#XF] SIMD浮点运算错误!",
    "[#VE] 虚拟化异常!",
    "[#CP] 控制保护错误!",
    "[保留] 保留!",
    "[保留] 保留!",
    "[保留] 保留!",
    "[保留] 保留!",
    "[保留] 保留!",
    "[保留] 保留!",
    "[#HV] Hypervisor注入异常!",
    "[#VC] VMM通信异常!",
    "[#SX] 安全性错误!",
    "[保留] 保留!"
};

typedef bool(*ExceptionSolution)(void);

static const ExceptionSolution solutionList[] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

/**
 * @brief 异常主处理程序
 * 
 * @param errno 异常号
 * @param stack 堆栈
 */
void PrimaryExceptionHandler(int errno, IStack *stack) {
    LogError("在%04X:%08X处发生错误:", stack->cs, stack->eip);
    LogError("%s", exceptionMessage[errno]);

    if (errcode != 0xFFFFFFFF) {
        LogError("Error Code = %08X", errcode);
    }

    LogError("现场已保存:");

    LogError("eax: %08X ; ebx: %08X ; ecx: %08X ; edx: %08X", stack->eax, stack->ebx, stack->ecx, stack->edx);
    LogError("esi: %08X ; edi: %08X ; esp: %08X ; ebp: %08X", stack->esi, stack->edi, stack->esp, stack->ebp);

    LogError(" ds: %04X     ;  es: %04X     ;  fs: %04X     ;  gs: %04X    ", stack->ds, stack->es, stack->fs, stack->gs);

    LogError("cr3: %08X ; eflags: %08X", stack->cr3, stack->eflags);

    if (solutionList[errno] != NULL) {
        if (! solutionList[errno]()) {
            LogFatal("解决异常%02X失败!", errno);
            while (true);
        }
    }
    else {
        LogFatal("无法解决异常%02X!", errno);
        while (true);
    }
}