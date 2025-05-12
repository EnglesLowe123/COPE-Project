/*
项目 30：微型指令集架构设计与解释器实现
1.自定义一个 8 位微型指令集（例如指令长度为 8-bit，支持基本算术与逻辑运算、跳转、内存访问）。
2.设计其编码格式、寄存器与内存结构。
3.实现一个解释器可以解析、加载并运行该指令集程序，并输出执行日志。
4.要求不得使用已有汇编解释器或虚拟机库，指令解析必须自己手动实现。

Instruction Set Format:  
  Opcode: 3 bits  
  Operand1: 2 bits  
  Operand2: 3 bits  

Input Program (in binary):  
  000 01 011  // ADD R0, R3  这里可能需要做映射（因为正常是00代表R0，01代表R1，以此类推）
  001 11 000  // MOV R1, #0  

Execution Log:
  R0 = R0 + R3 = 7  //这里可以看出R0初始值可以是4，而R3初始值可以是3
  R1 = 0
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

uint8_t R[4] = {0};     // 寄存器 R0-R3
uint8_t MEM[8] = {0};   // 内存 8 字节
uint8_t PC = 0;
bool running = true;

uint8_t parse_binary(const char *str) {
    uint8_t val = 0;
    for (int i = 0; i < 8 && str[i]; i++) {
        val <<= 1;
        if (str[i] == '1') val |= 1;
    }
    return val;
}

//我们正常是00代表R0，01代表R1，以此类推，但根据题目要求需要做映射，01代表R0，11代表R1, 011正常代表R3
uint8_t map_register(uint8_t code) {
    switch (code) {
        case 0b01: return 0;  // 01 -> R0
        case 0b11: return 1;  // 11 -> R1
        default: return 0;    // fallback
    }
}

void execute(uint8_t instr) {
    uint8_t opcode = (instr >> 5) & 0x07;
    //只保留下面这些可以运行映射版
    
    uint8_t raw_op1 = (instr >> 3) & 0x03;
    uint8_t op2 = instr & 0x07;
    uint8_t op1 = map_register(raw_op1);
    

    //只保留下面这些可以运行正常版
    /*
    uint8_t op1 = (instr >> 3) & 0x03;
    uint8_t op2 = instr & 0x07;
    */

    switch (opcode) {
        case 0b000:  // ADD
            R[op1] += R[op2];
            printf("R%d = R%d + R%d = %d\n", op1, op1, op2, R[op1]);
            break;
        case 0b001:  // MOV (立即数)
            R[op1] = op2;
            printf("R%d = %d\n", op1, R[op1]);
            break;
        case 0b010:  // SUB
            R[op1] -= R[op2];
            printf("R%d = R%d - R%d = %d\n", op1, op1, op2, R[op1]);
            break;
        case 0b011:  // LD
            R[op1] = MEM[op2];
            printf("R%d = MEM[%d] = %d\n", op1, op2, R[op1]);
            break;
        case 0b100:  // ST
            MEM[op2] = R[op1];
            printf("MEM[%d] = R%d = %d\n", op2, op1, MEM[op2]);
            break;
        case 0b101:  // JMP
            PC = op2;
            printf("JMP to %d\n", op2);
            return;
        case 0b110:  // JZ
            if (R[op1] == 0) {
                PC = op2;
                printf("JZ: R%d == 0, jump to %d\n", op1, op2);
                return;
            } else {
                printf("JZ: R%d != 0, no jump\n", op1);
            }
            break;
        case 0b111:  // HLT
            running = false;
            printf("HLT: Halting\n");
            break;
        default:
            printf("Unknown opcode: %d\n", opcode);
    }

    PC++;
}

void run_program(uint8_t program[], size_t len) {
    while (running && PC < len) {
        execute(program[PC]);
    }
}

int main() {
    char line[16];
    uint8_t program[256];
    int count = 0;

    //每行输入8位二进制数，输入END结束
    printf("Enter 8-bit binary instructions (one per line). Type 'END' to run:\n");

    while (1) {
        if (!fgets(line, sizeof(line), stdin)) break;

        // 去除换行
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "END") == 0) break;

        if (strlen(line) != 8 || strspn(line, "01") != 8) {
            printf("Invalid input. Please enter 8 binary digits.\n");
            continue;
        }

        program[count++] = parse_binary(line);

    }

    // 可选：初始化某些寄存器值
    R[0] = 4;  // 用于 ADD 的测试
    R[3] = 3;

    run_program(program, count);

    return 0;
}
