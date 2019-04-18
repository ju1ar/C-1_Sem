#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rec_descent_tree.h"
#include "../Tree/tree.h"
#include "../Log/log.h" // Specify  -D __DEBUG__  if you want to write to log file
#include "dsl.h"

#define KEK( msg ) printf("%s\n", msg);

//------------------------------------------------------------------------------------------------------------

Node* DiffOp(const Node* func_node);
Node* DiffFunc(const Node* func_node);
log_error_t SimplifyFuncTree(Node** func_node);
bool MulBy1_AddWith0(Node** func_node);
bool MulBy0(Node** func_node);

//------------------------------------------------------------------------------------------------------------

int main()
{
    LogCtor();

    FILE* func_file_ptr = fopen("func.txt", "r");
    CHECK_LOG(_FILE, func_file_ptr, return 1);

    Node* func_tree = RecDescentReadTree(func_file_ptr, "easy function");
    CHECK_LOG(_PTR, func_tree, return 1);

//  SimplifyFuncTree(&func_tree);
//  Trees[0].root = func_tree;
//  NODES_NUM_REFRES(func_tree);

    Node* derivative_tree = DiffFunc(func_tree);
    AddNewTreeInfo(derivative_tree, "easy easy func");
    LOG_OK(return 1);

//  SimplifyFuncTree(&derivative_tree);
//  NODES_NUM_REFRES(derivative_tree);

//  DumpTreeDot(func_tree, "func.dot");
    DumpTreeDot(derivative_tree, "der_func.dot");

//  system("dot -Tpng check.dot -o check.png");
//  system("open check.png");

//  TreeDtorWrap(&func_tree);
    TreeDtorWrap(&derivative_tree);
    TreesInfoDtor();

    fclose(func_file_ptr);
    LogDtor();

    return 0;
}

IMPORT_DSL;
#define NODE  func_node

Node* DiffFunc(const Node* func_node)
{

    if(!NODE) return NULL;

    switch(NODE->TYPE)
    {
        case CONST:  return  CONST(0); 
 
        case VAR:    return  CONST(1);

        case OP:     return  DiffOp(func_node); 

        default:     PrintLog("There's no such type = %d, line = %d", NODE->TYPE, __LINE__);
                     return NULL;
    }
}

#undef NODE

IMPORT_DSL;
#define NODE  func_node

Node* DiffOp(const Node* func_node)
{

    switch(int(NODE->CONTENT))
    {
       case PLUS:   return  PLUS(  D( NODE->L ), D( NODE->R ) );    

       case MINUS:  return  MINUS( D( NODE->L ), D( NODE->R ) );    

       case MUL:    return  PLUS( MUL( D(NODE->L), C(NODE->R) ), MUL( D(NODE->R), C(NODE->L) ) );

       case DIV:    return  DIV( MINUS( MUL( D( NODE->L ), C( NODE->R ) ), MUL( D( NODE->R ), C( NODE->L ) ) ),
                                                                           MUL( C( NODE->R ), C( NODE->R ) ) );

       default:     PrintLog("There's no such operator = %d, line = %d", NODE->CONTENT, __LINE__);
                    return NULL;
    }
}

#undef NODE

//check parents


IMPORT_DSL;
#define NODE (*func_node)

log_error_t SimplifyFuncTree(Node** func_node)
{
    if ( !NODE->L || !NODE->R )  return LOG_NOERR;          //sin cos

    bool is_change = false;

    if ( !( is_change = MulBy1_AddWith0(&NODE)) )
    {
        SWAP_CHILDREN;
        is_change = MulBy1_AddWith0(&NODE);
    }

    if ( !is_change ) MulBy0(&NODE);

    SimplifyFuncTree(&NODE->R);

    return LOG_NOERR; 
}

bool MulBy0(Node** func_node)
{
    bool is_change = false;

    if ( IS_MUL( NODE )  && (IS_0( NODE->R ) || IS_0( NODE->L )) )
    {
        NODE->TYPE    = CONST;
        NODE->CONTENT = 0;

        KILL_TREE( &NODE->L );
        KILL_TREE( &NODE->R );

        is_change = true;
    }

    return is_change;
}

bool MulBy1_AddWith0(Node** func_node)
{
    bool is_change = false;
//errors
    if ( ( IS_MUL( NODE ) && IS_1( NODE->R ) ) ||
         ( IS_ADD( NODE ) && IS_0( NODE->R ) )   )                                     
    {                                                                                           
        
        if( NODE->PAR )                                                                         
        {                                                                                       
             KEK("par!");
             NODE->L->PAR = NODE->PAR;                                                           
            (NODE->PAR->L == NODE) ? NODE->PAR->L = NODE->L : NODE->PAR->R = NODE->L;            
        }                                                                                       
        else NODE->L->PAR = NULL;
                                                                                            
        KILL_NODE( &NODE->R );                                                                  

        Node* gonna_die = NODE;                                                                 

        NODE = NODE->L;                                                                         
        KILL_NODE( &gonna_die );                                                                

        is_change = true;
    }

    return is_change;
}

#undef NODE

