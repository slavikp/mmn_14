/*
 * fileManager.h
 *
 *  Created on: Jul 4, 2016
 *      Author: student
 */

#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_


#include "assembler.h"

commandStringNode * decodingFile (char* fileName);
commandStringNode * ConvertLineToLinkList (char* line);
commandStringNode * InsertItemToCommandList(commandStringNode * HeadList, CommandItemNode * NodeList);
char *strdup(const char *str);
CommandItemNode * InsertItemToItemList(CommandItemNode * HeadItem, CommandItemNode * NodeItem);
commandStringNode * InsertItemToStringList (commandStringNode * HeadItem, CommandItemNode*NodeItem);
void printLineLinkList(commandStringNode * LineList);



#endif /* FILEMANAGER_H_ */
