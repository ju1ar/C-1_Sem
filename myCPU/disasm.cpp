#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "cmds_regs.h"
#include "../Log/log.h" // Specify  -D __DEBUG__  if you want to write to log file

//-------------------------------------------------------------------------------------------------------------
const int MAX_NUM_OF_JUMPS = 30;
 
int Jumps[MAX_NUM_OF_JUMPS] = {-1};                  //global array of jumps;

int Num_of_jumps = 0;
//-------------------------------------------------------------------------------------------------------------

int* ReadInstrToAllocatedBuf(const char* exe_name);
log_error_t DisAsm(int* instruction, const char* file_name);
int* DetectCmdWithArgs(int* instruction, const char* cmd_name, int is_jump, int ip, FILE* file_ptr);
bool IpInJumps(int ip);

//-------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    LogCtor();

    const char* exe_name = argv[1];
    int* instruction = ReadInstrToAllocatedBuf(exe_name);
    CHECK_LOG(_PTR, instruction, return 1);

    const char* dis_name = argv[2];
    DisAsm(instruction, dis_name);
    DisAsm(instruction, dis_name);
    LOG_OK(return 1);

    LogDtor();
}

//-------------------------------------------------------------------------------------------------------------

int* ReadInstrToAllocatedBuf(const char* exe_name)
{
    FILE* exe_ptr = fopen(exe_name, "rb");
    CHECK_LOG(_FILE, exe_ptr, return NULL);

    fseek(exe_ptr, 0, SEEK_END);
    int instr_size = ftell(exe_ptr) / sizeof(int);
    rewind(exe_ptr);

    int* instruction = (int*)calloc(instr_size, sizeof(*instruction));
    CHECK_LOG(_PTR, instruction, return NULL);

    if(fread(instruction, sizeof(*instruction), instr_size, exe_ptr) != instr_size)
        PrintLog("Wrong read");

    return instruction;
}

#define DEF_REG(reg_name, reg_num)             \
{                                              \
    if (instruction[ip + 2] == reg_num)        \
        fprintf(file_ptr, "%s\n", #reg_name);  \
}

int* DetectCmdWithArgs(int* instruction, const char* cmd_name, int is_jump, int ip, FILE* file_ptr)
{                                                                   
    if (is_jump) 
    {
        fprintf(file_ptr, "%s %d\n", cmd_name, instruction[ip + 1]); 
        if(strcmp(cmd_name, "RET") != 0)
        {
            Jumps[Num_of_jumps] = instruction[ip + 1];
            Num_of_jumps++;
        }
    }
    else                                                            
    {                                                               
        switch(instruction[ip + 1])                                 
        {                                                           
            case VAL_TYPE:                                          
                fprintf(file_ptr, "%s %d\n", cmd_name, instruction[ip + 2]);  
                break;                                              
                                                                    
            case REG_TYPE:                                          
                fprintf(file_ptr, "%s ", cmd_name);                           
                #include "def_regs.h"                               
                break;                                              
        }                                                           
    }                                                               
                                                                    
    return instruction;
}                                                                   


#undef DEF_REG

#define DEF_CMD(cmd_name, cmd_num, num_of_args, is_jump, code)                     \
{                                                                                  \
    case CMD_##cmd_name:                                                           \
        if (num_of_args > 0)                                                       \
           instruction = DetectCmdWithArgs(instruction, #cmd_name, is_jump, ip, file_ptr);   \
        else                                                                       \
           fprintf(file_ptr, "%s\n", #cmd_name);                                   \
                                                                                   \
        ip += (num_of_args + 1);                                                   \
                                                                                   \
        break;                                                                     \
}

log_error_t DisAsm(int* instruction, const char* file_name)
{
    FILE* file_ptr = fopen(file_name, "w");

    CHECK_LOG(_PTR, instruction, return log_err);
    int ip = 0;

    for (ip = 0; instruction[ip] != CMD_END;)
    {
        if(IpInJumps(ip)) fprintf(file_ptr, "%d:\n", ip);

        switch(instruction[ip])    
        {
            #include "def_cmds.h"
        }

    }
    fprintf(file_ptr, "%d:\n", ip);
    fprintf(file_ptr, "END\n");

    Num_of_jumps = 0;
        
    fclose(file_ptr);
    return LOG_NOERR;
}

#undef DEF_CMD

bool IpInJumps(int ip)
{
    for(int i = 0; i < Num_of_jumps; i++)
        if (Jumps[i] == ip) return true;

    return false;
}
