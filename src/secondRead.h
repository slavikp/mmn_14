/*
 * secondRead.h
 *
 *  Created on: Jul 10, 2016
 *      Author: student
 */

#ifndef SECONDREAD_H_
#define SECONDREAD_H_

secondReadStruct* ConvertFirstReadToSecondRead(firstReadStruct* firstRead);
int runSecondRead(commandStringNode* CommandListHead,secondReadStruct * secondTranstion);
int UpdateSecondReadStruct(secondReadStruct * secondTranstion, CommandItemNode * ItemHead);
void RunSecondReadAction(secondReadStruct * secondTranstion, CommandItemNode * ItemHead, symbolsListNode *Lable);
char* myItoa(int num, char* str, int base);
void WritingLineToFile(char* fileName,char* text);
void buildAction(secondReadStruct * secondTranstion, CommandItemNode * ItemNode,unsigned int paramNum, unsigned int actionNum,char* strAction);
void ConvertParamToData(char SourceOrDest ,secondReadStruct * secondRead ,Data * data , Data * otherParam ,CommandItemNode * item);
void AddRowToTransitionList(secondReadStruct * secondTranstion, Data* data);
void reverse(char* str, int length);
unsigned int GetNumberByAddressing(ADDERSSING Address);
Data* BuiledDataForAction(unsigned int rnd , unsigned int group, unsigned int opcode,unsigned int sourceAddress,unsigned int destAddress,unsigned int ERA);
int getRandomRegister();
PARAMETERTYPE getParameterType(char*param);
void swap(char *i, char *j) ;


#endif /* SECONDREAD_H_ */
