#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <math.h>
#include <unistd.h>
#include "cmds_regs.h"
#include "../Stack/stack.h"
#include "../Log/log.h"

//-------------------------------------------------------------------------------------------------------------
const int NUM_OF_REGS               = 4;
const int MAX_NUM_OF_INSTRS_IN_CMD  = 3;
const int RAM_SIZE                  = 50;
//-------------------------------------------------------------------------------------------------------------

struct CPU_t
{
    Stack stk;
    int regs[NUM_OF_REGS];
    int RAM[RAM_SIZE];
};

//-------------------------------------------------------------------------------------------------------------

int* ReadInstrToAllocatedBuf(int* instr_size, const char* exe_name);
log_error_t CPU_Ctor(CPU_t* cpu, int stk_size);
log_error_t Execute(CPU_t* cpu, int* instruction);
log_error_t CPU_Dtor(CPU_t* cpu);

//-------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    LogCtor();

    const char* exe_name = argv[1];
    int instr_size = 0;
    int* instruction = ReadInstrToAllocatedBuf(&instr_size, exe_name);

    CPU_t cpu = {};

    int max_stk_size = instr_size / MAX_NUM_OF_INSTRS_IN_CMD;

    CPU_Ctor(&cpu, max_stk_size);

    Execute(&cpu, instruction);

    CPU_Dtor(&cpu);

    LogDtor();

    return 0;
}


int* ReadInstrToAllocatedBuf(int* instr_size, const char* exe_name)
{
    CHECK_LOG(_PTR,  instr_size, return NULL);

    FILE* exe_ptr = fopen(exe_name, "rb");
    CHECK_LOG(_FILE, exe_ptr,    return NULL);

    fseek(exe_ptr, 0, SEEK_END);
    *instr_size = ftell(exe_ptr) / sizeof(int);
    rewind(exe_ptr);

    int* instruction = (int*)calloc(*instr_size, sizeof(*instruction));
    CHECK_LOG(_PTR, instruction, return NULL);

    if(fread(instruction, sizeof(*instruction), *instr_size, exe_ptr) != *instr_size)
        PrintLog("Wrong read");

    return instruction;
}


log_error_t CPU_Ctor(CPU_t* cpu, int stk_size)
{
    CHECK_LOG(_PTR, cpu, return log_err);
    
    StackCtor(&cpu->stk, stk_size);
    
    return LOG_NOERR;
}

log_error_t CPU_Dtor(CPU_t* cpu)
{
    CHECK_LOG(_PTR, cpu, return log_err);
    
    StackDtor(&cpu->stk);

    return LOG_NOERR;
}



#define DEF_CMD(cmd_name, cmd_num, num_of_args, is_jump, code)  \
{                                                               \
    case CMD_##cmd_name:                                        \
         args_num = num_of_args;                                \
         code;                                                  \
         break;                                                 \
}                                                           

log_error_t Execute(CPU_t* cpu, int* instruction)
{
    CHECK_LOG(_PTR, instruction, return log_err);
    
    int args_num = 0;     

    for (int ip = 0; instruction[ip] != CMD_END;)
    {
        switch(instruction[ip])
        {
            #include "def_cmds.h"
        }
    }

    printf("\n");
    
    return LOG_NOERR;
} 

#undef DEG_CMD
