/*
 ============================================================================
 Name        : firstRead.c
 Author      : Slavik Pashanin
 Version     :
 Copyright   : Slavik_pashanin
 Description : [complete]
 ============================================================================
 */

#include "firstRead.h"
#include "assembler.h"
#include "fileManager.h"

/*first transition function*/
int runFirstRead(commandStringNode* CommandListHead,firstReadStruct * firstRead){
	commandStringNode* CommandListNodeTmp = CommandListHead;
	while(CommandListNodeTmp != NULL){
		/*update the sturct with symbols, data , transitions and counters*/
		int result = UpdateFirstReadStruct(firstRead, (CommandItemNode *)CommandListNodeTmp->head);
		/*if the command line was with error remove line*/
		if(result == EXIT_FAILURE){
			/*remove command line form list - we don't want to run on this line in second transition*/
			commandStringNode * failNext = (commandStringNode *)CommandListNodeTmp-> next;
			CommandListNodeTmp = (commandStringNode *)CommandListNodeTmp-> prev;
			if(CommandListNodeTmp != NULL){
				CommandListNodeTmp->next = failNext;
				failNext->prev = CommandListNodeTmp;
				/*move to next line*/
				CommandListNodeTmp = (commandStringNode*)CommandListNodeTmp->next;
			}else{
				/*if we delete from root list*/
				CommandListHead->head = failNext->head;
				CommandListHead->next = failNext->next;
				CommandListNodeTmp = failNext;
			}
		}else{
			/*move to next line*/
			CommandListNodeTmp = (commandStringNode*)CommandListNodeTmp->next;
		}
	}
	return EXIT_SUCCESS;
}

int UpdateFirstReadStruct(firstReadStruct * firstRead, CommandItemNode * ItemHead){

	/*comment Statements*/
	if(strncmp(ItemHead -> str, STRING_COMMENT ,1) == 0){
		printf("[info] comment%s\n", ItemHead -> str);
		return EXIT_FAILURE;
	}
	/*label Statements*/
	if((ItemHead -> str)[strlen(ItemHead -> str)-1] ==  STRING_LABEL[0] ){
		printf("[Info] Add Label: %s\n", ItemHead -> str);
		/*if label length bigger then 30 char*/
		if(strlen(ItemHead ->str) > MAX_LABEL_LENGTH ){
			printf("[Error] label length bigger than 30\n");
			return EXIT_FAILURE;
		}
		/*if is register label*/
		if(IsRegisterString(ItemHead -> str) == 0){
			printf("[Error] label have register name\n");
			return EXIT_FAILURE;
		}
		/*create symbol node for symbols list*/
		symbolsListNode *Lable = (symbolsListNode *)malloc(sizeof(symbolsListNode));
		Lable->Label = ItemHead -> str;
		Lable->Address = 0;
		Lable->Extract = 0;
		Lable->Action = 0;
		Lable->next = NULL;
		/*run the action and update the symbol*/
		int result = RunFirstReadAction(firstRead, (CommandItemNode *)ItemHead-> next, Lable);

		if(result == EXIT_SUCCESS){
			/*insert to symbols list*/
			AddSymbolToSymbolsList(firstRead, Lable);
			return EXIT_SUCCESS;
		}
		else
			return EXIT_FAILURE;
	}

	/*Action Statements*/
	return RunFirstReadAction(firstRead, ItemHead, NULL);
}

int RunFirstReadAction(firstReadStruct * firstRead, CommandItemNode * ItemHead, symbolsListNode *Lable){
	printf("[Info] Action: %s\n", ItemHead -> str);

	/*run an all action - check for error in actions and if not error update the IC counter,*/
	/*in data, string, extern, entry insert if we need to lists (if we need)*/
	if((strcmp(ItemHead-> str, STR_STRING)) == 0){
		if(ItemHead->next ==NULL)
		{
			printf("[Error] not getting %s string\n",ItemHead-> str);
			return EXIT_FAILURE;
		}
		/*if we need to update symbols list*/
		if(Lable != NULL)
			Lable->Address = firstRead-> DC;
		/* get string to save*/
		char * String= ((CommandItemNode *)ItemHead->next)-> str;
		size_t StringLen = strlen(String);
		int i = 1;
		/*add data rows to struct*/
		while(i < StringLen-1){
			AddRowToDataList(firstRead, convertCharToUnsignedInt(String[i]));
			i++;
		}
		/*update the data list*/
		Data* endStr = (Data*)malloc(sizeof(Data));
		endStr->bytes = 0;
		AddRowToDataList(firstRead, endStr);
		return EXIT_SUCCESS;

	}else if(strcmp(ItemHead-> str, STR_DATA)== 0){
		/*get next data int*/
		CommandItemNode *tmp =  (CommandItemNode *)ItemHead->next;
		/*if we have label before action and if we have one int or more*/
		if(Lable != NULL && tmp !=NULL)
			Lable->Address = firstRead->DC;
		/*add all ints to data list*/
		while(tmp != NULL){
			AddRowToDataList(firstRead, convertIntCharToUnsignedInt(tmp-> str));
			tmp=(CommandItemNode *)tmp->next;
		}
	}else if(strcmp(ItemHead-> str, STR_ENTRY)== 0){
		/*Do nothing in first Run*/

	}else if(strcmp(ItemHead-> str, STR_EXTERN)== 0){
		/*create symbol to extract and add to symbols list*/
		symbolsListNode* Symbol = (symbolsListNode*)malloc(sizeof(symbolsListNode));
		Symbol->Label = (char*)((CommandItemNode *)ItemHead->next)-> str;
		Symbol->Address = 0;
		Symbol->Action = 0;
		Symbol->Extract = 1;
		Symbol->next = NULL;
		AddSymbolToSymbolsList(firstRead, Symbol);

	}else if(strcmp(ItemHead-> str, STR_MOV)== 0){
		if(CheckIfWeHaveTwoOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfSourceAddressingOk(((CommandItemNode *)ItemHead->next)-> str, STR_MOV) == EXIT_FAILURE
				|| CheckIfDestenationAddressingOk( ((CommandItemNode *)((CommandItemNode *)ItemHead->next)->next)-> str , STR_MOV) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		int rows = 3;
		/*if both params are register*/
		if(IfBothParamRegister((CommandItemNode *)ItemHead->next,(CommandItemNode *)((CommandItemNode *)ItemHead->next)->next )==0){
			rows--;
		}
		updateIC(firstRead, Lable,rows);

	}else if(strcmp(ItemHead-> str, STR_CMP)== 0){
		if(CheckIfWeHaveTwoOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfSourceAddressingOk(((CommandItemNode *)ItemHead->next)-> str, STR_CMP) == EXIT_FAILURE
				|| CheckIfDestenationAddressingOk( ((CommandItemNode *)((CommandItemNode *)ItemHead->next)->next)-> str , STR_CMP) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		int rows = 3;
		/*if both params are register*/
		if(IfBothParamRegister((CommandItemNode *)ItemHead->next,(CommandItemNode *)((CommandItemNode *)ItemHead->next)->next )==0){
			rows--;
		}
		updateIC(firstRead, Lable,rows);

	}else if(strcmp(ItemHead-> str, STR_ADD)== 0){
		if(CheckIfWeHaveTwoOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfSourceAddressingOk(((CommandItemNode *)ItemHead->next)-> str, STR_ADD) == EXIT_FAILURE
				|| CheckIfDestenationAddressingOk( ((CommandItemNode *)((CommandItemNode *)ItemHead->next)->next)-> str , STR_ADD) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		int rows = 3;
		/*if both params are register*/
		if(IfBothParamRegister((CommandItemNode *)ItemHead->next,(CommandItemNode *)((CommandItemNode *)ItemHead->next)->next )==0){
			rows--;
		}
		updateIC(firstRead, Lable,rows);

	}else if(strcmp(ItemHead-> str, STR_SUB)== 0){
		if(CheckIfWeHaveTwoOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfSourceAddressingOk(((CommandItemNode *)ItemHead->next)-> str, STR_SUB) == EXIT_FAILURE
				|| CheckIfDestenationAddressingOk( ((CommandItemNode *)((CommandItemNode *)ItemHead->next)->next)-> str , STR_SUB) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		int rows = 3;
		/*if both params are register*/
		if(IfBothParamRegister((CommandItemNode *)ItemHead->next,(CommandItemNode *)((CommandItemNode *)ItemHead->next)->next )==0){
			rows--;
		}
		updateIC(firstRead, Lable,rows);

	}else if(strcmp(ItemHead-> str, STR_LEA)== 0){
		if(CheckIfWeHaveTwoOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfSourceAddressingOk(((CommandItemNode *)ItemHead->next)-> str, STR_LEA) == EXIT_FAILURE
				|| CheckIfDestenationAddressingOk( ((CommandItemNode *)((CommandItemNode *)ItemHead->next)->next)-> str , STR_LEA) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		int rows = 3;
		/*if both params are register*/
		if(IfBothParamRegister((CommandItemNode *)ItemHead->next,(CommandItemNode *)((CommandItemNode *)ItemHead->next)->next )==0){
			rows--;
		}
		updateIC(firstRead, Lable,rows);

	}else if(strcmp(ItemHead-> str, STR_NOT)== 0){
		if(CheckIfWeHaveOneOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfDestenationAddressingOk( ((CommandItemNode *)ItemHead->next)-> str , STR_NOT) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}

		updateIC(firstRead, Lable,2);

	}else if(strcmp(ItemHead-> str, STR_CLR)== 0){
		if(CheckIfWeHaveOneOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfDestenationAddressingOk( ((CommandItemNode *)ItemHead->next)-> str , STR_CLR) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		updateIC(firstRead, Lable,2);

	}else if(strcmp(ItemHead-> str, STR_INC)== 0){
		if(CheckIfWeHaveOneOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfDestenationAddressingOk( ((CommandItemNode *)ItemHead->next)-> str , STR_INC) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		updateIC(firstRead, Lable,2);

	}else if(strcmp(ItemHead-> str, STR_DEC)== 0){
		if(CheckIfWeHaveOneOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfDestenationAddressingOk( ((CommandItemNode *)ItemHead->next)-> str , STR_DEC) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		updateIC(firstRead, Lable,2);

	}else if(strcmp(ItemHead-> str, STR_JMP)== 0){
		if(CheckIfWeHaveOneOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfDestenationAddressingOk( ((CommandItemNode *)ItemHead->next)-> str , STR_JMP) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		updateIC(firstRead, Lable,2);

	}else if(strcmp(ItemHead-> str, STR_BNE)== 0){
		if(CheckIfWeHaveOneOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfDestenationAddressingOk( ((CommandItemNode *)ItemHead->next)-> str , STR_BNE) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		updateIC(firstRead, Lable,2);

	}else if(strcmp(ItemHead-> str, STR_RED)== 0){
		if(CheckIfWeHaveOneOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfDestenationAddressingOk( ((CommandItemNode *)ItemHead->next)-> str , STR_RED) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		updateIC(firstRead, Lable,2);

	}else if(strcmp(ItemHead-> str, STR_PRN)== 0){
		if(CheckIfWeHaveOneOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfDestenationAddressingOk( ((CommandItemNode *)ItemHead->next)-> str , STR_PRN) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		updateIC(firstRead, Lable,2);

	}else if(strcmp(ItemHead-> str, STR_JSR)== 0){
		if(CheckIfWeHaveOneOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfDestenationAddressingOk( ((CommandItemNode *)ItemHead->next)-> str , STR_JSR) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		updateIC(firstRead, Lable,2);

	}else if(strcmp(ItemHead-> str, STR_RTS)== 0){
		updateIC(firstRead, Lable,1);
	}else if(strcmp(ItemHead-> str, STR_STOP)== 0){
		updateIC(firstRead, Lable,1);
	}

	printf("[Info] IC: %d\n", firstRead ->IC);
	printf("[Info] DC: %d\n", firstRead ->DC);

	return EXIT_SUCCESS;
}

/*function check if the label is one of the register name*/
int IsRegisterString(char*str){
	if(strncmp(str, "R" ,1) == 0){
		switch(str[1]){
		case '0':
			return 0;
		case '1':
			return 0;
		case '2':
			return 0;
		case '3':
			return 0;
		case '4':
			return 0;
		case '5':
			return 0;
		case '6':
			return 0;
		case '7':
			return 0;
		}
	}
	return TRUE;
}

void AddSymbolToSymbolsList(firstReadStruct * firstRead, symbolsListNode *symbol){
	/*run on symbols link list and insert to the end of list the new symbol*/
	if(firstRead->symbolHead == NULL){
		firstRead->symbolHead = symbol;
		return;
	}
	symbolsListNode* tmp =(symbolsListNode*)firstRead->symbolHead;
	while(tmp->next !=NULL){
		tmp= (symbolsListNode*)tmp->next;
	}
	tmp->next = symbol;
}

void AddRowToDataList(firstReadStruct * firstRead, Data* data){
	/*add .data, .string to data list and update counter DC*/
	int DC = firstRead->DC;
	(firstRead->DataList)[DC] = data;
	firstRead->DC = DC +1;
}

Data* convertCharToUnsignedInt(char Char){
	/*cast and get the ascii code*/
	Data* instruction = (Data*)malloc(sizeof(Data));
	instruction->bytes = 0;
	instruction->bytes = (unsigned int)Char;
	return instruction;
}

Data* convertIntCharToUnsignedInt(char * Char){
	int intChar=0;
	int isNegative = 0;
	Data* instruction = (Data*)malloc(sizeof(Data));
	/*check if negative*/
	char * neg = "-";
	if(strncmp(Char,neg,1)==0 ){
		isNegative = 1;
		Char ++;
	}
	intChar = atoi(Char);
	/*fix number if negative*/
	if(isNegative == 1){
		intChar = ~intChar + 1;
	}
	instruction->bytes = intChar;
	return instruction;
}

int CheckIfSourceAddressingOk(char * str, char * action){
	/*check if the source address not correct*/
	if(strcmp(action, STR_LEA)== 0){
		ADDERSSING ad = ChooseAddressType(str);
		if(ad == IMMEDIATEADDERSSING || ad  == DIRECTREGISTERADDERSSING||
				ad == RANDOMADDERSSING2 || ad == RANDOMADDERSSING1){
			printf("[Error] wrong parameter source - %s : %s", action, str);
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

int CheckIfDestenationAddressingOk(char * str, char * action){
	/*check if the destination address not correct*/
	ADDERSSING ad = ChooseAddressType(str);
	if(strcmp(action, STR_MOV)== 0 ||
			strcmp(action, STR_ADD)== 0 ||
			strcmp(action, STR_SUB)== 0 ||
			strcmp(action, STR_NOT)== 0 ||
			strcmp(action, STR_CLR)== 0 ||
			strcmp(action, STR_LEA)== 0 ||
			strcmp(action, STR_INC)== 0 ||
			strcmp(action, STR_DEC)== 0||
			strcmp(action, STR_JMP)== 0 ||
			strcmp(action, STR_BNE)== 0 ||
			strcmp(action, STR_RED)== 0 ||
			strcmp(action, STR_JSR)== 0 ){
		if(ad == IMMEDIATEADDERSSING || ad  == RANDOMADDERSSING1||
				ad == RANDOMADDERSSING2 || ad == RANDOMADDERSSING3 ){
			printf("[Error] wrong parameter source - %s : %s", action, str);
			return EXIT_FAILURE;
		}
	}

	if(strcmp(action, STR_CMP)== 0 ||
			strcmp(action, STR_PRN)== 0 ){
		if(ad  == RANDOMADDERSSING1|| ad == RANDOMADDERSSING2 || ad == RANDOMADDERSSING3 ){
			printf("[Error] wrong parameter source - %s : %s", action, str);
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

int CheckIfWeHaveTwoOperators(CommandItemNode * ItemNode){
	/*check if we have 2 param*/
	if(ItemNode == NULL || ItemNode->next == NULL ){
		printf("[Error] Do not have two operators - %s", ItemNode-> str);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int IfBothParamRegister(CommandItemNode * ItemNodeS, CommandItemNode * ItemNodeD){
	if( IsRegisterString(ItemNodeS->str)==0 && IsRegisterString(ItemNodeD->str)==0 )
		return 0;
	return 1;
}

void updateIC(firstReadStruct * firstRead,  symbolsListNode *Lable, int rows){
	/*in first transition update the counter of transitions IC*/
	/*if we have label to the row we update that we in action line and update the address of action index*/
	if(Lable != NULL){
		Lable->Address = firstRead->IC;
		Lable->Action = 1;
	}
	firstRead->IC = firstRead->IC + rows;
}

int CheckIfWeHaveOneOperators(CommandItemNode * ItemNode){
	/*check if we get 1 param*/
	if(ItemNode == NULL){
		printf("[Error] Do not have one operators - %s", ItemNode-> str);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

ADDERSSING ChooseAddressType(char * str){
	if(strncmp(str, "#" ,1) == 0){
		return IMMEDIATEADDERSSING;
	}else if(strcmp(str, "***" ) == 0){
		return RANDOMADDERSSING3;
	}else if(strcmp(str, "**" ) == 0){
		return RANDOMADDERSSING2;
	}else if(strcmp(str, "*" ) == 0){
		return RANDOMADDERSSING1;
	}else if(IsRegisterString(str) == 0){
		return DIRECTREGISTERADDERSSING;
	}
	return DIRECTADDERSSING;
}
