/*
 ============================================================================
 Name        : fileManager.h
 Author      : Slavik Pashanin
 Version     :
 Copyright   : Slavik_pashanin
 Description : Header file for all function prototype in file-Manger.c
 ============================================================================
 */

#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include "assembler.h"

commandStringNode * decodingFile (char* fileName);
commandStringNode * ConvertLineToLinkList (char* line);
commandStringNode * InsertItemToStringList (commandStringNode * HeadItem, CommandItemNode * NodeItem);
char *strdup(const char *str);
CommandItemNode * InsertItemToItemList(CommandItemNode * HeadItem, CommandItemNode * NodeItem);
void printLineLinkList(commandStringNode * LineList);

#endif /* FILEMANAGER_H_ */
