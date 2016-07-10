/*
 ====================================================================================
 Name        : assembler.h
 Author      : Slavik Pashanin
 Version     :
 Description : General header file for the assembly.
		       Contains Constant, macros, data structures and methods declaration.
 ====================================================================================
 */

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>


/* ======== Macros ======== */
/* Utilities */
#define BYTE_SIZE			8
#define FALSE				0
#define TRUE				1
#define EXIT_SUCSSES		0
#define EXIT_FAILURE		1

/* Given Constants */
#define MAX_DATA_NUM		1000
#define FIRST_ADDRESS		100
#define MAX_LINE_LENGTH		80
#define MAX_LABEL_LENGTH	30
#define MEMORY_WORD_LENGTH	15
#define MAX_REGISTER_DIGIT	7
#define FILE_NAME_MAX       4096

/* Defining Constants */
#define MAX_LINES_NUM		700
#define MAX_LABELS_NUM		0

typedef  struct {
	unsigned int ERA 	:2;
	unsigned int dest   :2;
	unsigned int src    :2;
	unsigned int opcode :4;
	unsigned int grp    :2;
	unsigned int dummy  :3;
} command;

typedef struct commandStringNode {
	struct CommandItemNode *head;
	struct commandStringNode *next;
	struct commandStringNode *prev;
} commandStringNode;

typedef struct CommandItemNode {
	char * str;
	struct CommandItemNode *next;
} CommandItemNode;

typedef struct {
	unsigned int bytes : 15 ;
} Data;

typedef struct firstReadStruct {
	unsigned int IC;
	Data* instructionsList[MAX_DATA_NUM];
	unsigned int DC;
	Data* DataList[MAX_DATA_NUM];
	struct symbolsListNode *symbolHead;
} firstReadStruct;

typedef struct secondReadStruct{
	unsigned int oldIC;
	unsigned int oldDC;
	unsigned int IC;
	Data* InstructionsList[2000];
	struct SymbolsListNode *SymbolesHead;
} secondReadStruct;

typedef struct symbolsListNode{
	unsigned int Action;
	unsigned int Extract;
	unsigned int Address;
	char* Label;
	struct symbolsListNode *next;
} symbolsListNode;

#define STRING_COMMENT  ";"
#define STRING_LABEL    ":"
/*Guidance Statements*/
#define STR_STRING      ".string"
#define STRDATA ".data"
#define STRENTRY ".entry"
#define STREXTERN ".extern"
/*Action Statements -1*/
#define STRMOV "mov"
#define STRCMP "cmp"
#define STRADD "add"
#define STRSUB "sub"
#define STRLEA "lea"
/*Action Statements -2*/
#define STRNOT "not"
#define STRCLR "clr"
#define STRINC "inc"
#define STRDEC "dec"
#define STRJMP "jmp"
#define STRBNE "bne"
#define STRRED "red"
#define STRPRN "prn"
#define STRJSR "jsr"
/*Action Statements -3*/
#define STRRTS "rts"
#define STRSTOP "stop"
#define INPUTFILENAME "prog1.ob"
#define EXTFILENAME "prog1.ext"
#define ENTFILENAME "prog1.ent"

/*Action Statements -1*/
#define ACTIONMOV 0
#define ACTIONCMP 1
#define ACTIONADD 2
#define ACTIONSUB 3
#define ACTIONLEA 6
/*Action Statements -2*/
#define ACTIONNOT 4
#define ACTIONCLR 5
#define ACTIONINC 7
#define ACTIONDEC 8
#define ACTIONJMP 9
#define ACTIONBNE 10
#define ACTIONRED 11
#define ACTIONPRN 12
#define ACTIONJSR 13
/*Action Statements -3*/
#define ACTIONRTS 14
#define ACTIONSTOP 15


typedef enum {
	IMMEDIATEADDERSSING, DIRECTADDERSSING,
	RANDOMADDERSSING1,RANDOMADDERSSING2,
	RANDOMADDERSSING3, DIRECTREGISTERADDERSSING
} ADDERSSING;

typedef enum {
	REGISTER, LABLE, NUMBER
}PARAMETERTYPE;

#endif /* ASSEMBLER_H */
