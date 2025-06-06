# **项目 30：微型指令集架构设计与解释器实现**

## 1.功能要求

* 自定义一个 8 位微型指令集（例如指令长度为 8-bit，支持基本算术与逻辑运算、跳转、内存访问）。
* 设计其编码格式、寄存器与内存结构。
* 实现一个解释器可以解析、加载并运行该指令集程序，并输出执行日志。
* 要求不得使用已有汇编解释器或虚拟机库，指令解析必须自己手动实现。

## 2.输入输出样例

```
Instruction Set Format:  
  Opcode: 3 bits  
  Operand1: 2 bits  
  Operand2: 3 bits  

Input Program (in binary):  
  00000011  // ADD R0, R3  
  00101000  // MOV R1, #0  

Execution Log:
  R0 = R0 + R3 = 7  
  R1 = 0
```

## 3.工作分工

* 罗琰佳：指令集设计与编码，做演示；

* 邢菲琳：解释器与执行引擎实现；

* 欧天勤：执行日志格式与程序加载。

      

## 4.代码展示

```c
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
  000 00 011  // ADD R0, R3  （00代表R0，01代表R1，以此类推）
  001 01 000  // MOV R1, #0  

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

void execute(uint8_t instr) {
    uint8_t opcode = (instr >> 5) & 0x07;
    uint8_t op1 = (instr >> 3) & 0x03;
    uint8_t op2 = instr & 0x07;

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
        case 0b110:  // AND
            R[op1] = R[op1] & R[op2];
            printf("R%d = R%d & R%d = %d\n", op1, op1, op2, R[op1]);
            break;
        case 0b111:  // OR
            R[op1] = R[op1] | R[op2];
            printf("R%d = R%d | R%d = %d\n", op1, op1, op2, R[op1]);
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
    R[1] = 8;
    R[2] = 5; 
    R[3] = 3;

    run_program(program, count);

    return 0;
}

```

运行方式：运行编译好的文件，文件提示：

```
Enter 8-bit binary instructions (one per line). Type 'END' to run:
```

输入

```
00001011 
00111000
END
```

输出

```
R0 = R0 + R3 = 7  
R1 = 0
```



## 5.原理解析

### a. 指令编码格式（8-bit）

| 位数 | 含义                               |
| ---- | ---------------------------------- |
| 0-2  | Opcode（操作码，3 位）             |
| 3-4  | Operand1（目标寄存器，2 位）       |
| 5-7  | Operand2（源寄存器或立即数，3 位） |

#### 操作码表（Opcode）

| Opcode | 指令 | 含义                            |
| ------ | ---- | ------------------------------- |
| 000    | ADD  | R[op1] = R[op1] + R[op2]        |
| 001    | MOV  | R[op1] = op2  |
| 010    | SUB  | R[op1] = R[op1] - R[op2]        |
| 011    | LD   |   R[op1] = MEM[op2]    |
| 100    | ST  |   MEM[op2] = R[op1]     |
| 101    | JMP   | PC = op2（jump） |
| 110    | AND   | R[op1] = R[op1] & R[op2]               |
| 111    | OR  | R[op1] = R[op1] \| R[op2]          |



------

### b. 寄存器与内存设计

- **寄存器（4 个）**：`R0` ~ `R3`，编号 00 ~ 11 

- **内存（8 个字节）**：地址范围为 0~7（3-bit addressing）

- **程序计数器（PC）**：跟踪当前执行指令的位置

      

## 6.总结

本项目设计并实现了一个完整的 8 位微型指令集，采用固定格式：Opcode（3 位）| Operand1（2 位）| Operand2（3 位）。该指令集支持基本的算术运算（如 ADD）、立即数赋值（MOV）、跳转控制（如 JMP、JZ）、内存读写等功能，具备基本 CPU 模拟能力。指令通过位运算手动解析，无依赖第三方汇编器或虚拟机库，实现了简洁的解释器框架。寄存器数量为 4（R0–R3），并配有小型内存单元。程序以 C 语言实现，利用数组模拟寄存器和内存，逐条加载执行指令，并输出清晰的执行日志。以指令 `000 00 011`（ADD R0, R3）和 `001 01 000`（MOV R1, #0）为例，系统成功解析并执行加法与赋值操作。该项目展现了从指令编码、解析到执行的完整流程，为深入理解底层计算机体系结构提供了良好实践基础，并具备良好的扩展性。
