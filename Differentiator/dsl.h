#pragma once

#define IMPORT_DSL   

#define L                    L_child 
#define R                    R_child 
#define PAR                  parent
#define TYPE                 data->type 
#define CONTENT              data->content 

#define CONST( num )         NodeCtor(CONST, num,   NULL, NULL, NULL)
#define PLUS(  U , V )       NodeCtor(OP,    PLUS , U,    V,    NULL)
#define MINUS( U , V )       NodeCtor(OP,    MINUS, U,    V,    NULL)
#define MUL(   U , V )       NodeCtor(OP,    MUL  , U,    V,    NULL)
#define DIV(   U , V )       NodeCtor(OP,    DIV  , U,    V,    NULL)


#define D( X )               DiffFunc(X)
#define C( X )               NodeCopy(X, NULL)

#define KILL_TREE( X )       {int count = 0;             \
                              TreeDtor(X, &count);}

#define KILL_NODE( X )       NodeDtor(X)

#define IS_MUL(   X )        ( X->TYPE == OP && X->CONTENT == MUL   )
#define IS_DIV(   X )        ( X->TYPE == OP && X->CONTENT == DIV   ) 
#define IS_ADD(   X )        ( X->TYPE == OP && X->CONTENT == PLUS  ) 
#define IS_MINUS( X )        ( X->TYPE == OP && X->CONTENT == MINUS ) 

#define IS_0( X )            ( X->TYPE == CONST && X->CONTENT == 0 )
#define IS_1( X )            ( X->TYPE == CONST && X->CONTENT == 1 )

#define SWAP_CHILDREN        {Node* help_child = NODE->L;           \
                              NODE->L = NODE->R;                    \
                              NODE->R = help_child;}                           
