#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Tree/tree.h"
#include "../Onegin/textsplit.h"
#include "../Stack/stack.h"
#include "../Log/log.h"  // Specify  -D __DEBUG__  if you want to write to log file
//-------------------------------------------------------------------------------------------------------------

#define COMMAND_SAY "say "

#define dbl2ptr(dbl) (*((char**)&(dbl)))

#define MAKE_BUF_AND_ASK(buf_name)                  \
                                                    \
char buf_name[BYTES_IN_RUS_LETTER + 1] = {};        \
fgets(buf_name, BYTES_IN_RUS_LETTER + 1, stdin);    \
fpurge(stdin);                                      \


#define REPEAT_YES_NO                               \
                                                    \
while(yes_no[1] != "Д"[1] && yes_no[1] != "Н"[1])   \
{                                                   \
    printf("Ответьте   | [Д]а | [Н]ет |\n");        \
    fgets(yes_no, BYTES_IN_RUS_LETTER + 1, stdin);  \
    fpurge(stdin);                                  \
}

const int BYTES_IN_RUS_LETTER = 2;
const int MAX_STR_LEN         = 128;

enum Feature
{
    NO  = 0,
    YES = 1 
};

//-------------------------------------------------------------------------------------------------------------

Node* AkinatorFindObject(Node* aki_node, const char* object_name);
char* AllocateGetStr(int str_size);
log_error_t AkinatorRestsrtGame(Node* aki_tree);
log_error_t Akinator(Node* aki_node);
log_error_t AkinatorInfo();
log_error_t AkinatorMenu(Node* aki_node);
log_error_t AkinatorGuess(Node* aki_node);
log_error_t AkinatorAnswer(Node* aki_node);
log_error_t AkinatorQuestion(Node* aki_node);
log_error_t AkinatorGame(Node* aki_tree);
log_error_t AkinatorNewObject(Node* aki_node);
log_error_t PrintAkinatorArt();
log_error_t AkinatorDefine(Node* aki_tree);
log_error_t CollectDefinition(Node* object_node);
log_error_t SayDefinition(Stack* stack_def, Stack* stack_yes_no);
log_error_t CollectDefinition(Node* object_node, Stack* stack_def, Stack* stack_yes_no);
log_error_t SayComparison(Stack* first_stack_yes_no , Stack* first_stack_def , char* object_name1,
                          Stack* second_stack_yes_no, Stack* second_stack_def, char* object_name2);
char* AskCollectDefinition(Node* aki_tree, Stack* stack_def, Stack* stack_yes_no, int object_num);
log_error_t AkinatorCompare(Node* aki_tree);
log_error_t LastAnswer(int answer, double difference);

//-------------------------------------------------------------------------------------------------------------

int main()
{
    LogCtor();
    printf("Yes\n");

    FILE* aki_ptr = fopen("aki_data.txt", "r");
    CHECK_LOG(_FILE, aki_ptr, return 1);

    Node* aki_tree = ReadTreeWrap(NULL, aki_ptr, "Akinator");
    CHECK_LOG(_PTR, aki_tree, return 1);

    Akinator(aki_tree);

    aki_ptr = fopen("aki_data.txt", "w");
    CHECK_LOG(_FILE, aki_ptr, return 1);

    PrintTreeWrap(aki_tree, aki_ptr);

    DumpTreeDot(aki_tree, "check.dot");

//  system("dot -Tpng check.dot -o check.png"); 
//  system("open check.png"); 

    TreeDtorWrap(&aki_tree);
    TreesInfoDtor();

    fclose(aki_ptr);
    LogDtor();

    return 0;
}

log_error_t Akinator(Node* aki_tree)
{
    CHECK_LOG(_PTR, aki_tree, return LOG_BAD_PTR);

    PrintAkinatorArt();
    AkinatorMenu(aki_tree);

    system("say Прощай игрок, возвращайся скорее");
    system("clear");
    
    return LOG_NOERR;
}

log_error_t AkinatorMenu(Node* aki_tree)
{
    CHECK_LOG(_PTR, aki_tree, return LOG_BAD_PTR);
    printf("Нажмите   | [Н]ачать игру | [И]нформация об игре | [Ф]ото Полторашки | [В]ыйти из игры |\n");

    MAKE_BUF_AND_ASK(menu_type);

    switch(menu_type[1])
    {
        case "Н"[1]:                                    
            AkinatorGame(aki_tree);
            break;

        case "И"[1]:                                    
            AkinatorInfo();
            AkinatorMenu(aki_tree);
            break;

        case "Ф"[1]:                                    
            system("open Полторашка.jpg");
            AkinatorMenu(aki_tree);
            break;

        case "В"[1]:                                    
            return LOG_NOERR;
            break;

        default:
            AkinatorMenu(aki_tree);
    }

    NODES_NUM_REFRES(aki_tree);
    return LOG_NOERR;
}

log_error_t AkinatorInfo()
{
    printf("--------------------------------------------\n"
           "Максимальная длина входной строки - %d\n"
           "Озвучил Акинатора Юрий Макось\n\n"
           "© Tolstov Entertainment  2018\n"
           "--------------------------------------------\n", MAX_STR_LEN / BYTES_IN_RUS_LETTER);

    return LOG_NOERR;
}

log_error_t PrintAkinatorArt()
{
    FILE* asci_ptr = fopen("asci_akinator.txt", "r");
    CHECK_LOG(_FILE, asci_ptr, return LOG_BAD_FILE);

    long int buf_size = 0;
    char* asci_art_aki = ReadToAllocatedBuffer(asci_ptr, &buf_size);
    CHECK_LOG(_PTR, asci_art_aki, return LOG_BAD_PTR);

    printf("%s\n", asci_art_aki);
    
    return LOG_NOERR;
}

log_error_t AkinatorGame(Node* aki_tree)
{
    CHECK_LOG(_PTR, aki_tree, return LOG_BAD_PTR);
    system("say Приветствую тебя, игрок");
    printf("Нажмите   | [У]гадать объект | [О]пределить объект | [С]равнить объекты | [М]еню\n");

    MAKE_BUF_AND_ASK(game_type);

    switch(game_type[1])
    {
        case "У"[1]:                                    //[У]гадать
            AkinatorGuess(aki_tree);
            break;

        case "О"[1]:                                    //[О]пределить
            AkinatorDefine(aki_tree);
            break;

        case "С"[1]:                                    //[C]равнить
            AkinatorCompare(aki_tree);
            break;

        case "М"[1]:                                    
            AkinatorMenu(aki_tree);
            break;

        default:
            AkinatorGame(aki_tree);
    }

    AkinatorRestsrtGame(aki_tree);


    return LOG_NOERR;
}
//------------------------------------------------------------------------------------------------------------
//BAD CODE
log_error_t AkinatorCompare(Node* aki_tree)
{
    CHECK_LOG(_PTR, aki_tree, return LOG_BAD_PTR);

    Stack first_stack_def= {};
    StackCtor(&first_stack_def, 10);

    Stack first_stack_yes_no= {};
    StackCtor(&first_stack_yes_no, 10);
    char* object_name1 = AskCollectDefinition(aki_tree, &first_stack_def, &first_stack_yes_no, 1);

    Stack second_stack_def = {};
    StackCtor(&second_stack_def, 10);

    Stack second_stack_yes_no = {};
    StackCtor(&second_stack_yes_no, 10);
    char* object_name2 = AskCollectDefinition(aki_tree, &second_stack_def, &second_stack_yes_no, 2);

    SayComparison(&first_stack_yes_no , &first_stack_def  , object_name1,
                  &second_stack_yes_no, &second_stack_def,  object_name2);

    StackDtor(&first_stack_def);
    StackDtor(&first_stack_yes_no);
    free(object_name1);

    StackDtor(&second_stack_def);
    StackDtor(&second_stack_yes_no);
    free(object_name2);

    return LOG_NOERR;
}

log_error_t SayComparison(Stack* first_stack_yes_no , Stack* first_stack_def , char* object_name1,
                          Stack* second_stack_yes_no, Stack* second_stack_def, char* object_name2)
{
    CHECK_LOG(_PTR, first_stack_def    , return LOG_BAD_PTR);
    CHECK_LOG(_PTR, first_stack_yes_no , return LOG_BAD_PTR);
    CHECK_LOG(_PTR, second_stack_def   , return LOG_BAD_PTR);
    CHECK_LOG(_PTR, second_stack_yes_no, return LOG_BAD_PTR);
    
    int answer = -1;
    int count = 0;

    while((answer = StackPop(first_stack_yes_no)) == StackPop(second_stack_yes_no))
    {
        if(count == 0)
        {
            char answer1[MAX_STR_LEN] = {};
            sprintf(answer1, COMMAND_SAY"И %s, и %s", object_name1, object_name2);
            system(answer1);
        }

        if(answer == NO) system(COMMAND_SAY"не");
        double obj_def = StackPop(first_stack_def);
        char answer[MAX_STR_LEN] = {};
        sprintf(answer, COMMAND_SAY"%s", dbl2ptr(obj_def));
        system(answer);

        count++;
    }

    double difference = StackPop(first_stack_def);
    if(count > 0)
    {
        system("say Но");  
    }
    char answer2[MAX_STR_LEN] = {};
    sprintf(answer2, COMMAND_SAY"%s", object_name1);
    system(answer2);
    LastAnswer(answer, difference);

    char answer3[MAX_STR_LEN] = {};
    sprintf(answer3, COMMAND_SAY"А %s", object_name2);
    system(answer3);
    LastAnswer(!answer, difference);

    return LOG_NOERR;
}

log_error_t LastAnswer(int answer, double difference)
{
    if(answer == NO) system(COMMAND_SAY"не");
    char answer_[MAX_STR_LEN] = {};
    sprintf(answer_, COMMAND_SAY"%s", dbl2ptr(difference));
    system(answer_);

    return LOG_NOERR;
}


char* AskCollectDefinition(Node* aki_tree, Stack* stack_def, Stack* stack_yes_no, int object_num)
{
    CHECK_LOG(_PTR, aki_tree    , return NULL);
    CHECK_LOG(_PTR, stack_def   , return NULL);
    CHECK_LOG(_PTR, stack_yes_no, return NULL);

    if      (object_num == 1)                                                               
    {                                                                                       
        system(COMMAND_SAY"Введите имя первого объекта");                                   
        printf("Введите имя первого объекта #   ");                                         
    }                                                                                       
    else if (object_num == 2)                                                               
    {                                                                                       
        system(COMMAND_SAY"Введите имя второго объекта");                                   
        printf("Введите имя второго объекта #   ");                                         
    }                                                                                       
                                                                                        
    char* object_name = AllocateGetStr(MAX_STR_LEN);                            
    CHECK_LOG(_PTR, object_name, return NULL);
    Node* object_node = AkinatorFindObject(aki_tree, object_name);  

    while(!object_node)
    {
        if      (object_num == 1)                                                               
        {                                                                                       
            system(COMMAND_SAY"Введите имя первого объекта корректно");                                   
            printf("Введите имя первого объекта корректно #   ");                                         
        }                                                                                       
        else if (object_num == 2)                                                               
        {                                                                                       
            system(COMMAND_SAY"Введите имя второго объекта корректно");                                   
            printf("Введите имя второго объекта  корректно #   ");                                         
        }                                                                                       

        object_name = AllocateGetStr(MAX_STR_LEN);                         
        object_node = AkinatorFindObject(aki_tree, object_name); 
    }

    CollectDefinition(object_node, stack_def, stack_yes_no);

    return object_name;
}
//------------------------------------------------------------------------------------------------------------

log_error_t AkinatorDefine(Node* aki_tree)
{
    CHECK_LOG(_PTR, aki_tree, return LOG_BAD_PTR);

    system(COMMAND_SAY"Введите имя объекта");
    printf("Введите имя объекта #   ");
     
    char* object_name = AllocateGetStr(MAX_STR_LEN);
    Node* object_node = AkinatorFindObject(aki_tree, object_name);
    if(!object_node)
    {
        printf("Введите имя корректно #   ");    
        AkinatorDefine(aki_tree);
    }
    else
    {
        free(object_name);

        Stack stack_def = {};
        StackCtor(&stack_def, 10);

        Stack stack_yes_no = {};
        StackCtor(&stack_yes_no, 10);

        CollectDefinition(object_node, &stack_def, &stack_yes_no);
        SayDefinition(&stack_def, &stack_yes_no);

        StackDtor(&stack_def);
        StackDtor(&stack_yes_no);
    }

    return LOG_NOERR;
}

log_error_t CollectDefinition(Node* object_node, Stack* stack_def, Stack* stack_yes_no)
{
    CHECK_LOG(_PTR, object_node , return LOG_BAD_PTR);
    CHECK_LOG(_PTR, stack_def   , return LOG_BAD_PTR);
    CHECK_LOG(_PTR, stack_yes_no, return LOG_BAD_PTR);

    Node* temp = NULL;
    Node* _parent_= object_node;

    while(_parent_->parent != NULL)
    {
        temp = _parent_;
        _parent_ = _parent_->parent;
        StackPush(stack_def, _parent_->data->content);

        if(_parent_->L_child == temp)  StackPush(stack_yes_no, YES);
        else                           StackPush(stack_yes_no, NO);
    }
    
    return LOG_NOERR;
}

log_error_t SayDefinition(Stack* stack_def, Stack* stack_yes_no)
{
    CHECK_LOG(_PTR, stack_def   , return LOG_BAD_PTR);
    CHECK_LOG(_PTR, stack_yes_no, return LOG_BAD_PTR);

    system(COMMAND_SAY"Это ");

    while(stack_def->size)
    {
        if(StackPop(stack_yes_no) == NO) system(COMMAND_SAY"не");
        {
        double obj_def = StackPop(stack_def);
        char answer[MAX_STR_LEN] = {};
        sprintf(answer, COMMAND_SAY"%s", dbl2ptr(obj_def));
        system(answer);
        }
    }

    system(COMMAND_SAY"Вот и всё");

    return LOG_NOERR;
}

log_error_t AkinatorRestsrtGame(Node* aki_tree)
{
    CHECK_LOG(_PTR, aki_tree, return LOG_BAD_PTR);

    printf("Нажмите   | [Н]ачать игру заново | [В]ыйти из игры |\n");

    MAKE_BUF_AND_ASK(continue_type);

    switch(continue_type[1])
    {
        case "Н"[1]:                                    
            AkinatorGame(aki_tree);
            break;

        case "В"[1]:                                   
            return LOG_NOERR;
            break;

        default:
            AkinatorRestsrtGame(aki_tree);
    }
    
    return LOG_NOERR;
}

log_error_t AkinatorGuess(Node* aki_node)
{
    CHECK_LOG(_PTR, aki_node, return LOG_BAD_PTR);

    if(aki_node->L_child && aki_node->R_child)
        AkinatorQuestion(aki_node);
    else
        AkinatorAnswer(aki_node);

    return LOG_NOERR;
}

log_error_t AkinatorAnswer(Node* aki_node)
{
    CHECK_LOG(_PTR, aki_node, return LOG_BAD_PTR);

    char answer[MAX_STR_LEN] = {};
    sprintf(answer, COMMAND_SAY"Я думаю, это %s! Я угадал?", dbl2ptr(aki_node->data->content));
    system(answer);

    printf("Ответьте   | [Д]а | [Н]ет |\n");

    MAKE_BUF_AND_ASK(yes_no);
    REPEAT_YES_NO;

    if     (yes_no[1] == "Д"[1])
        system("say Я всё знаю");               
    else if(yes_no[1] == "Н"[1])
    {
        AkinatorNewObject(aki_node);
        system("say Теперь буду знать");
    }

    return LOG_NOERR;
}

log_error_t AkinatorNewObject(Node* aki_node)
{
    CHECK_LOG(_PTR, aki_node, return LOG_BAD_PTR);
    
    system("say А кто же это?");

    printf("Введите имя объекта #  ");

    char* new_object_str = AllocateGetStr(MAX_STR_LEN);
    CHECK_LOG(_PTR, new_object_str, return LOG_BAD_PTR);

    char answer[MAX_STR_LEN] = {};
    sprintf(answer, COMMAND_SAY"Чем %s отличается от %s", new_object_str, dbl2ptr(aki_node->data->content));
    system(answer);

    printf("Введите отличительный признак #  ");

    char* difference = AllocateGetStr(MAX_STR_LEN);
    CHECK_LOG(_PTR, difference, return LOG_BAD_PTR);


    double new_object_dbl = 0;
    dbl2ptr(new_object_dbl) = new_object_str;

    double old_object_dbl = 0;
    dbl2ptr(old_object_dbl) = dbl2ptr(aki_node->data->content);

    Node* new_object_node = NodeCtor(STR, new_object_dbl, NULL, NULL, aki_node);
    Node* old_object_node = NodeCtor(STR, old_object_dbl, NULL, NULL, aki_node);

    dbl2ptr(aki_node->data->content) = difference;
    aki_node->L_child = new_object_node;
    aki_node->R_child = old_object_node;


    return LOG_NOERR;
}

char* AllocateGetStr(int str_size)
{
    char* str = (char*)calloc(str_size + 1, sizeof(*str));
    CHECK_LOG(_PTR, str, return NULL);
    fgets(str, str_size, stdin);
    fpurge(stdin);

    char* symb = strchr(str, '\n'); 
    *symb = '\0';
    
    return str;
}

log_error_t AkinatorQuestion(Node* aki_node)
{
    CHECK_LOG(_PTR, aki_node, return LOG_BAD_PTR);

    char question[MAX_STR_LEN] = {};
    sprintf(question, COMMAND_SAY"Это %s?", dbl2ptr(aki_node->data->content));
    system(question);

    printf("Ответьте   | [Д]а | [Н]ет |\n");

    MAKE_BUF_AND_ASK(yes_no);

    REPEAT_YES_NO;

    if     (yes_no[1] == "Д"[1])
        AkinatorGuess(aki_node->L_child);
    else if(yes_no[1] == "Н"[1])
        AkinatorGuess(aki_node->R_child);

    return LOG_NOERR;
}

Node* AkinatorFindObject(Node* aki_node, const char* object_name)
{
    if(!aki_node) return NULL;

    if(strcmp(dbl2ptr(aki_node->data->content), object_name) == 0)   // '\n'
         return aki_node;

    Node* object_node = AkinatorFindObject(aki_node->L_child, object_name);
    if(!object_node) 
          object_node = AkinatorFindObject(aki_node->R_child, object_name);

    return object_node;
}
