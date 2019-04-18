#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rec_descent_tree.h"
#include "../Tree/tree.h"
#include "../Log/log.h"
#include "../Onegin/textsplit.h" // Specify  -D __DEBUG__  if you want to write to log file

//------------------------------------------------------------------------------------------------------------

 const char* pos = NULL;

//------------------------------------------------------------------------------------------------------------

Node* RecDescentReadTree(FILE* file_ptr, const char* tree_name)
{
    CHECK_LOG(_FILE, file_ptr, return NULL);
 
    long int exp_size = 0;
    char* expression = ReadToAllocatedBuffer(file_ptr, &exp_size);
    CHECK_LOG(_PTR, expression, return NULL);
 
    Node* tree = GetG(expression);
    CHECK_LOG(_PTR, tree, NULL);

    AddNewTreeInfo(tree, tree_name);
    LOG_OK(return NULL);
 
    return tree;
}

Node* GetG(char* expression)
{
    CHECK_LOG(_PTR, expression, return NULL);

    pos = expression;

    Node* expr_node = GetE();

    if(pos == expression)
        PrintLog("Expression hasn't been read (empty expression), line: %d", __LINE__);

    if(*pos != '\0')
        PrintLog("Expression hasn't been read, line: %d", __LINE__);


    return expr_node;
}

Node* GetN()
{
    Node* val_node = NodeCtor(CONST, 0, NULL, NULL, NULL);

    while('0' <= *pos && *pos <= '9')
    {
        val_node->data->content = val_node->data->content * 10 + (*pos++ - '0');
    }

    return val_node;
}


Node* GetE()
{
    Node* val_node = GetT(); 

    while(*pos == '+' || *pos == '-')
    {
        int op = *pos++;

        Node* valve_node = GetT();  

        if      (op == '+')  val_node = NodeCtor(OP, PLUS , NodeCopy(val_node, val_node->parent), valve_node, NULL);
        else if (op == '-')  val_node = NodeCtor(OP, MINUS, NodeCopy(val_node, val_node->parent), valve_node, NULL);
        else    
        {
            PrintLog("Expression hasn't been read, line: %d", __LINE__);
            return NULL;
        }

        val_node->L_child->parent = val_node;
        val_node->R_child->parent = val_node;
    }

    return val_node;
}

Node* GetT()
{
    Node* val_node = GetP();

    while(*pos == '*' || *pos == '/')
    {
        int op = *pos++;

        Node* valve_node = GetP();

        if      (op == '*')  val_node = NodeCtor(OP, MUL, NodeCopy(val_node, val_node->parent), valve_node, NULL);
        else if (op == '/')  val_node = NodeCtor(OP, DIV, NodeCopy(val_node, val_node->parent), valve_node, NULL);
        else    
        {
            PrintLog("Expression hasn't been read, line: %d", __LINE__);
            return NULL;
        }

        val_node->L_child->parent = val_node;
        val_node->R_child->parent = val_node;
    }

    return val_node;
}

Node* GetP()
{
    Node* val_node = NULL;

    if(*pos == '(')
    {
        pos++;
        val_node = GetE();
        if(*pos++ != ')')   PrintLog("Expression hasn't been read, line: %d", __LINE__);

    }
    else if ('a' <= *pos && *pos <= 'z')  val_node = NodeCtor(VAR, *pos++, NULL, NULL, NULL);
    else                                  val_node = GetN();


    return val_node;
}
