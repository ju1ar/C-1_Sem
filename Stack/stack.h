#ifndef STACK_H
#define STACK_H

typedef int sterror_t;

extern sterror_t sterrno;


//-------------------------------------------------------------------------------------------------------------------------
enum Purpose
{
    ADD, 
    FREE
};

enum Stack_errors
{
    ST_NOERR,
    ST_ERNULL,
    ST_ERDATA_NULL,
    ST_ERSIZE,
    ST_ERELEM,
    ST_ERHASH,
    ST_UNDERFLOW,
    ST_CANARY1_DEATH,
    ST_CANARY2_DEATH
};
//-------------------------------------------------------------------------------------------------------------------------
typedef double data_t;


typedef struct Stack
{
    int first_canary;
    data_t* data;
    size_t size;
    size_t capacity;
    data_t hash;
    int second_canary;
} Stack;
//-------------------------------------------------------------------------------------------------------------------------
sterror_t StackCtor(Stack* stack);
sterror_t StackCtor(Stack* stack, size_t capacity_);
data_t StackPop(Stack* stack);
sterror_t StackPush(Stack* stack, data_t val);
sterror_t StackOk(Stack* stack);
sterror_t StackDtor(Stack* stack);
sterror_t StackRealloc(Stack* stack, int purpose);
sterror_t StackDump(const Stack* stack, const char* stackname);
data_t StackHash(Stack* stack);
void psterror(const char* str);
bool CheckZero(const Stack* stack);
bool IsEqual(data_t val1, data_t val2);
//-------------------------------------------------------------------------------------------------------------------------

#endif
