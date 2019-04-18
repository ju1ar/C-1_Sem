#define COMMANDS_H
#ifdef COMMANDS_H


#define DEF_CMD(cmd_name, num, num_of_args, is_jump, code) CMD_##cmd_name = (num),

enum 
{
    #include "def_cmds.h"
};

#undef DEF_CMD

#define DEF_REG(reg_name, num) REG_##reg_name = (num),

enum 
{
    #include "def_regs.h"
};

#undef DEF_REG

enum Type
{
    VAL_TYPE = 1,
    REG_TYPE = 2, 
    RAM_TYPE = 3
};

#endif
