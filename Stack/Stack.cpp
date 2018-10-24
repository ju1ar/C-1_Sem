#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include "stack.h"
//-------------------------------------------------------------------------------------------------------------------------

#ifdef __DEBUG__

#define STACK_DUMP(stack) StackDump(stack, #stack)

#define HASHREFRESH(stack) StackHash(stack)
#define CHECKHASH(stack)                                        \
{                                                               \
    data_t hash = stack->hash;                                  \
    if (!IsEqual(StackHash(stack), hash))  return ST_ERHASH;    \
}

#define STERR_CHECK( stack, command )              \
{                                                  \
    if((sterrno = StackOk(stack)) != ST_NOERR)     \
    {                                              \
        STACK_DUMP(stack);                         \
        psterror(__FUNCTION__);                    \
        #command;                                  \
    }                                              \
}

#else
#define STACK_DUMP( stack )  if(0) StackDump(stack, #stack)

#define HASHREFRESH(stack)   if(0) StackHash(stack)


#define STERR_CHECK( stack, command )              \
{                                                  \
    if((sterrno = StackOk(stack)) != ST_NOERR)     \
    {                                              \
        #command;                                  \
    }                                              \
}

#endif
//-------------------------------------------------------------------------------------------------------------------------
sterror_t sterrno = ST_NOERR;

const int CANARY1 = rand();
const int CANARY2 = rand();
//-------------------------------------------------------------------------------------------------------------------------

sterror_t StackCtor(Stack* stack)
{
    if (!stack)
    {
        sterrno = ST_ERNULL;
        return sterrno;
    }

    stack->first_canary = CANARY1;
    stack->second_canary = CANARY2;


    stack->size = 0;
    stack->capacity = 0;

    HASHREFRESH(stack);

    return ST_NOERR;
}

sterror_t StackCtor(Stack* stack, size_t capacity_)
{
    if (!stack)
    {
        sterrno = ST_ERNULL;
        return sterrno;
    }

    stack->first_canary = CANARY1;
    stack->second_canary = CANARY2;

    stack->data = (data_t*)calloc(capacity_ , sizeof(*stack->data));
    STERR_CHECK(stack, return sterrno); 


    stack->size = 0;
    stack->capacity = capacity_;

    HASHREFRESH(stack);

    STERR_CHECK(stack, return sterrno); 

    return ST_NOERR;
}


sterror_t StackDtor(Stack* stack)
{
    STERR_CHECK(stack, return sterrno); 

    free(stack->data);
    stack->data = NULL; 

    stack->capacity = -1;
    stack->size = -1;

    return ST_NOERR;
}


sterror_t StackOk(Stack* stack)
{
    

    if (!stack)                            return ST_ERNULL;
    if (!stack->data)                      return ST_ERDATA_NULL;
    if (CheckZero(stack))                  return ST_ERELEM;
    if (stack->size > stack->capacity)     return ST_ERSIZE;
    if (stack->first_canary  != CANARY1)   return ST_CANARY1_DEATH;
    if (stack->second_canary != CANARY2)   return ST_CANARY2_DEATH;

    return ST_NOERR;
}


sterror_t StackPush(Stack* stack, data_t val)
{
    STERR_CHECK(stack, return sterrno); 

    if(stack->size == stack->capacity)
        StackRealloc(stack, ADD);

    stack->data[stack->size++] = val;
    HASHREFRESH(stack);

    STERR_CHECK(stack, return sterrno); 

    return ST_NOERR;
}


sterror_t StackRealloc(Stack* stack, int purpose)
{
    STERR_CHECK(stack, return sterrno); 

    switch(purpose)
    {
        case ADD:
            stack->capacity = 2*stack->capacity + 1;

            stack->data = (data_t*)realloc(stack->data, stack->capacity * sizeof(*stack->data));

            for(int i = stack->capacity / 2; i < stack->capacity; i++)
                stack->data[i] = 0;

            HASHREFRESH(stack);

            STERR_CHECK(stack, return sterrno); 

        break;

        case FREE:
            stack->capacity /= 2;

            stack->data = (data_t*)realloc(stack->data, stack->capacity * sizeof(*stack->data));
            HASHREFRESH(stack);

            STERR_CHECK(stack, return sterrno); 
        
        break;
    }

    STERR_CHECK(stack, return sterrno); 

    return ST_NOERR;
}


data_t StackPop(Stack* stack)
{
    STERR_CHECK(stack, );
    if (sterrno != ST_NOERR) return NAN;

    if(stack->size == 0) 
    {
        sterrno = ST_UNDERFLOW;
        return NAN;
    }

    if(stack->size == stack->capacity/4)
        StackRealloc(stack, FREE);
    
    data_t val = stack->data[stack->size - 1];

    stack->data[stack->size - 1] = 0;

    stack->size--;

    HASHREFRESH(stack);

    STERR_CHECK(stack, );
    if (sterrno != ST_NOERR) return NAN;

    return val;
}

data_t StackHash(Stack* stack)
{
    stack->hash = stack->size + 2*stack->capacity;
    
    for (int i = 0; i < stack->capacity; i++)
          stack->hash += (i+1)*stack->data[i]; 

    return stack->hash;
}

bool CheckZero(const Stack* stack)
{
    for (int i = stack->size; i < stack->capacity; i++)
        if (!IsEqual(stack->data[i], 0)) return true;   

    return false;
}


bool IsEqual(double val1, double val2)
{
    return (fabs(val1 - val2) < DBL_EPSILON);
}


void psterror(const char* str)
{
    switch(sterrno)
    {
        case ST_NOERR:
            break;

         case ST_ERNULL:                                                   
            fprintf(stderr, "%s : Pointer to stack is NULL\n", str);              
            break;            

         case ST_ERDATA_NULL:                                                   
            fprintf(stderr, "%s : Pointer to stack->data is NULL\n", str);              
            break;            

        case ST_ERSIZE:
            fprintf(stderr, "%s : stack->size > stack->capacity\n", str);              
            break;            

        case ST_ERELEM:
            fprintf(stderr, "%s : Element != 0  in stack->data[], where count > size\n", str);              
            break;            

        case ST_ERHASH:
            fprintf(stderr, "%s : Stack->hash is wrong!\n", str);              
            break;            

        case ST_UNDERFLOW:
            fprintf(stderr, "%s : Stack underflow\n", str);              
            break;            

        case ST_CANARY1_DEATH:
            fprintf(stderr, "%s : First canary is DEAD\n", str);

        case ST_CANARY2_DEATH:
            fprintf(stderr, "%s : Second canary is DEAD\n", str);

        default:
            fprintf(stderr, "%s : Unknown error = %d in switch\n", str, sterrno);
    }
}



#define DUMP_IN                                                                             \
{                                                                                           \
    fprintf(dump_in, "Stack ""%s"" {%p} \n{\n" , stackname, stack);                         \
    fprintf(dump_in, "\tcapacity = %lu \n\tsize = %lu \n", stack->capacity, stack->size);   \
    fprintf(dump_in, "\tdata[%lu] {%p} \n\t{\n", stack->capacity , stack->data);            \
                                                                                            \
    for(int i = 0; i < stack->size; i++)                                                    \
        fprintf(dump_in, "\t\t*[%d] = %lg\n", i, stack->data[i]);                           \
                                                                                            \
    for(int i = stack->size; i < stack->capacity; i++)                                      \
        fprintf(dump_in, "\t\t [%d] = %lg\n", i, stack->data[i]);                           \
                                                                                            \
        fprintf(dump_in, "\t}\n}\n");                                                       \
}

sterror_t StackDump(const Stack* stack, const char* stackname)
{
    FILE* dump_in = fopen("StackDump.txt", "w"); 
    if (!dump_in) perror(__FUNCTION__);

    switch(sterrno)
    {
        case ST_NOERR:
            DUMP_IN;

            break;

        case ST_ERNULL:
            fprintf(dump_in, "Stack ""%s"" {NULL}\n", stackname);

            break;

        case ST_ERDATA_NULL:
            fprintf(dump_in, "Stack ""%s"" {%p}\n{\n", stackname, stack);
            fprintf(dump_in, "\tcapacity = %lu \n\tsize = %lu \n", stack->capacity, stack->size);
            fprintf(dump_in, "\tdata[] {NULL} \n\t{\n");

            break;

        case ST_ERSIZE:
            fprintf(dump_in, "Size is wrong!\n");
            DUMP_IN;

        case ST_ERELEM:
            fprintf(dump_in, "Element != 0  in stack->data[], where count > size\n");
            DUMP_IN;

            break;
            
        case ST_ERHASH:
            fprintf(dump_in,"Stack->hash is wrong!\n");
            DUMP_IN;

            break;

        case ST_UNDERFLOW:
            fprintf(dump_in, "Stack underflow\n");              
            DUMP_IN;

            break;

        case ST_CANARY1_DEATH:
            fprintf(dump_in, "First canary is DEAD!\n");
            DUMP_IN;

            break;

        case ST_CANARY2_DEATH:
            fprintf(dump_in, "Second canary is DEAD!\n");
            DUMP_IN;

            break;

        default:
            fprintf(dump_in, "Unfnown error: switch, func %s, line %d\n", __FUNCTION__, __LINE__);
            DUMP_IN;
    }
            
    fclose(dump_in);
    return ST_NOERR;
}

#undef DUMP_IN
