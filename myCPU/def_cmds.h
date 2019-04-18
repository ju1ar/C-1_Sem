#define DEF_CMDS_H
#ifdef  DEF_CMDS_H


#define VAL_ARG instruction[ip + 1] == VAL_TYPE
#define REG_ARG instruction[ip + 1] == REG_TYPE
#define RAM_ARG instruction[ip + 1] == RAM_TYPE

#define _arg_ instruction[ip + 2]
#define _reg_ cpu->regs[instruction[ip + 2]]
#define _ram_ cpu->RAM[instruction[ip + 2]]

#define SPUSH(_arg_) StackPush(&cpu->stk, _arg_)
#define SPOP         StackPop (&cpu->stk)

#define JUMP    ip = instruction[ip + 1]

#define INCR_IP ip += (args_num + 1)

#define INPUT             \
    int val = 0;          \
    scanf("%d", &val);    

#define OUTPUT            \
{                         \
    printf("%d ", SPOP);  \
}           

DEF_CMD(PUSH,  1,  2, 0, {if (VAL_ARG) SPUSH(_arg_); else if (REG_ARG) SPUSH(_reg_); 
                          else if (RAM_ARG) {SPUSH(_ram_); sleep(1);}; INCR_IP;})
DEF_CMD(POP,   2,  2, 0, {if (REG_ARG) _reg_ = SPOP; else if (RAM_ARG) {_ram_ = SPOP; sleep(1);}; INCR_IP;})
DEF_CMD(ADD,   3,  0, 0, {SPUSH(    (SPOP + SPOP)); INCR_IP;})
DEF_CMD(SUB,   4,  0, 0, {SPUSH(   -(SPOP - SPOP)); INCR_IP;})
DEF_CMD(MUL,   5,  0, 0, {SPUSH(    (SPOP * SPOP)); INCR_IP;})
DEF_CMD(DIV,   6,  0, 0, {int a = SPOP; int b = SPOP; SPUSH((b / a)); INCR_IP;})
DEF_CMD(FSQRT, 7,  0, 0, {SPUSH(sqrt(SPOP)); INCR_IP;})
DEF_CMD(JMP,   8,  1, 1, {JUMP;})
DEF_CMD(JA,    9,  1, 1, {if (SPOP <  SPOP) JUMP; else INCR_IP;})
DEF_CMD(JB,    10, 1, 1, {if (SPOP >  SPOP) JUMP; else INCR_IP;})
DEF_CMD(JNA,   11, 1, 1, {if (SPOP <= SPOP) JUMP; else INCR_IP;})
DEF_CMD(JNB,   12, 1, 1, {if (SPOP >= SPOP) JUMP; else INCR_IP;})
DEF_CMD(JE,    13, 1, 1, {if (SPOP == SPOP) JUMP; else INCR_IP;})
DEF_CMD(JNE,   14, 1, 1, {if (SPOP != SPOP) JUMP; else INCR_IP;})
DEF_CMD(CALL,  15, 1, 1, {JUMP;})
DEF_CMD(RET,   16, 1, 1, {JUMP;})
DEF_CMD(IN,    17, 0, 0, {INPUT; SPUSH(val); INCR_IP;})
DEF_CMD(OUT,   18, 0, 0, {OUTPUT; INCR_IP;})
DEF_CMD(END,   19, 0, 0, {INCR_IP;})
DEF_CMD(MEOW,  20, 0, 0, {printf("                           \n"
"             .-'  `-.                                       \n" 
"          .-'        \\                                     \n"
"         ;    .-'\\   ;                                     \n"
"         `._.'    ;   |                                     \n"
"                  |   |                                     \n"
"                  ;   :                                     \n"
"                 ;   :                                      \n"
"                 ;   :                                      \n"
"                /   /                                       \n"
"               ;   :                   ,                    \n"
"               ;   |               .-\"7|                   \n"
"             .-'\" :            .-' .' :                    \n"
"          .-'       \\        .'  .'   `.                   \n"
"        .'           `-. \"\"-.-'`\"\"    `\",`-._..--\"7   \n"
"        ;    .          `-.J `-,    ;\"`.;|,_,    ;         \n"
"      _.'    |         `\"\" `. .\"\"\"--. o \\:.-. _.'     \n"
"   .\"\"       :            ,--`;   ,  `--/}o,' ;           \n"
"   ;   .___.'        /     ,--.`-. `-..7_.-  /_             \n"
"    \\   :   `..__.._;    .'__;    `---..__.-'-.`\"-,       \n"
"    .'   `--. |   \\_;    \'   `-._.-\")     \\\\  `-,      \n"
"    `.   -.`_):      `.   `-\"\"\"`.   ;__.' ;/ ;           \n"
"      `-.__7\"  `-..._.'`7     -._;'  ``\"-''               \n"
"                        `--.,__.'                           \n"); INCR_IP;})

#endif
