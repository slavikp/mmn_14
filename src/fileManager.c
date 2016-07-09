/*
 ============================================================================
 Name        : fileManager.c
 Author      : Slavik Pashanin
 Version     :
 Copyright   : Slavik_pashanin
 Description : This file get *.as file and convert it to linked list
 ============================================================================
 */

#include "fileManager.h"
#include "assembler.h"

/*convert the input file *.as  to linked list of commands
in each line we have link list of commands */

commandStringNode * decodingFile (char* fileName) {
	FILE *file;
	char line[MAX_DATA_NUM];
	char* buff = NULL;
	commandStringNode * headList;
	headList = NULL;
	file = fopen(fileName, "r");
	/* Open File */
	if (file == NULL)
	{
		printf("[file-manager][Error] Can't open the file %s\n",fileName);
		return FALSE;
	}
	else
		printf("[file-manager][Info] Successfully opened the file %s\n",fileName);
	while(fgets(line,MAX_DATA_NUM,file)!= NULL) {
		int index = strlen(line);
		buff = malloc(index);
		memcpy(buff, line, index);
		buff[index] = '\0';
		commandStringNode * item = ConvertLineToLinkList(buff);
		headList = InsertItemToStringList(headList,item);
	}

	fclose(file);

	return headList;
}

/*function that convert command line to link list*/

commandStringNode * ConvertLineToLinkList (char* line) {
	CommandItemNode * head = NULL;
	char * token;
	char * emptyChar = " ,\t\n";  /*chars needed to be skiped*/
	token = strtok(line, emptyChar);
	while(token !=NULL) {
		CommandItemNode * node = (CommandItemNode *)malloc(sizeof(CommandItemNode));
		node -> str = strdup (token);
		node -> next = NULL;
		head = InsertItemToItemList(head , node);
		token = strtok(NULL, emptyChar);
	}
	return head;
}
/*add link list of command line to line item in all list of lines*/

commandStringNode * InsertItemToStringList(commandStringNode * HeadList, CommandItemNode * NodeList){

	if(NodeList == NULL)
		return HeadList;
	commandStringNode * listNode = (commandStringNode *)malloc(sizeof(commandStringNode));
	listNode -> head = NodeList;
	listNode -> next = NULL;
	listNode -> prev = NULL;

	if(HeadList == NULL)
		return listNode;

	commandStringNode * NodeTmp = HeadList;
	while(NodeTmp -> next != NULL){
		NodeTmp = (commandStringNode *)NodeTmp -> next;
	};
	NodeTmp -> next =listNode;
	NodeTmp -> next->prev =  NodeTmp;
	return HeadList;
}
/*copies the  string s into a newly allocated string*/

char *strdup(const char *str)
{
    int n = strlen(str) + 1;
    char *dup = malloc(n);
    if(dup)
    {
        strcpy(dup, str);
    }
    return dup;
}

/*function that insert part of command to link list of row command*/

CommandItemNode * InsertItemToItemList(CommandItemNode * HeadItem, CommandItemNode * NodeItem){

	if(HeadItem == NULL)
		return NodeItem;

	CommandItemNode * NodeTmp = HeadItem;
	while(NodeTmp -> next != NULL){
		NodeTmp = (CommandItemNode *)NodeTmp -> next;
	}
	NodeTmp -> next = NodeItem;

	return HeadItem;
}

/*function printing the linked list*/

void printLineLinkList(commandStringNode * LineList) {
	commandStringNode * LineListTmp = LineList;
	while(LineListTmp != NULL){
		CommandItemNode * ItemTmp = (CommandItemNode *)(LineListTmp -> head);
		while(ItemTmp != NULL){
			printf("%s ", ItemTmp -> str);
			ItemTmp = (CommandItemNode *)ItemTmp -> next;
		}

		LineListTmp = (commandStringNode *)LineListTmp -> next;
		printf("\n");
	}
}
