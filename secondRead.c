/*
 ============================================================================
 Name        : secondRead.c
 Author      : Slavik Pashanin
 Version     :
 Copyright   : Slavik_pashanin
 Description : [complete]
 ============================================================================
 */
#include "secondRead.h"
#include "fileManager.h"



secondReadStruct* ConvertFirstReadToSecondRead(firstReadStruct* firstRead){
	secondReadStruct* secondRead = (secondReadStruct*)malloc(sizeof(secondReadStruct));
	secondRead-> oldDC = firstRead->DC;
	secondRead-> oldIC = firstRead->IC;
	secondRead->IC = 100;

	int i ;
	int j  = 0;
	/*insert the data from DC list to IC list*/
	for(i = secondRead->oldIC; j < firstRead->DC ; i++,j++){
		secondRead->InstructionsList[i]= firstRead->DataList[j];
		unsigned int n = ((Data*)firstRead->DataList[j])->bytes;
		printf("[Info] value of data with new index: index = %d , value = ",i);
		while(n){
			if(n & 1)
				printf("1");
			else
				printf("0");
			n >>=1;
		}
		printf("\n");
	}

	/*run on symbol linked list and update address of .data or .string*/
	secondRead->SymbolesHead = firstRead->symbolHead;
	symbolsListNode* tmpsymbol =(symbolsListNode *)secondRead->SymbolesHead;
	while(tmpsymbol !=NULL){
		printf("[Info] before SymbolsListNode: Label = %s , address = %d, extract= %d, action= %d \n",tmpsymbol->Label,tmpsymbol->Address,tmpsymbol->Extract,tmpsymbol->Action);
		/*update the address*/
		if(tmpsymbol->Extract == 0 && tmpsymbol->Action == 0){
			tmpsymbol->Address =tmpsymbol->Address+ secondRead->oldIC;
		}
		printf("[Info] after  SymbolsListNode: Label = %s , address = %d, extract= %d, action= %d \n",tmpsymbol->Label,tmpsymbol->Address,tmpsymbol->Extract,tmpsymbol->Action);
		tmpsymbol = (symbolsListNode *)tmpsymbol->next;
	}
	return secondRead;
}

/*second transition function*/
int runSecondRead(commandStringNode* CommandListHead,secondReadStruct * secondRead){
	/*run on command list*/
	commandStringNode* CommandListNodeTmp = CommandListHead;
	while(CommandListNodeTmp != NULL){
		/*update the lists of transition*/
		UpdateSecondReadStruct(secondRead, (CommandItemNode *)CommandListNodeTmp->head);
		/*move to next item in list*/
		CommandListNodeTmp = (commandStringNode*)CommandListNodeTmp->next;
	}

	return EXIT_SUCCESS;
}

int UpdateSecondReadStruct(secondReadStruct * secondRead, CommandItemNode * ItemHead){
	/*comment Statements*/
	if(strncmp(ItemHead ->str, STRING_COMMENT ,1) == 0){
		return EXIT_SUCCESS;
	}
	/*label Statements*/
	if((ItemHead ->str)[strlen(ItemHead ->str)-1] ==  STRING_LABEL[0] ){
		/*run the action after label*/
		RunSecondReadAction(secondRead, (CommandItemNode *)ItemHead->next, NULL);
		return EXIT_SUCCESS;
	}
	/*run action*/
	RunSecondReadAction(secondRead, ItemHead, NULL);

	return EXIT_SUCCESS;
}


void RunSecondReadAction(secondReadStruct * secondRead, CommandItemNode * ItemHead, symbolsListNode *Lable){
	printf("[Info] action: %s\n", ItemHead ->str);
	if((strcmp(ItemHead->str, STR_STRING)) == 0){
		/*do nothing*/
	}else if(strcmp(ItemHead->str, STR_DATA)== 0){
		/*do nothing*/
	}else if(strcmp(ItemHead->str, STR_ENTRY)== 0){
		/*if entry*/
		int bool = 0;
		/*find the entry in symbols list*/
		char * itemStr = ((CommandItemNode *)ItemHead->next)->str;
		symbolsListNode * tmp = (symbolsListNode *)secondRead->SymbolesHead;
		while(tmp != NULL && bool ==0){
			if(((tmp->Label)[strlen(tmp->Label)-1] ==  STRING_LABEL[0] &&
					strncmp(tmp->Label, itemStr, strlen(tmp->Label)-1) ==0) || (strcmp(tmp->Label, itemStr) ==0)){
				bool = 1;
			}else{
				tmp= (symbolsListNode *)tmp->next;
			}
		}
		/*if exist in symbols list*/
		if(bool == 1){
			char* str = (char*)malloc(sizeof(char));
			myItoa(tmp->Address, str, 32);
			WritingLineToFile(ENTFILENAME , itemStr);
			WritingLineToFile(ENTFILENAME , " ");
			WritingLineToFile(ENTFILENAME , str);
			WritingLineToFile(ENTFILENAME , "\n");
		}
	}else if(strcmp(ItemHead->str, STR_EXTERN)== 0){
		/*Nothing second Run*/

	}else if(strcmp(ItemHead->str, STR_MOV)== 0){
		/*for every action do the same action*/
		/*get the source if we need*/
		/*get the destination if we need*/
		CommandItemNode * s = (CommandItemNode *)ItemHead->next;
		CommandItemNode * d = (CommandItemNode *)s->next;

		Data* datas = (Data*)malloc(sizeof(Data));
		datas->bytes = 0;

		Data* datad = (Data*)malloc(sizeof(Data));
		datad->bytes = 0;


		/*build the action for the action*/
		buildAction(secondRead,(CommandItemNode *)ItemHead->next, 2, ACTIONMOV,STR_MOV);

		/*convert source to bytes (Data contains bytes)*/
		ConvertParamToData('s',secondRead,datas, datad, s);
		/*add source Data (Bytes) to transition*/
		(secondRead, datas);

		/*convert destination to bytes (Data contains bytes)*/
		ConvertParamToData('d',secondRead,datad, datas, d);
		if(IfBothParamRegister((CommandItemNode *)ItemHead->next,(CommandItemNode *)((CommandItemNode *)ItemHead->next)->next )==0){
			datas->bytes = datas->bytes +  datad->bytes;
		}else{
			/*add destination Data (Bytes) to tranistion*/
			AddRowToTransitionList(secondRead, datad);
		}
	}else if(strcmp(ItemHead->str, STR_CMP)== 0){
		CommandItemNode * s = (CommandItemNode *)ItemHead->next;
		CommandItemNode * d = (CommandItemNode *)s->next;

		Data* datas = (Data*)malloc(sizeof(Data));
		datas->bytes = 0;

		Data* datad = (Data*)malloc(sizeof(Data));
		datad->bytes = 0;

		buildAction(secondRead,(CommandItemNode *)ItemHead->next, 2, ACTIONCMP,STR_CMP);

		ConvertParamToData('s',secondRead,datas, datad, s);
		AddRowToTransitionList(secondRead, datas);

		ConvertParamToData('d',secondRead,datad, datas, d);
		if(IfBothParamRegister((CommandItemNode *)ItemHead->next,(CommandItemNode *)((CommandItemNode *)ItemHead->next)->next )==0){
			datas->bytes = datas->bytes +  datad->bytes;
		}else{
			/*add destination Data (Bytes) to tranistion*/
			AddRowToTransitionList(secondRead, datad);
		}

	}else if(strcmp(ItemHead->str, STR_ADD)== 0){
		CommandItemNode * s = (CommandItemNode *)ItemHead->next;
		CommandItemNode * d = (CommandItemNode *)s->next;

		Data* datas = (Data*)malloc(sizeof(Data));
		datas->bytes = 0;

		Data* datad = (Data*)malloc(sizeof(Data));
		datad->bytes = 0;

		buildAction(secondRead,(CommandItemNode *)ItemHead->next, 2, ACTIONADD,STR_ADD);

		ConvertParamToData('s',secondRead,datas, datad, s);
		AddRowToTransitionList(secondRead, datas);

		ConvertParamToData('d',secondRead,datad, datas, d);
		if(IfBothParamRegister((CommandItemNode *)ItemHead->next,(CommandItemNode *)((CommandItemNode *)ItemHead->next)->next )==0){
			datas->bytes = datas->bytes +  datad->bytes;
		}else{
			/*add destination Data (Bytes) to tranistion*/
			AddRowToTransitionList(secondRead, datad);
		}

	}else if(strcmp(ItemHead->str, STR_SUB)== 0){
		CommandItemNode * s = (CommandItemNode *)ItemHead->next;
		CommandItemNode * d = (CommandItemNode *)s->next;

		Data* datas = (Data*)malloc(sizeof(Data));
		datas->bytes = 0;

		Data* datad = (Data*)malloc(sizeof(Data));
		datad->bytes = 0;

		buildAction(secondRead,(CommandItemNode *)ItemHead->next, 2, ACTIONSUB,STR_SUB);

		ConvertParamToData('s',secondRead,datas, datad, s);
		AddRowToTransitionList(secondRead, datas);

		ConvertParamToData('d',secondRead,datad, datas, d);
		if(IfBothParamRegister((CommandItemNode *)ItemHead->next,(CommandItemNode *)((CommandItemNode *)ItemHead->next)->next )==0){
			datas->bytes = datas->bytes +  datad->bytes;
		}else{
			/*add destination Data (Bytes) to tranistion*/
			AddRowToTransitionList(secondRead, datad);
		}

	}else if(strcmp(ItemHead->str, STR_LEA)== 0){
		CommandItemNode * s = (CommandItemNode *)ItemHead->next;
		CommandItemNode * d = (CommandItemNode *)s->next;

		Data* datas = (Data*)malloc(sizeof(Data));
		datas->bytes = 0;

		Data* datad = (Data*)malloc(sizeof(Data));
		datad->bytes = 0;

		buildAction(secondRead,(CommandItemNode *)ItemHead->next, 2, ACTIONLEA,STR_LEA);

		ConvertParamToData('s',secondRead,datas, datad, s);
		AddRowToTransitionList(secondRead, datas);

		ConvertParamToData('d',secondRead,datad, datas, d);
		if(IfBothParamRegister((CommandItemNode *)ItemHead->next,(CommandItemNode *)((CommandItemNode *)ItemHead->next)->next )==0){
			datas->bytes = datas->bytes +  datad->bytes;
		}else{
			/*add destination Data (Bytes) to tranistion*/
			(secondRead, datad);
		}

	}else if(strcmp(ItemHead->str, STR_NOT)== 0){

		CommandItemNode * d = (CommandItemNode *)ItemHead->next;

		Data* datad = (Data*)malloc(sizeof(Data));
		datad->bytes = 0;

		buildAction(secondRead,(CommandItemNode *)ItemHead->next, 1, ACTIONNOT,STR_NOT);

		ConvertParamToData('d',secondRead,datad, NULL, d);
		AddRowToTransitionList(secondRead, datad);

	}else if(strcmp(ItemHead->str, STR_CLR)== 0){

		CommandItemNode * d = (CommandItemNode *)ItemHead->next;

		Data* datad = (Data*)malloc(sizeof(Data));
		datad->bytes = 0;

		buildAction(secondRead,(CommandItemNode *)ItemHead->next, 1, ACTIONCLR,STR_CLR);

		ConvertParamToData('d',secondRead,datad, NULL, d);
		AddRowToTransitionList(secondRead, datad);

	}else if(strcmp(ItemHead->str, STR_INC)== 0){

		CommandItemNode * d = (CommandItemNode *)ItemHead->next;

		Data* datad = (Data*)malloc(sizeof(Data));
		datad->bytes = 0;

		buildAction(secondRead,(CommandItemNode *)ItemHead->next, 1, ACTIONINC,STR_INC);

		ConvertParamToData('d',secondRead,datad, NULL, d);
		AddRowToTransitionList(secondRead, datad);

	}else if(strcmp(ItemHead->str, STR_DEC)== 0){

		CommandItemNode * d = (CommandItemNode *)ItemHead->next;

		Data* datad = (Data*)malloc(sizeof(Data));
		datad->bytes = 0;

		buildAction(secondRead,(CommandItemNode *)ItemHead->next, 1, ACTIONDEC,STR_DEC);

		ConvertParamToData('d',secondRead,datad, NULL, d);
		AddRowToTransitionList(secondRead, datad);

	}else if(strcmp(ItemHead->str, STR_JMP)== 0){

		CommandItemNode * d = (CommandItemNode *)ItemHead->next;

		Data* datad = (Data*)malloc(sizeof(Data));
		datad->bytes = 0;

		buildAction(secondRead,(CommandItemNode *)ItemHead->next, 1, ACTIONJMP,STR_JMP);

		ConvertParamToData('d',secondRead,datad, NULL, d);
		(secondRead, datad);

	}else if(strcmp(ItemHead->str, STR_BNE)== 0){

		CommandItemNode * d = (CommandItemNode *)ItemHead->next;

		Data* datad = (Data*)malloc(sizeof(Data));
		datad->bytes = 0;

		buildAction(secondRead,(CommandItemNode *)ItemHead->next, 1, ACTIONBNE,STR_BNE);

		ConvertParamToData('d',secondRead,datad, NULL, d);
		AddRowToTransitionList(secondRead, datad);

	}else if(strcmp(ItemHead->str, STR_RED)== 0){

		CommandItemNode * d = (CommandItemNode *)ItemHead->next;

		Data* datad = (Data*)malloc(sizeof(Data));
		datad->bytes = 0;

		buildAction(secondRead,(CommandItemNode *)ItemHead->next, 1, ACTIONRED,STR_RED);

		ConvertParamToData('d',secondRead,datad, NULL, d);
		AddRowToTransitionList(secondRead, datad);

	}else if(strcmp(ItemHead->str, STR_PRN)== 0){

		CommandItemNode * d = (CommandItemNode *)ItemHead->next;

		Data* datad = (Data*)malloc(sizeof(Data));
		datad->bytes = 0;

		buildAction(secondRead,(CommandItemNode *)ItemHead->next, 1, ACTIONPRN,STR_PRN);

		ConvertParamToData('d',secondRead,datad, NULL, d);
		AddRowToTransitionList(secondRead, datad);

	}else if(strcmp(ItemHead->str, STR_JSR)== 0){

		CommandItemNode * d = (CommandItemNode *)ItemHead->next;

		Data* datad = (Data*)malloc(sizeof(Data));
		datad->bytes = 0;

		buildAction(secondRead,(CommandItemNode *)ItemHead->next, 1, ACTIONJSR,STR_JSR);

		ConvertParamToData('d',secondRead,datad, NULL, d);
		AddRowToTransitionList(secondRead, datad);

	}else if(strcmp(ItemHead->str, STR_RTS)== 0){

		buildAction(secondRead,(CommandItemNode *)ItemHead->next, 0, ACTIONRTS,STR_RTS);

	}else if(strcmp(ItemHead->str, STR_STOP)== 0){
		buildAction(secondRead,(CommandItemNode *)ItemHead->next, 0, ACTIONSTOP,STR_STOP);
	}


}


char* myItoa(int num, char* str, int base)
{
	int i = 0;
	int isNegative = 0;

	/* Handle 0 explicitely, otherwise empty string is printed for 0 */
	if (num == 0)
	{
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	/* In standard itoa(), negative numbers are handled only with*/
	/*base 10. Otherwise numbers are considered unsigned.*/
	if (num < 0 && base == 10)
	{
		isNegative = 1;
		num = -num;
	}

	/* Process individual digits*/
	while (num != 0)
	{
		int rem = num % base;
		str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
		num = num/base;
	}

	/* If number is negative, append '-'*/
	if (isNegative)
		str[i++] = '-';

	str[i] = '\0'; /* Append string terminator*/

	/*Reverse the string*/
	reverse(str, i);

	return str;
}

void WritingLineToFile(char* fileName,char* text){
	FILE *fp;

	fp= fopen(fileName, "r+");
	if(fp == NULL)
		return;

	fseek(fp, 0,SEEK_END);
	fputs(text, fp);



	fclose(fp);
}

void buildAction(secondReadStruct * secondRead, CommandItemNode * ItemNode,unsigned int paramNum, unsigned int actionNum,char* strAction){
	unsigned int rnd = 0;
	unsigned int group = paramNum;
	unsigned int opcode = actionNum;
	unsigned int source = 0;
	unsigned int destenation = 0;
	unsigned int ERA=0;
	/*check if we in action with 2 params*/
	if(paramNum == 2){
		CommandItemNode * s = ItemNode;
		CommandItemNode * d = s->next;
		/* get the address type for rnd code*/
		ADDERSSING sAddress = ChooseAddressType(s->str);
		ADDERSSING dAddress = ChooseAddressType(d->str);

		if(sAddress == RANDOMADDERSSING1)
			rnd = 1;
		if(sAddress == RANDOMADDERSSING2)
			rnd = 2;
		if(sAddress == RANDOMADDERSSING3)
			rnd = 3;
		/*get address code for source and destination*/
		source = GetNumberByAddressing(sAddress);
		destenation = GetNumberByAddressing(dAddress);
		/*add row to transition list*/
		AddRowToTransitionList(secondRead, BuiledDataForAction(rnd, group, opcode, source, destenation, ERA));

		/*check if we in action with 1 params*/
	}else if(paramNum == 1){
		CommandItemNode * d = ItemNode;
		/* get the address type for rnd code*/
		ADDERSSING dAddress = ChooseAddressType(d->str);
		/* update rnd code*/
		if(dAddress == RANDOMADDERSSING1)
			rnd = 1;
		if(dAddress == RANDOMADDERSSING2)
			rnd = 2;
		if(dAddress == RANDOMADDERSSING3)
			rnd = 3;
		/*get address code for  destination*/
		destenation = GetNumberByAddressing(dAddress);
		/*add row to transition list*/
		AddRowToTransitionList(secondRead, BuiledDataForAction(rnd, group, opcode, source, destenation, ERA));
	}else{
		/*no param for action*/
		/*add row to transition list*/
		AddRowToTransitionList(secondRead, BuiledDataForAction(rnd, group, opcode, source, destenation, ERA));
	}
}


void ConvertParamToData(char SourceOrDest ,secondReadStruct * secondRead ,Data * data , Data * otherParam ,CommandItemNode * item){
	char * param = item->str;

	ADDERSSING add = ChooseAddressType(param);
	if(add == RANDOMADDERSSING3){
		/*if we get ****/
		data->bytes = 0;
		symbolsListNode * symbol = (symbolsListNode *)secondRead->SymbolesHead;
		int bool = 0;
		/*run on symbols list and get the first symbol .data or .string*/
		while(symbol != NULL && bool == 0)
		{
			if(symbol->Action == 0 && symbol->Extract == 0){
				bool = 1;
				/*update data to the address of symbol*/
				data->bytes = symbol->Address;
			}else{
				symbol = (symbolsListNode *)symbol->next;
			}
		}
		/*if we get ***/
	}else if(add == RANDOMADDERSSING2){
		/*get random number between 0 to 9*/
		data->bytes = rand()%10;
		/*if we get **/
	}else if(add == RANDOMADDERSSING1){
		/*get random register*/
		data->bytes = getRandomRegister();
		/*if destination param shift 2 bit*/
		if(SourceOrDest == 'd'){
			data->bytes <<= 2;
			/*if source param shift 8 bit*/
		}else if(SourceOrDest == 's'){
			data->bytes <<= 8;
		}

		/*if we get register (R1 - R2 )param (3)*/
	}else if(add == DIRECTREGISTERADDERSSING){
		param++; /* num after R*/
		Data* dataTmp = convertIntCharToUnsignedInt(param);
		/*if destination param shift 2 bit*/
		if(SourceOrDest == 'd'){
			dataTmp->bytes <<= 2;
			/*if source param shift 8 bit*/
		}else if(SourceOrDest == 's'){
			dataTmp->bytes <<= 8;
		}
		data->bytes = dataTmp->bytes;
		/*if we get variable param*/
	}else if(add == DIRECTADDERSSING){
		/*find variable in symbols link list*/
		symbolsListNode * symbol = (symbolsListNode *)secondRead->SymbolesHead;
		int bool = 0;
		while(symbol != NULL && bool == 0)
		{
			if(((symbol->Label)[strlen(symbol->Label)-1] ==  STRING_LABEL[0] &&
					strncmp(symbol->Label, param, strlen(symbol->Label)-1) ==0) || (strcmp(symbol->Label, param) ==0)){
				bool = 1;
			}else{
				symbol = (symbolsListNode *)symbol->next;
			}
		}

		if(symbol != NULL){
			/*if extract print to file the lable and the variable param*/
			if(symbol->Extract == 1 ){
				WritingLineToFile(EXTFILENAME , param);
				WritingLineToFile(EXTFILENAME , " ");
				char * strIndex = (char*)malloc(sizeof(char));
				/*secondRead->IC*/
				myItoa(secondRead->IC,strIndex,32);
				WritingLineToFile(EXTFILENAME , strIndex);
				WritingLineToFile(EXTFILENAME , "\n");
				data->bytes = data->bytes + 1; /* add ARE extract 01*/
			}else if(symbol->Action == 0){
				/*if parameter label return the data from instruction list*/
				data->bytes = symbol->Address;
				/*insert ERA - E*/
				data->bytes<<=2;
				data->bytes = data->bytes + 2;
			}
		}
		/*if we get # param*/
	}else if(add == IMMEDIATEADDERSSING){
		param++;
		/*convert param to number*/
		if(getParameterType(param) == NUMBER){
			Data* dataTmp = convertIntCharToUnsignedInt(param);
			/*insert ERA - A*/
			dataTmp->bytes<<=2;
			data->bytes =dataTmp->bytes;
		}
	}
}
void AddRowToTransitionList(secondReadStruct * secondTranstion, Data* data){
	secondTranstion->InstructionsList[secondTranstion->IC] = data;
	secondTranstion->IC = secondTranstion->IC +1;
	unsigned int n = data->bytes;
	while(n){
		if(n & 1)
			printf("1");
		else
			printf("0");
		n >>=1;
	}
	printf("\n");

}

void reverse(char* str, int length)
{
	/*reverse string*/
	int start = 0;
	int end = length -1;
	/*run strat to end*/
	while (start < end)
	{
		/*replace string*/
		swap(&(*(str+start)), &(*(str+end)));
		start++;
		end--;
	}
}

unsigned int GetNumberByAddressing(ADDERSSING Address){
	/*get the address code*/
	if(Address == IMMEDIATEADDERSSING)
		return 0;
	if(Address == DIRECTADDERSSING)
		return 1;
	if(Address == RANDOMADDERSSING1 || Address == RANDOMADDERSSING2 || Address == RANDOMADDERSSING3)
		return 2;
	if(Address == DIRECTREGISTERADDERSSING)
		return 3;

	return 0;
}

Data* BuiledDataForAction(unsigned int rnd , unsigned int group, unsigned int opcode,unsigned int sourceAddress,unsigned int destAddress,unsigned int ERA){
	Data* data = (Data*)malloc(sizeof(Data));
	data->bytes = 0;
	rnd <<=12;
	group <<=10;
	opcode <<=6;
	sourceAddress <<=4;
	destAddress <<=2;
	data->bytes = data->bytes + rnd + group+ opcode+ sourceAddress+destAddress;
	return data;
}

int getRandomRegister(){
	/*get number between 0 - 7*/
	int rnd =rand()%10;
	if(rnd > 7)
		rnd = 7;
	if(rnd < 0)
		rnd = 0;

	return rnd;
}

PARAMETERTYPE getParameterType(char*param){
	/*get parameter type*/
	/*if is register*/
	if(IsRegisterString(param)==0 )
		return REGISTER;
	/*if number*/
	if( isdigit((char)param[0]) || strncmp(param,"-",1)==0)
		return NUMBER;

	/*Default label*/
	return LABLE;
}

void swap(char *i, char *j) {
	/*switch char **/
	char t = *i;
	*i = *j;
	*j = t;
}
