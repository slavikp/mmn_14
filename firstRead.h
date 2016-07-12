/*
 ====================================================================================
 Name        : assembler.h
 Author      : Slavik Pashanin
 Version     :
 Description : [complete]
 ====================================================================================
 */


#ifndef FIRSTREAD_H_
#define FIRSTREAD_H_


#include "assembler.h"

int runFirstRead(commandStringNode* CommandListHead,firstReadStruct * firstRead);
int UpdateFirstReadStruct(firstReadStruct * firstRead, CommandItemNode * ItemHead);
int IsRegisterString(char*str);
int RunFirstReadAction(firstReadStruct * firstRead, CommandItemNode * ItemHead, symbolsListNode *Lable);
void AddSymbolToSymbolsList(firstReadStruct * firstRead, symbolsListNode *symbol);
void AddRowToDataList(firstReadStruct * firstRead, Data* data);
Data* convertCharToUnsignedInt(char Char);
Data* convertIntCharToUnsignedInt(char * Char);
int CheckIfSourceAddressingOk(char * str, char * action);
int CheckIfDestenationAddressingOk(char * str, char * action);
int CheckIfWeHaveTwoOperators(CommandItemNode * ItemNode);
int IfBothParamRegister(CommandItemNode * ItemNodeS, CommandItemNode * ItemNodeD);
void updateIC(firstReadStruct * firstRead,  symbolsListNode *Lable, int rows);
int CheckIfWeHaveOneOperators(CommandItemNode * ItemNode);
ADDERSSING ChooseAddressType(char * str);



#endif /* FIRSTREAD_H_ */
