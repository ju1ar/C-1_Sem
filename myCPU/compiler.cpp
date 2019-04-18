#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "cmds_regs.h"
#include "../Onegin/textsplit.h"
#include "../Log/log.h"  // Specify  -D __DEBUG__  if you want to write to log file

//-------------------------------------------------------------------------------------------------------------

const int MAX_NUM_INSTR_STR = 3;
const int MAX_LEN_REG       = 2;
const int MAX_LEN_LABEL     = 20;
const int MAX_LABELS_NUM    = 30;

//-------------------------------------------------------------------------------------------------------------

struct label_t
{
    char   label_name[MAX_LEN_LABEL];
    size_t label_str;
    size_t call_str_next;
};

typedef struct label_t label_t;

label_t labels[MAX_LABELS_NUM] = {};         // global array for labels                         

//-------------------------------------------------------------------------------------------------------------

char** ReadAndAllocateCodeStrs(int* code_strs_num, const char* code_name);
int* Compile(char** code_strs, int code_strs_num, int* instr_size);
int* CompileAllocate(char** code_strs, int code_strs_num, int* instr_size);
log_error_t WriteInstrToExeFile(int* instruction, int instr_size, const char* exe_name);
int* DetectJumpStr(const char* code_str, int* instruction, int ip);
int* DetectPushOrPopArgs(const char* code_str, int* instruction, int ip, const int jump_cmd);
bool IsEmptySymb(char symb);
log_error_t PrintInfo();

//-------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    LogCtor();

    int code_strs_num = 0;
    const char* code_name = argv[1];
    char** code_strs = ReadAndAllocateCodeStrs(&code_strs_num, code_name);
    CHECK_LOG(_PTR, code_strs, return 1); 

    int instr_size = 0;
    int* instruction = CompileAllocate(code_strs, code_strs_num, &instr_size);
    CHECK_LOG(_PTR, instruction, return 1); 

    const char* exe_name = argv[2];
    WriteInstrToExeFile(instruction, instr_size, exe_name);
    LOG_OK(return 1);

    if(argv[argc - 1][1] == 'i')  PrintInfo();

    free(code_strs[0]);
    free(code_strs);
    free(instruction);

    LogDtor();

    return 0;
}

//-------------------------------------------------------------------------------------------------------------

log_error_t PrintInfo()
{
    printf("## Compiler Version: 1.3\n"
           "## Max number of labels - %d\n"
           "## Max length of label's name - %d\n",
            MAX_LEN_LABEL, MAX_LABELS_NUM);

    return LOG_NOERR;
}

int* CompileAllocate(char** code_strs, int code_strs_num, int* instr_size)
{
    int* instruction = Compile(code_strs, code_strs_num, instr_size); 
    CHECK_LOG(_PTR, instruction, return NULL); 

    instruction = Compile(code_strs, code_strs_num, instr_size);
    CHECK_LOG(_PTR, instruction, return NULL); 

    return instruction;
}

log_error_t WriteInstrToExeFile(int* instruction, int instr_size, const char* exe_name)
{
    FILE* exe_ptr = fopen(exe_name, "wb");
    CHECK_LOG(_FILE, exe_ptr, return log_err); 

    fwrite(instruction, sizeof(*instruction), instr_size, exe_ptr);

    return LOG_NOERR;
}

char** ReadAndAllocateCodeStrs(int* code_strs_num, const char* code_name)
{
    CHECK_LOG(_PTR, code_strs_num, return NULL);

    FILE* code_ptr = fopen(code_name, "r");
    CHECK_LOG(_FILE, code_ptr, return NULL); 

    long int code_size = 0;
    char* code = ReadToAllocatedBuffer(code_ptr, &code_size);
    CHECK_LOG(_PTR, code, return NULL); 

    char** code_strs = AllocateArrayOfStrs(code, code_strs_num);
    CHECK_LOG(_PTR, code_strs, return NULL); 

    fclose(code_ptr);

    return code_strs;
}


int* DetectJumpStr(const char* code_str, int* instruction, int ip, const int jump_cmd)
{
    CHECK_LOG(_PTR, code_str, return NULL); 
    CHECK_LOG(_PTR, instruction, return NULL); 

    char label_name[MAX_LEN_LABEL] = {};
    bool find_label = 0;

    sscanf(code_str, "%*s%s", label_name);

    for (int i = 0; i < MAX_LABELS_NUM; i++)
    {
        if (strcmp(label_name, labels[i].label_name) == 0)       
        {
            instruction[ip + 1] = labels[i].label_str;
            find_label = 1;
            labels[i].call_str_next = (jump_cmd == CMD_CALL)? ip + 1 : -1;

            break;
        }
    }

    if (!find_label) instruction[ip + 1] = -1;

    return instruction;
}

#define DEF_REG(reg_name, reg_num)      \
{                                       \
if (strcasecmp(reg, #reg_name) == 0)    \
    instruction[ip + 2] = (reg_num);    \
}

int* DetectPushOrPopArgs(const char* code_str, int* instruction, int ip)
{
    CHECK_LOG(_PTR, code_str, return NULL); 
    CHECK_LOG(_PTR, instruction, return NULL); 

    int val = 0;
    int RAM_num = 0;
    char reg[MAX_LEN_REG + 1] = {};
push       [50]
    if (sscanf(code_str, "%*s%d", &val))   // push val
    {
        instruction[ip + 1] = VAL_TYPE;                    
        instruction[ip + 2] = val;                    
    }
    else if ((sscanf(code_str, "%*[^[][%d]", &RAM_num)) > 0)   //push/pop ram_num
    {
        instruction[ip + 1] = RAM_TYPE;
        instruction[ip + 2] = RAM_num;
    }
    else if (sscanf(code_str, "%*s%s", reg))    // push/pop reg
    {
        instruction[ip + 1] = REG_TYPE;                         
        #include "def_regs.h"                           
    }
    return instruction;
}

#undef DEF_REG

#define DEF_CMD(cmd_name, cmd_num, num_of_args, is_jump, code)                             \
else if ((cmd_cmp = strcasecmp(code_strs[i], #cmd_name)) == '\0' || cmd_cmp == ' ')        \
{                                                                                          \
    instruction[ip] = (cmd_num);                                                           \
                                                                                           \
    if ((num_of_args) > 0)                                                                 \
        {                                                                                  \
            if (is_jump)                                                                   \
                {                                                                          \
                if  (cmd_num == CMD_RET)                                                   \
                      instruction[ip + 1] = Call_str + 1;                                  \
                else  instruction = DetectJumpStr(code_strs[i], instruction, ip, cmd_num); \
                }                                                                          \
                                                                                           \
            else if (cmd_num == CMD_PUSH || cmd_num == CMD_POP)                            \
                instruction = DetectPushOrPopArgs(code_strs[i], instruction, ip);          \
        }                                                                                  \
                                                                                           \
    ip += (num_of_args + 1);                                                               \
}

int Call_str = 0;

int* Compile(char** code_strs, int code_strs_num, int* instr_size)
{
    CHECK_LOG(_PTR, code_strs,  return NULL);
    CHECK_LOG(_PTR, instr_size, return NULL);

    *instr_size = code_strs_num * MAX_NUM_INSTR_STR;
    int  ip = 0;

    int* instruction = (int*)calloc(*instr_size, sizeof(*instruction));
    CHECK_LOG(_PTR, instruction, return NULL);

    int label_count = 0;
    int cmd_cmp = 0;

    for (int i = 0; i < code_strs_num; i++)
    {
        if (IsEmptySymb(code_strs[i][0]));

        #include "def_cmds.h"

        else if (sscanf(code_strs[i], "%[^:]", labels[label_count].label_name) != 0)
        {
            labels[label_count].label_str = ip;
            if (labels[label_count].call_str_next != -1) Call_str = labels[label_count].call_str_next;
            label_count++;
        }
    }

    *instr_size = ip;
    instruction = (int*)realloc(instruction, *instr_size * sizeof(*instruction));      

    return instruction;
}

#undef DEF_CMD


bool IsEmptySymb(char symb)
{
    if (symb == '\0' || symb == ' ' || symb == ';')
        return true;

    return false;
}
