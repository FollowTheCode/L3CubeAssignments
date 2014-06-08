/* 
	=====================================================================================
	TITLE  -  SIMPLE VERSION CONTROL 
	-------------------------------------------------------------------------------------
	AUTHOR -  FOLLOW THE CODE
	-------------------------------------------------------------------------------------
	DATE   -  7th June, 2014
	-------------------------------------------------------------------------------------
	BRIEF DESCRIPTION - 
	-------------------
	Simple Version control is an easy to use version control program which keeps track 
	of all the previous versions (consecutive duplicate versions are not added) of a 
	text file and allows the user to revert back to any version.

	NOTE : EVERY VERSION IN THE VERSION FILE DOES NOT CONTAIN THE ENTIRE TEXT, INSTEAD 
	IT ONLY CONTAINS THE INFORMATION ABOUT HOW IT IS DIFFERRENT FROM THE PREVIOUS VERSION
	======================================================================================
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define LINES 20
#define LINE_SIZE 10

/* FUNCTION DECLARATION */
int addToVersionFile();
int addContents();
int checkIfNew();
void loadVersion(char [], char [][LINE_SIZE], int);
int getOldVersion(char [], int);
int check(char [], char [][LINE_SIZE], int);
void help();

/* VERSION - FILE CONTAINING INFORMATION ABOUT ALL VERSIONS */
/* CURRENT - CURRENT TEXT FILE */
FILE *version, *current;

/* MAIN FUNCTION */
int main(int argc, char *argv[])
{
	int i=0, new = 0;
	char filename[20], versionFile[25];
	if(argc < 3)
	{
		help();
		return 0;
	}
	/* ACCEPT FILE NAME AND COMMAND VIA COMMAND LINE ARGUMENTS */
	strcpy(filename, argv[2]);

	/* VERSION FILENAME FOR A FILE 'ABC.txt' will be '.VersionABC.txt' */
	strcpy(versionFile, ".Version");
	strcat(versionFile, filename);
	
	/* OPEN FILE IN READ MODE AND VERSION FILE IN APPEND+READ MODE */
	/* EXIT IF EITHER OF THE ABOVE OPERATIONS FAIL */
	current = fopen(filename, "r");		
	if(current == NULL)
	{
		printf("\nFile Could not be opened.");
		help();
		return 0;
	}
	version = fopen(versionFile, "a+");
	new = checkIfNew();
	if(version == NULL)
	{
		printf("\nFile Could not be opened/created.");
		help();
		return 0;
	}

	/* IF THIRD ARGUMENT IS 'COMMIT', call AddToVersionFile() */
	if(argc == 3 && !strcmp(argv[1], "commit"))
	{
		/* FOR FIRST COMMIT MULT-LINE TEXT IS PERMITTED */
		if(new == 1)
			addContents();
		else
			addToVersionFile();
	}
	/* IF THIRD ARGUMENT IS 'VERSION', call getOldVersion() */
	else if(argc == 4 && !strcmp(argv[1], "version") )
		getOldVersion(argv[2], atoi(argv[3]));
	
	/* IF INVALID ARGUMENTS, LIST VALID ARGUMENTS AND USAGE */
	else
		help();
	
	/* CLOSE BOTH THE FILE POINTERS */
	fclose(current);
	fclose(version);
	return 0;
}

/************************************************ ADD VERSION *******************************************************/

/* FUNCTION TO CREATE A NEW VERSION AND ADD THE UPDATE DETAILS OF THE CURRENT FILE TO THE NEW VERSION. (COMMIT) */
/* UPDATE DETAILS MAY INCLUDE EITHER
   
   - <n>    : delete nth line from the previous version 
   
   OR
   
   <string> : append this string at the end of the previous version
 */
int addToVersionFile()
{
	/* buffer     - STORES THE TEXT OF THE LAST VERSION 
	   append     - UPDATE INFORMATION WHICH IS TO BE WRITTEN UNDER THE NEW VERSION 
	   version    - COUNTS ALL VERSIONS
	   line_count - NO OF LINES IN THE PREVIOUS VERSION
	   d          - LINE INDEX WHICH IS TO BE DELETED FROM THE BUFFER
	   issues     - VALUE RETURNED BY check() 
					-1 : INVALID FILE OPERATION
					 0 : ALL OKAY
					 1 : SAME AS THE PREVIOUS VERSION
	*/
	char ch, str[LINE_SIZE+5], append[LINE_SIZE], buffer[LINES][LINE_SIZE];	
	int versions=-1, d, k, line_count = 0, i=0, issues;

	/* COUNT THE PREVIOUS VERSIONS */
	/* EVERY VERSION STARTS WITH "#Version <version no.>" */
	/* UPDATE THE BUFFER AT EVERY VERSION */
	/* ONLY ONE OPERATION PERMITTED AT EVERY STAGE */
	rewind(version);
	while(fgets(str, (LINE_SIZE+5), version))
	{
		if(str[(strlen(str)-1)] == '\n')
			str[(strlen(str)-1)] = '\0';
		if(str[0] == '#' && str[1] == 'V')
		{
			versions++;
		}
		else
		{
			/* IF THE STRING IS "- <NUMBER>", LINE AT INDEX = NUMBER 
				OF THE PREVIOUS VERSION IS DELETED */ 
			if(str[0] == '-')
			{
				sscanf(str, "%c %d", &ch, &d);
				for(k=d;k<(line_count-1);k++)
					strcpy(buffer[k], buffer[k+1]);		
				line_count--;
			}
			/* ELSE, THE GIVEN LINE IS APPENDED (ADDED TO THE BUFFER) */
			else
			{
					strcpy(buffer[line_count], str);
					line_count++;
			}
		}

	}
	/* CHECK FOR DUPLICATE VERSION (LAST VERSION), ILLEGAL FILE OPERATIONS, UPDATE INFO */
	issues = check(append, buffer, line_count);
	/* NO NEW UPDATE */
	if(issues == 1)
	{
		/* IF FOUND, DON'T ADD, DISPLAY MESSAGE */
		printf("\nThis Version Already Exists (%d)\n\n", versions);
		return -1;
	}
	/* ILLEGAL FILE OPERATION */
	if(issues == -1)
	{
		printf("\nYOU CAN ONLY APPEND ONE LINE OR DELETE ANY ONE LINE AT GIVEN TIME.\nPLEASE UNDO THE CHANGES AND COMMIT.\n");
		return -1;
	}
	versions++;
	/* WRITE THE CONTENTS OF THE CURRENT FILE */
	fprintf(version, "%s %d\n", "#Version", versions);
	fprintf(version, "%s\n", append);
	printf("\nNew Version Added (%d)\n\n", versions);
}

/* FUNCTION TO CHECK IF THE CONTENTS OF THE BUFFER ARE SAME AS THAT OF THE CURRENT FILE */
/* TRACK THE UPDATE AND CREATE THE APPEND STRING */
int check(char append[], char buffer[][LINE_SIZE], int n)
{
	/*
		COUNT  - LINES IN THE CURRENT FILE
		N      - LINES IN THE BUFFER (PREVIOUS VERSION) 
		d      - INDEX OF THE LINE WHICH IS DELETED

	*/

	int i = 0, count=0, d;
	char str[LINE_SIZE];
	rewind(current);
	strcpy(append,"");

	/* COUNT THE NUMBER OF LINES AND INITIALIZE STR WITH THE LAST LINE 
		OF THE CURRENT FILE */
	while(fgets(str, LINE_SIZE, current))
	{
		if(!strcmp(str, "\n"))
		{
			printf("\nBlank line ignored");
			continue;
		}
		count++;
	}
	if(str[(strlen(str)-1)] == '\n')
			str[(strlen(str)-1)] = '\0';

	/* MULTIPLE LINES ADDED/DELETED */
	if(count < (n-1) || count > (n+1))
		return -1;

	/* DUPLICATE VERSION, RETURN 1 */
	if(!strcmp(buffer[n-1], str) && count == n)
		return 1;
	else
	{
		/* LINE DELETED */
		/* STORE APPEND STRING AS "- <index>" */
		if(count == (n-1))
		{
			d=getDeletedIndex(buffer, n);
			sprintf(append,"- %d", d);
		}

		/* LINE APPENDED */
		else if(count == (n+1))
		{
			strcpy(append, str);
		}
		return 0;
	}
}

/* FUNCTION CALLED WHEN THE LINES OF THE CURRENT FILE ARE ONE LESS 
   THE LINES OF THE PREVIOUS VERSION */
/* FIND THE LINE WHICH HAS BEEN DELETED BY COMPARING THE BUFFER
   OF THE PREVIOUS VERSION AND THE CURRENT FILE */
int getDeletedIndex(char buffer[][LINE_SIZE], int n)
{
	int index=0;
	char str[10];
	rewind(current);
	while(fgets(str, LINE_SIZE, current))
	{
		if(str[(strlen(str)-1)] == '\n')
			str[(strlen(str)-1)] = '\0';
		if(!strcmp(str, ""))
		{
			printf("\nBlank line ignored");
			continue;
		}
		/* DELETED LINE INDEX FOUND */
		if(strcmp(str, buffer[index]))
			return index;
		index++;
	}
	/* IF NOT FOUND IN THE LOOP, LAST LINE DELETED */
	return index;
}

/* FUNCTION TO CREATE THE FIRST VERSION. MULTI-LINE TEXT FILE CAN BE COMMITED IN THE FIRST VERSION */
int addContents()
{
	char str[LINE_SIZE];
	int line_count=0;
	while(fgets(str, LINE_SIZE, current))
		line_count++;
	if(line_count == 0)
	{
		printf("\nEmpty File. Please add some content.\n");
		return -1;
	}
	rewind(current);
	fprintf(version, "%s\n", "#Version 0");
	while(fgets(str, LINE_SIZE, current))
	{
		if(str[(strlen(str)-1)] == '\n')
			str[(strlen(str)-1)] = '\0';
		fprintf(version, "%s\n",str);
	}
	printf("\nNew Version Added (0)\n\n");
	return line_count;
}

/* CHECK IF VERSION FILE IS EMPTY */
int checkIfNew()
{
	char str[LINE_SIZE + 5];
	int line_count=0;
	while(fgets(str, LINE_SIZE + 5, version))
	{
		line_count++;
	}
	if(line_count == 0)
		return 1;
	return 0;
}
/********************************************************************************************************************/

/******************************************* FETCH EXISTING VERSION *************************************************/

/* FUNCTION TO LOAD CONTENTS OF THE BUFFER(VERSION) IN THE CURRENT FILE */
void loadVersion(char name[], char buffer[][LINE_SIZE], int n)
{
	int i=0;
	fclose(current);
	current = fopen(name, "w");
	for(i=0;i<n;i++)
		fprintf(current, "%s\n", buffer[i]);
	printf("\nFILE LOADED.");
}

/* FUNCTION TO FETCH THE CONTENTS OF Kth VERSION */
int getOldVersion(char name[], int k)
{
	int v=0, line_count=0, flag=0, i=0, d;
	char ch, str[LINE_SIZE+3], temp[LINE_SIZE], buffer[LINES][LINE_SIZE];
	rewind(version);

	/* SEARCH FOR THE Kth VERSION, AND UPDATE THE BUFFER THROUGH THE PROCESS */
	
	while(fgets(str, (LINE_SIZE+5), version))
	{
		if(str[(strlen(str)-1)] == '\n')
			str[(strlen(str)-1)] = '\0';
		if(str[0] == '#' && str[1] == 'V')
		{
			/* VERSION FOUND, EXIT LOOP AFTER NEXT OPERATION */
			if(v==k)
			{
				flag=1;
			}
			v++;
		}
		else
		{
			/* DELETE A LINE FROM THE BUFFER */
			if(str[0] == '-')
			{
				sscanf(str, "%c %d", &ch, &d);
				for(i=d;i<(line_count-1);i++)
					strcpy(buffer[i], buffer[i+1]);		
				line_count--;
			}
			/* ADD A LINE TO THE BUFFER */
			else
			{
				strcpy(buffer[line_count], str);
				line_count++;
			}
			if(flag == 1)
				break;
		}

	}
	/* IF FLAG = 0, VERSION NOT AVAILABLE. RETURN */
	if(flag == 0)
	{
		printf("\nNOT FOUND. Available versions [0-%d].\n\n", v-1);
		return -1;
	}

	/* DISPLAY THE CONTENTS UNDER THE CURRENT VERSION */
	for(i=0;i<line_count;i++)
		printf("\n%s", buffer[i]);

	/* ASK THE USER IF HE/SHE WANTS TO LOAD THIS VERSION */
	printf("\n\nDo you want load this version [y/n] : ");
	scanf("%c", &ch);
	if(ch == 'y' || ch == 'Y')
		loadVersion(name, buffer, line_count);
	printf("\n\n");
	return 1;
}

/********************************************************************************************************************/

/* FUNCTION TO DISPLAY A LIST OF VALID COMMANDS AND USAGE */
void help()
{
	printf("\n\nADD NEW VERSION       : ADDS THE CURRENT VERSION TO THE VERSION FILE");
	printf("\nUSAGE                 : ./svc commit <filename>");
	printf("\n\nVIEW PREVIOUS VERSION : FETCHES Nth VERSION. OPTION TO LOAD AN EXISTING VERSION PROVIDED");
	printf("\nUSAGE                 : ./svc version <filename> <version-no>\n\n");
}