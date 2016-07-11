/*
 ============================================================================
 Name        : main.c
 Author      : Slavik Pashanin
 Version     :
 Copyright   : Slavik_pashanin
 Description : The purpose of this program is to create an assembler program
 	 	 	   The program getting *.as file "translte it to linked list
 	 	 	   by first and second read of file.
 ============================================================================
 */

#include "assembler.h"
#include "fileManager.h"
#include "firstRead.h"
#include "secondRead.h"



int main(int argc, char *argv[]) {

		//char inputFile[FILENAME_MAX];
		int i;

		if (argc < 2)
		{
			printf("[Error] No file was detected.\n");
			return FALSE;
		}

		for (i = 1; i < argc; i++)
		{
			/*[complete]- function detarmin name file*/
		//	fileName = argv[i];
		//	strcpy(inputFile,argv[i]);
		//	strcat(inputFile,".as");
			commandStringNode * CommandListHead;
			printf("[Info]: starting run project of %s file\n",argv[i]);
			CommandListHead = decodingFile("myprog.as");
			printLineLinkList(CommandListHead);

			printf("[Info] Starting first read of %s file\n",argv[i]);
			/*create the struct for the first transition and give defaults*/
			firstReadStruct * firstRead = (firstReadStruct *)malloc(sizeof(firstReadStruct));
			firstRead->DC = 0;
			firstRead->IC = 100;
			firstRead->symbolHead = NULL;
			runFirstRead(CommandListHead,firstRead);

			printf("[Info] Starting second read of %s file\n",argv[i]);
			secondReadStruct* secondRead = ConvertFirstReadToSecondRead(firstRead);
			free(firstRead);

		//	printf("[Info]: Starting building files\n");

		//	printf("[Info]: Running fineshed!\n");

		}
		return 0;
}
