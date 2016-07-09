/*
 * firstRead.h
 *
 *  Created on: Jul 7, 2016
 *      Author: student
 */

#ifndef FIRSTREAD_H_
#define FIRSTREAD_H_


#include "assembler.h"

int firstRead(commandStringNode* CommandListHead);
int updateFirstReadStruct(firstReadStruct * firstRead, CommandItemNode * ItemHead);
int IsRegisterString(char*str);
void AddSymbolToSymbolsList(firstReadStruct * firstTranstion, symbolsListNode *symbol);
int RunFirstReadAction(firstReadStruct * firstTranstion, CommandItemNode * ItemHead, symbolsListNode *Lable);
void AddRowToDataList(firstReadStruct * firstTranstion, Data* data);
void AddSymbolToSymbolsList(firstReadStruct * firstTranstion, symbolsListNode *symbol);
Data* convertCharToUnsignedInt(char Char);
Data* convertIntCharToUnsignedInt(char * Char);
int CheckIfWeHaveTwoOperators(CommandItemNode * ItemNode);
int CheckIfWeHaveOneOperators(CommandItemNode * ItemNode);



#endif /* FIRSTREAD_H_ */
