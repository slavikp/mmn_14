#include "firstRead.h"
#include "assembler.h"


int firstRead(commandStringNode* CommandListHead){
	/*Create the Struct for the firstRead and give default parameters*/
	firstReadStruct * firstRun = (firstReadStruct *)malloc(sizeof(firstReadStruct));
	firstRun-> DC = 0;
	firstRun-> IC = FIRST_ADDRESS;
	firstRun-> symbolHead = NULL;

	/*Starting the first read on lines*/

	commandStringNode* CommandListNodeTemp = CommandListHead;
	while(CommandListNodeTemp != NULL) {
		/*update the sturct with symbols, data , transitions and counters*/
		int result = updateFirstReadStruct(firstRun, (CommandItemNode *)CommandListNodeTemp-> head);
		/*if the command line was with error remove line*/
		if(result == EXIT_FAILURE) {
			/*remove command line form list - we don't want to run on this line in second transition*/
			commandStringNode * failNext = (commandStringNode *)CommandListNodeTemp-> next;
			CommandListNodeTemp = (commandStringNode *)CommandListNodeTemp-> prev;
			if(CommandListNodeTemp != NULL){
				CommandListNodeTemp->next = failNext;
				failNext->prev = CommandListNodeTemp;
				/*move to next line*/
				CommandListNodeTemp = (commandStringNode*)CommandListNodeTemp->next;
			}
			else {
				/*if we delete from root list*/
				CommandListHead-> head = failNext-> head;
				CommandListHead-> next = failNext-> next;
				CommandListNodeTemp = failNext;
			}
		}
		else {
			/*move to next line*/
			CommandListNodeTemp = (commandStringNode*)CommandListNodeTemp-> next;
		}
	}
	return EXIT_SUCCESS;
}

int updateFirstReadStruct(firstReadStruct * firstRead, CommandItemNode * ItemHead){
	/*comment Statements*/
	if(strncmp(ItemHead -> str, STRING_COMMENT ,1) == 0){
		printf("[First-Read][Info] comment %s\n", ItemHead -> str);
		return EXIT_FAILURE;
	}
	/*label Statements*/
	if((ItemHead -> str)[strlen(ItemHead -> str)-1] == STRING_LABEL[0]) {
		printf("[First-Read][Info] Add Label %s\n", ItemHead -> str);
		/*if label length bigger then 30 char*/
		if(strlen(ItemHead -> str) > MAX_LABEL_LENGTH ){
			printf("[First-Read][Error]label length is bigger than 30 chars.\n");
			return EXIT_FAILURE;
		}
		/*if is register label*/
		if(IsRegisterString(ItemHead -> str) == 0) {
			printf("[First-Read][Error] label have register name\n");
			return EXIT_FAILURE;
		}
		/*create symbol node for symbols list and give default parameters*/
		symbolsListNode *Lable = (symbolsListNode *)malloc(sizeof(symbolsListNode));
		Lable-> Label = ItemHead -> str;
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
		else {
			return EXIT_FAILURE;
		}
	}
	/*Action Statements*/
	return RunFirstReadAction(firstRead, ItemHead, NULL);
}


int RunFirstReadAction(firstReadStruct * firstRead, CommandItemNode * ItemHead, symbolsListNode *Lable){
	printf("[First-Read][Info] action: %s\n", ItemHead -> str);

	/*run an all action - check for error in actions and if not error update the IC counter,*/
	/*in data, string, extern, entry insert if we need to lists (if we need)*/

	if((strcmp(ItemHead-> str, STRSTRING)) == 0) {
		if(ItemHead-> next == NULL)
		{
			printf("[First-Read][Error] %s not getting string\n",ItemHead-> str);
			return EXIT_FAILURE;
		}
		/*if we need to update symbols list*/
		if(Lable != NULL)
			Lable-> Address = firstRead-> DC;
		/* get string to save*/
		char * String = ((CommandItemNode *)ItemHead-> next)-> str;
		size_t StringLen = strlen(String);
		int i = 1;
		/*add data rows to struct*/
		while(i < StringLen-1) {
			AddRowToDataList(firstRead, convertCharToUnsignedInt(String[i]));
			i++;
		}
		/*update the data list*/
		Data* endStr = (Data*)malloc(sizeof(Data));
		endStr-> bytes = 0;
		AddRowToDataList(firstRead, endStr);
		return EXIT_SUCCESS;

	}else if(strcmp(ItemHead-> str, STRDATA)== 0){
		/*get next data int*/
		CommandItemNode *tmp =  (CommandItemNode *)ItemHead-> next;
		/*if we have label before action and if we have one int or more*/
		if(Lable != NULL && tmp !=NULL)
			Lable-> Address = firstRead-> DC;
		/*add all ints to data list*/
		while(tmp != NULL){
			AddRowToDataList(firstRead, convertIntCharToUnsignedInt(tmp-> str));
			tmp=(CommandItemNode *)tmp->next;
		}
	}else if(strcmp(ItemHead-> str, STRENTRY)== 0){
		/*Nothing first Run*/

	}else if(strcmp(ItemHead-> str, STREXTERN)== 0){
		/*create symbol to extract and add to symbols list*/
		symbolsListNode* Symbol = (symbolsListNode*)malloc(sizeof(symbolsListNode));
		Symbol->Label = (char*)((CommandItemNode *)ItemHead->next)-> str;
		Symbol->Address = 0;
		Symbol->Action = 0;
		Symbol->Extract = 1;
		Symbol->next = NULL;
		AddSymbolToSymbolsList(firstRead, Symbol);

	}else if(strcmp(ItemHead-> str, STRMOV)== 0){
		if(CheckIfWeHaveTwoOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfSourceAddressingOk(((CommandItemNode *)ItemHead->next)-> str, STRMOV) == EXIT_FAILURE
				|| CheckIfDestenationAddressingOk( ((CommandItemNode *)((CommandItemNode *)ItemHead->next)->next)-> str , STRMOV) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		int rows = 3;
		/*if both params are register*/
		if(IfBothParamRegister((CommandItemNode *)ItemHead->next,(CommandItemNode *)((CommandItemNode *)ItemHead->next)->next )==0){
			rows--;
		}
		updateICandLabelFromAction(firstRead, Lable,rows);

	}else if(strcmp(ItemHead-> str, STRCMP)== 0){
		if(CheckIfWeHaveTwoOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfSourceAddressingOk(((CommandItemNode *)ItemHead->next)-> str, STRCMP) == EXIT_FAILURE
				|| CheckIfDestenationAddressingOk( ((CommandItemNode *)((CommandItemNode *)ItemHead->next)->next)-> str , STRCMP) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		int rows = 3;
		/*if both params are register*/
		if(IfBothParamRegister((CommandItemNode *)ItemHead->next,(CommandItemNode *)((CommandItemNode *)ItemHead->next)->next )==0){
			rows--;
		}
		updateICandLabelFromAction(firstRead, Lable,rows);

	}else if(strcmp(ItemHead-> str, STRADD)== 0){
		if(CheckIfWeHaveTwoOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfSourceAddressingOk(((CommandItemNode *)ItemHead->next)-> str, STRADD) == EXIT_FAILURE
				|| CheckIfDestenationAddressingOk( ((CommandItemNode *)((CommandItemNode *)ItemHead->next)->next)-> str , STRADD) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		int rows = 3;
		/*if both params are register*/
		if(IfBothParamRegister((CommandItemNode *)ItemHead->next,(CommandItemNode *)((CommandItemNode *)ItemHead->next)->next )==0){
			rows--;
		}
		updateICandLabelFromAction(firstRead, Lable,rows);

	}else if(strcmp(ItemHead-> str, STRSUB)== 0){
		if(CheckIfWeHaveTwoOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfSourceAddressingOk(((CommandItemNode *)ItemHead->next)-> str, STRSUB) == EXIT_FAILURE
				|| CheckIfDestenationAddressingOk( ((CommandItemNode *)((CommandItemNode *)ItemHead->next)->next)-> str , STRSUB) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		int rows = 3;
		/*if both params are register*/
		if(IfBothParamRegister((CommandItemNode *)ItemHead->next,(CommandItemNode *)((CommandItemNode *)ItemHead->next)->next )==0){
			rows--;
		}
		updateICandLabelFromAction(firstRead, Lable,rows);

	}else if(strcmp(ItemHead-> str, STRLEA)== 0){
		if(CheckIfWeHaveTwoOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfSourceAddressingOk(((CommandItemNode *)ItemHead->next)-> str, STRLEA) == EXIT_FAILURE
				|| CheckIfDestenationAddressingOk( ((CommandItemNode *)((CommandItemNode *)ItemHead->next)->next)-> str , STRLEA) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		int rows = 3;
		/*if both params are register*/
		if(IfBothParamRegister((CommandItemNode *)ItemHead->next,(CommandItemNode *)((CommandItemNode *)ItemHead->next)->next )==0){
			rows--;
		}
		updateICandLabelFromAction(firstRead, Lable,rows);

	}else if(strcmp(ItemHead-> str, STRNOT)== 0){
		if(CheckIfWeHaveOneOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfDestenationAddressingOk( ((CommandItemNode *)ItemHead->next)-> str , STRNOT) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}

		updateICandLabelFromAction(firstRead, Lable,2);

	}else if(strcmp(ItemHead-> str, STRCLR)== 0){
		if(CheckIfWeHaveOneOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfDestenationAddressingOk( ((CommandItemNode *)ItemHead->next)-> str , STRCLR) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		updateICandLabelFromAction(firstRead, Lable,2);

	}else if(strcmp(ItemHead-> str, STRINC)== 0){
		if(CheckIfWeHaveOneOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfDestenationAddressingOk( ((CommandItemNode *)ItemHead->next)-> str , STRINC) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		updateICandLabelFromAction(firstRead, Lable,2);

	}else if(strcmp(ItemHead-> str, STRDEC)== 0){
		if(CheckIfWeHaveOneOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfDestenationAddressingOk( ((CommandItemNode *)ItemHead->next)-> str , STRDEC) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		updateICandLabelFromAction(firstRead, Lable,2);

	}else if(strcmp(ItemHead-> str, STRJMP)== 0){
		if(CheckIfWeHaveOneOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfDestenationAddressingOk( ((CommandItemNode *)ItemHead->next)-> str , STRJMP) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		updateICandLabelFromAction(firstRead, Lable,2);

	}else if(strcmp(ItemHead-> str, STRBNE)== 0){
		if(CheckIfWeHaveOneOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfDestenationAddressingOk( ((CommandItemNode *)ItemHead->next)-> str , STRBNE) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		updateICandLabelFromAction(firstRead, Lable,2);

	}else if(strcmp(ItemHead-> str, STRRED)== 0){
		if(CheckIfWeHaveOneOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfDestenationAddressingOk( ((CommandItemNode *)ItemHead->next)-> str , STRRED) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		updateICandLabelFromAction(firstRead, Lable,2);

	}else if(strcmp(ItemHead-> str, STRPRN)== 0){
		if(CheckIfWeHaveOneOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfDestenationAddressingOk( ((CommandItemNode *)ItemHead->next)-> str , STRPRN) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		updateICandLabelFromAction(firstRead, Lable,2);

	}else if(strcmp(ItemHead-> str, STRJSR)== 0){
		if(CheckIfWeHaveOneOperators((CommandItemNode *)ItemHead->next) == EXIT_FAILURE ||
				CheckIfDestenationAddressingOk( ((CommandItemNode *)ItemHead->next)-> str , STRJSR) == EXIT_FAILURE){
			return EXIT_FAILURE;
		}
		updateICandLabelFromAction(firstRead, Lable,2);

	}else if(strcmp(ItemHead-> str, STRRTS)== 0){
		updateICandLabelFromAction(firstRead, Lable,1);
	}else if(strcmp(ItemHead-> str, STRSTOP)== 0){
		updateICandLabelFromAction(firstRead, Lable,1);
	}

	printf("[First-Read][Info] IC parameter is: %d\n", firstRead ->IC);
	printf("[First-Read][Info] DC parameter is: %d\n",firstRead -> DC);

	return EXIT_SUCCESS;
}


void AddSymbolToSymbolsList(firstReadStruct * firstRead, symbolsListNode *symbol){
	/*run on symbols link list and insert to the end of list the new symbol*/
	if(firstRead -> symbolHead == NULL){
		firstRead-> symbolHead = symbol;
		return;
	}
	symbolsListNode* tmp =(symbolsListNode*)firstRead-> symbolHead;
	while(tmp->next !=NULL){
		tmp= (symbolsListNode*)tmp-> next;
	}
	tmp->next = symbol;
}

void AddRowToDataList(firstReadStruct * firstTranstion, Data* data){
	/*add .data, .string to data list and update counter DC*/
	int DC = firstTranstion-> DC;
	(firstTranstion-> DataList)[DC] = data;
	firstTranstion-> DC = DC +1;
}

int CheckIfWeHaveTwoOperators(CommandItemNode * ItemNode) {
	/*check if we have 2 param*/
	if(ItemNode == NULL || ItemNode->next == NULL ){
		printf("error not get 2 parameters - %s", ItemNode-> str);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int CheckIfWeHaveOneOperators(CommandItemNode * ItemNode){
	/*check if we get 1 param*/
	if(ItemNode == NULL){
		printf("error not get 1 parameters - %s", ItemNode->str);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int isRegisterString(char*str) {
	if(strncmp(str, "R" ,1) == 0){
		switch(str[1]){
		case '0':
			return EXIT_SUCSSES;
		case '1':
			return EXIT_SUCSSES;
		case '2':
			return EXIT_SUCSSES;
		case '3':
			return EXIT_SUCSSES;
		case '4':
			return EXIT_SUCSSES;
		case '5':
			return EXIT_SUCSSES;
		case '6':
			return EXIT_SUCSSES;
		case '7':
			return EXIT_SUCSSES;
		}
	}
	return EXIT_FAILURE;
}
Data* convertCharToUnsignedInt(char Char){
	/*cast and get the Ascii table code*/
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
	if(strncmp(Char,neg,1)==0 ) {
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


