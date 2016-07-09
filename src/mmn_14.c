/*
 ============================================================================
 Name        : main.c
 Author      : Slavik Pashanin
 Version     :
 Copyright   : slavik_pashanin
 Description :
 ============================================================================
 */

#include "assembler.h"
#include "fileManager.h"
#include "firstRead.h"


int main(int argc, char *argv[]) {


	{
		//char inputFile[FILENAME_MAX];
		int i;

		if (argc < 2)
		{
			printf("[Main][Error] No file was detected.\n");
			return 0;
		}

		for (i = 1; i < argc; i++)
		{
			/*[complete]- function detarmin name file*/
		//	fileName = argv[i];
		//	strcpy(inputFile,argv[i]);
		//	strcat(inputFile,".as");
			commandStringNode * CommandListHead;
			printf("[Main][Info]: Starting building linked list of %s file\n",argv[i]);
			CommandListHead = decodingFile("myprog.as");
			printLineLinkList(CommandListHead);
			printf("shalom");






			printf("[Main][Info]: Starting first read of %s file\n",argv[i]);
			firstRead(CommandListHead);
			printf("[Main][Info]: Starting second read of %s file\n",argv[i]);

		//	printf("[Main][Info]: Starting building files\n");

		//	printf("[Main][Info]: Running fineshed!\n");

		}
		return 0;
	}
}
