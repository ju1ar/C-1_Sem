#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

//-------------------------------------------------------------------------------------------------------------------------

#define FILE_OK(fptr, fname)                                \
{                                                           \
    if(!fptr)                                               \
        fprintf(stderr, "Can't open file %s\n", fname);     \
}

//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------PROTOTYPES--------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------

char*  ReadToAllocatedBuffer(FILE* fptr, const char* fname, long int* bufsize);
char** AllocateArrayOfStrs(char* buf, int* arrsize);
int CompareStrsFromBeg(const void * str1, const void * str2);
int CompareStrsFromEnd(const void * str1, const void * str2);
char* AllocateNewBuf(char* buf, long int bufsize, long int* newbufsize);
void WriteStrs(FILE* fptr, const char* fname, char** strs, long int numofstrs);
void WriteBuf(FILE* fptr, const char* fname, char* buf, long int bufsize);

//------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------MAIN----------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------ 

int main()
{   
    const char* uneditedpoem = "UneditedPoem.txt";  // File with an unedited poem (spaces,
                                                    // empty strings, numbers of stanzas)
    FILE* uneditedpoem_ptr = fopen(uneditedpoem, "r");

    long int bufsize = 0;
    char* buffer = ReadToAllocatedBuffer(uneditedpoem_ptr, uneditedpoem, &bufsize);  // Write content of file to buffer

    long int newbufsize = 0;
    char* newbuffer = AllocateNewBuf(buffer, bufsize, &newbufsize);  // Rewrite content of buffer to allocated 
                                                                     // new buffer without unnecessary symbols 
    int arrsize = 0;
    char** arrofstrs = AllocateArrayOfStrs(newbuffer, &arrsize);  // Allocate array and write pointers to strings into it

    const char* byalph = "PoemByAlphabet.txt";
    FILE* byalph_ptr  = fopen(byalph, "w");

    const char* byalphend = "PoemByAlphabetFromEnd.txt";
    FILE* byalphend_ptr = fopen(byalphend, "w");

    const char*  editedpoem = "EditedPoem.txt";
    FILE* editedpoem_ptr = fopen(editedpoem, "w");

    qsort(arrofstrs, arrsize, sizeof(*arrofstrs), CompareStrsFromBeg);
    WriteStrs(byalph_ptr, byalph, arrofstrs, arrsize);

    qsort(arrofstrs, arrsize, sizeof(*arrofstrs), CompareStrsFromEnd);
    WriteStrs(byalphend_ptr, byalphend, arrofstrs, arrsize);

    WriteBuf(editedpoem_ptr, editedpoem, newbuffer, newbufsize);

    fclose(uneditedpoem_ptr);
    fclose(byalph_ptr);
    fclose(byalphend_ptr);
    fclose(editedpoem_ptr);

    free(buffer);
    free(newbuffer);
    free(arrofstrs);

    return 0;
}

//-------------------------------------------------------------------------------------------------------------------------
//! @brief  Write the content of file to buffer
//!
//! @param[in]   fptr     Pointer to file
//! @param[in]   fname    Name of file
//! @param[out]  bufsize  Size of buffer
//!
//! @return  buf  Buffer with content(unedited poem) of file 
//!
//! @note Change last symbol('\n') of file for '\0'. It allowed to use function strlen(const char* str) 
//-------------------------------------------------------------------------------------------------------------------------

char*  ReadToAllocatedBuffer(FILE* fptr, const char* fname, long int* bufsize)
{
    FILE_OK(fptr, fname);
    assert(bufsize);

    fseek(fptr, 0, SEEK_END);
    *bufsize = ftell(fptr);
    rewind(fptr);

    char* buf = (char*)calloc(*bufsize, sizeof(*buf));
    assert(buf);

    if(fread(buf, sizeof(*buf), *bufsize, fptr) != (size_t)*bufsize)
        fprintf(stderr, "Reading error(fread), file %s\n", fname);

    buf[*bufsize - 1] = '\0'; 

    return buf;
}

//-------------------------------------------------------------------------------------------------------------------------
//! @brief  Rewrite content of buffer(buf) to allocated new buffer(newbuf)  without unnecessary symbols 
//!
//! @param[in]   buf         Buffer with unedited poem
//! @param[in]   bufsise     Size of buf
//! @param[out]  newbufsize  Pointer to size of returned new buffer
//!
//! @return  newbuf  Buffer with content(edited poem) of file 
//!
//! @note  This function is not universal. It is only for Russian language and for special edition.
//-------------------------------------------------------------------------------------------------------------------------

char* AllocateNewBuf(char* buf, long int bufsize, long int* newbufsize)
{
    assert(buf);
    assert(newbufsize);

    char* newbuf = (char*)calloc(bufsize, sizeof(*newbuf));
    assert(newbuf);

    int y = 0;

    for(int x = 1; x < bufsize - 1; x++)
    {
        if(!isspace(buf[x])                 &&   // Don't write spaces
           buf[x] != 'I' && buf[x] != 'V'   &&   // Don't write number of stanza   
           buf[x] != 'X' && buf[x] != 'L')       
                newbuf[y++] = buf[x];


        if(buf[x - 1] != ' ' && buf[x] == ' ' && buf[x + 1] != ' ')   // Write spaces between words and punctuations marks
                newbuf[y++] = buf[x];


        if((ispunct(buf[x - 1]) || buf[x - 1] < 0) &&   // The string ends with a punctuation or with
            buf[x] == '\n')                             // russian letter(second byte of letter is always less than zero)  
                newbuf[y++] = '\n';
    }
    *newbufsize = y + 1;  // +1 for '\0' (previous calloc)

    newbuf = (char*)realloc(newbuf, *newbufsize * sizeof(*newbuf)); 
    assert(newbuf);
    
    return newbuf;
}

//-------------------------------------------------------------------------------------------------------------------------
//! @brief  Allocate array of pointers to beginning of strings
//!
//! @param[in]   buf       Buffer with edited poem
//! @param[out]  arrsize   Pointer to size of returned new array
//!
//! @return  arrofstrs  Array of pointers to beginning of strings
//-------------------------------------------------------------------------------------------------------------------------

char** AllocateArrayOfStrs(char* buf, int* arrsize)
{
    assert(buf);
    assert(arrsize);
    
    for(char* pos = strchr(buf, '\n'); pos; pos = strchr(pos + 1, '\n'))
    {
        *pos = '\0';
        (*arrsize)++;
    }
    (*arrsize)++; // +1 for the last string

    char** arrofstrs = (char**)calloc(*arrsize, sizeof(*arrofstrs));
    assert(arrofstrs);

    arrofstrs[0] = buf;

    for(int x = 0, y = 1; y < *arrsize; x++)
        if(buf[x] == '\0') arrofstrs[y++] = (buf + x) + 1;

    return arrofstrs;
} 

//-------------------------------------------------------------------------------------------------------------------------
//! @brief  Compare two strings by alphabet
//!
//! @param[in]   str1  
//! @param[in]   str2   
//!
//! @return  n  The result of comparing two strings
//-------------------------------------------------------------------------------------------------------------------------

int CompareStrsFromBeg(const void* str1, const void* str2)
{
    assert(str1);
    assert(str2);

    const char* cmpstr1 = *(const char**)str1;
    const char* cmpstr2 = *(const char**)str2;

    int n = 0;
    int x = 0, y = 0;
    
    while(!n)
    {
        while(isspace(cmpstr1[x]) || isdigit(cmpstr1[x]) || ispunct(cmpstr1[x]))  x++;
        while(isspace(cmpstr2[y]) || isdigit(cmpstr2[y]) || ispunct(cmpstr2[y]))  y++;

        n = (int)cmpstr1[x++] - (int)cmpstr2[y++];

        if(cmpstr1[x] == '\0' || cmpstr2[y] == '\0')  n = 1;
    }

    return n;
}

//-------------------------------------------------------------------------------------------------------------------------
//! @brief  Compare two strings by alphabet from end
//!
//! @param[in]   str1  
//! @param[in]   str2   
//!
//! @return  n  The result of comparing two strings
//-------------------------------------------------------------------------------------------------------------------------

int CompareStrsFromEnd(const void * str1, const void * str2)
{
    
    assert(str1);
    assert(str2);

    const char* cmpstr_end1 = *(const char**)str1;
    const char* cmpstr_end2 = *(const char**)str2;
    
    int x = strlen(cmpstr_end1) - 1;
    int y = strlen(cmpstr_end2) - 1;

    int n = 0;

    while(!n) 
    {
        while(isspace(cmpstr_end1[x]) || isdigit(cmpstr_end1[x]) || ispunct(cmpstr_end1[x]))  x--;
        while(isspace(cmpstr_end2[y]) || isdigit(cmpstr_end2[y]) || ispunct(cmpstr_end2[y]))  y--;

        n = (int)cmpstr_end1[x--] - (int)cmpstr_end2[y--];
        
        if(!x || !y) n = 0;
    }

    return n;
}

//-------------------------------------------------------------------------------------------------------------------------
//! @brief  Write strings to file 
//!
//! @param[in]   fptr       Pointer to file  
//! @param[in]   fname      Name of file 
//! @param[in]   strs       Array of strings
//! @param[in]   numofstrs  Number of strings
//-------------------------------------------------------------------------------------------------------------------------

void WriteStrs(FILE* fptr, const char* fname, char** strs, long int numofstrs)
{
    assert(strs);
    FILE_OK(fptr, fname);

    for(int i = 0; i < numofstrs; i++) 
        fprintf(fptr, "%s\n", strs[i]);
}

//-------------------------------------------------------------------------------------------------------------------------
//! @brief  Write buffer to file 
//!
//! @param[in]   fptr     Pointer to file  
//! @param[in]   fname    Name of file 
//! @param[in]   buf      Buffer with edited poem 
//! @param[in]   bufsize  Size of buffer
//-------------------------------------------------------------------------------------------------------------------------

void WriteBuf(FILE* fptr, const char* fname, char* buf, long int bufsize)
{
    assert(buf);
    FILE_OK(fptr, fname);

    for(int i = 1; i < bufsize - 1; i++) 
        if(buf[i] == '\0')   
            fprintf(fptr, "%s\n", (buf + i) + 1);
}

