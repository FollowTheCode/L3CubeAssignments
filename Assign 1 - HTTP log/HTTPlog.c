/* 
    =========================================================================================================
    TITLE  -  MAKING SENSE OUT OF HTTP LOG FILE 
    ---------------------------------------------------------------------------------------------------------
    AUTHOR -  FOLLOW THE CODE
    ---------------------------------------------------------------------------------------------------------
    DATE   -  5th June, 2014
    ---------------------------------------------------------------------------------------------------------
    BRIEF DESCRIPTION - 
    -------------------
    The input to the program is an HTTP log file. On executing, two files are created.
    1) Summary File -

            a) URL STATISTICS     - A list of URLs along with their visited count in descending order
            b) RESULT STATISTICS  - Success (2XX), Redirection (3XX), Error Client (4XX), Error Server (5XX)
            c) BROWSER STATISTICS - List of all browsers with their count in descending order
            d) OS STATISTICS      - List of all operating systems with their count in descending order
            e) BOT STATISTICS     - List of all bots, called spiders with their count in descending order
                                    (Eg - Yahoo ! Slurp, googlebot, exabot, etc.)

    2) Log file - Contains all the records in a readable format.

    ALSO, A MENU IS PROVIDED TO DISPLAY SPECIFIC STATS.
    ==========================================================================================================
*/

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>

#define OS_COUNT 4
#define BROWSER_COUNT 6
#define BOT_COUNT 5

/********************************************************************************************************/

/* TEMPORARY STRUCTURE TO STORE VALUES OF ONE RECORD*/
typedef struct LOG
{
    char ip[20];
    char time[50];
    char request[10];
    char url[25];
    char protocol[10];
    char result[5];
    char bytes[8];
    char r_url[50];
    char userAgent[120];
}LOG;

/* STRUCTURE TO STORE A LIST OF URLs, BROWSERS, etc AND THEIR COUNTS */
typedef struct STAT
{
    char name[30];
    int count;

}STAT;

/* STORES A LIST OF URLs WITH THEIR VISITED COUNT */
STAT URL[100];
int totalURL = 0;

/* STORES A LIST OF BROWSERS WITH THEIR COUNT */
STAT BROWSER[BROWSER_COUNT];

/* STORES A LIST OF OPERATING SYSTEMS WITH THEIR COUNT */
STAT OS[OS_COUNT];

/* STORES A LIST OF BOTS WITH THEIR COUNT */
STAT BOT[BOT_COUNT];



/*
    RESULT CODES -> 
    SUCCESS      - (2XX)
    REDIRECTION  - (3XX)
    CLIENT_ERROR - (4XX)
    SERVER_ERROR - (5XX)
*/
int success=0, client_error=0, server_error=0, redirection=0;

/*****************************************************************************************************/

/* FUNCTIONS TO UPDATE STATISTICS */
void initOS();
void initBrowser();
void initBOT();
int updateOSCount();
int updateBrowserCount();
int updateBOTCount();
int searchURL();
void updateURLCount();
void updateResultStats();

/* FUNCTIONS TO PARSE EACH LOG */
int readLogs();
int getIP(char []);
void getNumber(char []);
void getTime(char []);
void getAccessRequest(char [], char[], char[]);
void getQuoteContents(char []);

/* FUNCTION TO WRITE/DISPLAY */
void writeSummary();
void writeLOG(LOG *);
void displayURL();
void displayResults();
void displayBrowser();
void displayOS();   
void displayBOT();
int displaySummary();

void sort(STAT [], int n);

/*
 FP  - LOG FILE
 OUT - LOG FILE (READABLE)
 SUM - SUMMARY OF THE LOG FILE
*/

FILE *fp, *out, *sum;

/*****************************************************************************************************/
/* MAIN FUNCTION */
int main()
{
    fp = fopen("weblog.txt","r");
    out = fopen("logFile.txt","w");
    sum = fopen("logSummary.txt","w");
    if (fp == NULL || out == NULL || sum == NULL)
    {
        printf("\nFILE COULD NOT BE OPENED/CREATED.\n\n");
        return 0;
    }
    initBrowser();
    initOS();
    initBOT();
    readLogs();
    writeSummary();
    printf("\n\nLOG FILE AND SUMMARY FILE SAVED IN THE CURRENT FOLDER.\n");
    displaySummary();
    fcloseall();
    return 0;
}

/************************************ DISPLAY AND WRITE FUNCTIONS *******************************************************/

/* PROVIDE A MENU AND DISPLAY THE STATS */
int displaySummary()
{
    int ch;  
    while(1)
    {
        printf("\n\t-------------------MENU----------------------");
        printf("\n\t1. URL STATS\n\t2. RESULT STATS\n\t3. BROWSER STATS");
        printf("\n\t4. OS STATS\n\t5. BOT STATS\n\t6. EXIT");
        printf("\n\n\tSelect an Option - ");
        scanf("%d", &ch);
        switch(ch)
        {
            case 1:  displayURL(); break;
            case 2:  displayResults(); break;
            case 3:  displayBrowser(); break;
            case 4:  displayOS(); break;  
            case 5:  displayBOT(); break;
            case 6:  return 0;
            default: printf("\n\n\tInvalid Option.");
        }

    }
}

/* URL STATS */
void displayURL()
{
    int i;
    printf("\n\n-------------------------------------------------------------------------------------------------------------------");
    printf("\nURL STATISTICS -\n");
    for(i=0;i<totalURL;i++)
    {
        printf("\n%s : %d", URL[i].name, URL[i].count);
    }
    printf("\n\n-------------------------------------------------------------------------------------------------------------------\n");
}

/* RESULT CODE STATS */
void displayResults()
{
    printf("\n\n-------------------------------------------------------------------------------------------------------------------");
    printf("\nRESULT STATISTICS - \n");
    printf("\nSUCCESS - %d\nREDIRECTION - %d\nERROR (CLIENT) - %d\nERROR (SERVER) - %d", success, redirection, client_error, server_error);
    printf("\n\n-------------------------------------------------------------------------------------------------------------------\n");   
}

/* BROWSER STATS */
void displayBrowser()
{
    int i;
    printf("\n\n-------------------------------------------------------------------------------------------------------------------");
    printf("\nBROWSER STATISTICS -\n");
    for(i=0;i<BROWSER_COUNT;i++)
    {
        printf("\n%s : %d", BROWSER[i].name, BROWSER[i].count);
    }
    printf("\nNote - 'Other' may include bots, called spiders. EG - googlebot, Yahoo ! slurp, etc \n");
    printf("\n\n-------------------------------------------------------------------------------------------------------------------\n");
}

/* OS STATS */
void displayOS()
{
    int i;
    printf("\n\n-------------------------------------------------------------------------------------------------------------------");
    printf("\nOS STATISTICS -\n");
    for(i=0;i<OS_COUNT;i++)
    {
        printf("\n%s : %d", OS[i].name, OS[i].count);
    }
    printf("\nNote - 'Other' may include bots, called spiders. EG - googlebot, Yahoo ! slurp, etc \n");
    printf("\n\n-------------------------------------------------------------------------------------------------------------------\n");
    
}

/* OS STATS */
void displayBOT()
{
    int i;
    printf("\n\n-------------------------------------------------------------------------------------------------------------------");
    printf("\nBOT STATISTICS -\n");
    for(i=0;i<BOT_COUNT;i++)
    {
        printf("\n%s : %d", BOT[i].name, BOT[i].count);
    }
    printf("\n\n-------------------------------------------------------------------------------------------------------------------\n");
}

/* WRITE ALL THE STATS IN THE SUMMARY FILE */
void writeSummary()
{
    int i=0;
    
    /* URL STATS */
    sort(URL, totalURL);
    fprintf(sum,"-------------------------------------------------------------------------------------------------------------------\n");
    fprintf(sum, "URL STATISTICS - \n\n");
    for(i=0;i<totalURL;i++)
    {
        fprintf(sum, "%s : %d\n", URL[i].name, URL[i].count);
    }
    fprintf(sum,"\n-------------------------------------------------------------------------------------------------------------------\n");
    
    /* RESULT STATS */
    fprintf(sum, "RESULT STATISTICS - \n\n");
    fprintf(sum, "SUCCESS - %d\nREDIRECTION - %d\nERROR (CLIENT) - %d\nERROR (SERVER) - %d\n", success, redirection, client_error, server_error);
    fprintf(sum,"\n-------------------------------------------------------------------------------------------------------------------\n");
    

    /* BROWSER STATS */
    sort(BROWSER, BROWSER_COUNT-1);
    fprintf(sum, "BROWSER STATISTICS - \n\n");
    for(i=0;i<BROWSER_COUNT;i++)
    {
        fprintf(sum, "%s : %d\n", BROWSER[i].name, BROWSER[i].count);
    }
    fprintf(sum, "Note - 'Other' may include bots, called spiders. EG - googlebot, Yahoo ! slurp, etc \n");
    fprintf(sum,"\n-------------------------------------------------------------------------------------------------------------------\n");

    /* OS STATS */
    sort(OS, OS_COUNT-1);
    fprintf(sum, "OS STATISTICS - \n\n");
    for(i=0;i<OS_COUNT;i++)
    {
        fprintf(sum, "%s : %d\n", OS[i].name, OS[i].count);
    }
    fprintf(sum, "Note - 'Other' may include bots, called spiders. EG - googlebot, Yahoo ! slurp, etc \n");
    fprintf(sum,"\n-------------------------------------------------------------------------------------------------------------------\n");
    
    /* BOT STATS */
    sort(BOT, BOT_COUNT);
    fprintf(sum, "BOT STATISTICS - \n\n");
    for(i=0;i<BOT_COUNT;i++)
    {
        fprintf(sum, "%s : %d\n", BOT[i].name, BOT[i].count);
    }
    fprintf(sum,"\n-------------------------------------------------------------------------------------------------------------------\n");
}

/* FUNCTION TO START S[] IN DESCENDING ORDER OF COUNT */
void sort(STAT S[], int n)
{
    int i,j;
    STAT temp;
    for(i=1;i<n;i++)
    {
        j=i;
        while(j>0 & S[j].count > S[j-1].count)
        {
            temp = S[j];
            S[j] = S[j-1];
            S[j-1] = temp;
            j--;
        }
    }
}

/* WRITE EVERY RECORD IN THE FORMAT BELOW */
void writeLOG(LOG *log)
{
    fprintf(out,"-------------------------------------------------------------------------------------------------------------------\n");
    fprintf(out, "IP              : %s\n", log->ip);
    fprintf(out, "TIME            : %s\n", log->time);
    fprintf(out, "REQUEST         : %s\n", log->request);
    fprintf(out, "URL             : %s\n", log->url);
    fprintf(out, "PROTOCOL        : %s\n", log->protocol);
    fprintf(out, "RESULT CODE     : %s\n", log->result);
    fprintf(out, "BYTES           : %s\n",  log->bytes);
    fprintf(out, "REFERRINING URL : %s\n", log->r_url);
    fprintf(out, "USER AGENT      : %s\n\n", log->userAgent);
}

/********************************************************************************************************************/
/********************************************** UPDATE STATISTICS ***************************************************/
/* RETURN THE INDEX OF THE URL TO BE SEARCHED */
int searchURL(char str[])
{
    int i=0;
    for(i=0;i<totalURL;i++)
    {
        if(strcmp(str, URL[i].name) == 0)
            return i;
    }
    return -1;
}

/* SEARCH FOR URL AND INCREMENT COUNT, IF NOT FOUND, ADD NEW */
void updateURLCount(char str[])
{
    int i=searchURL(str);
    if(i!=-1)
        URL[i].count++;
    else
    {
        strcpy(URL[totalURL].name, str);
        URL[totalURL].count = 1;
        totalURL++;
    }
}

/* UPDATE RESULT CODE STATS */
void updateResultStats(char str[])
{
    if(str[0] == '2')
        success++;
    else if(str[0] == '3')
        redirection++;
    else if(str[0] == '4')
        client_error++;
    else if(str[0] == '5')
        server_error++;
}

/* INITIALIZE THE LIST OF BROWSERS */
void initBrowser()
{
    int i;
    strcpy(BROWSER[0].name,"Firefox");
    strcpy(BROWSER[1].name,"MSIE");
    strcpy(BROWSER[2].name,"Chrome");
    strcpy(BROWSER[3].name,"Opera");
    strcpy(BROWSER[4].name,"Safari");
    strcpy(BROWSER[5].name,"\nOther");
    for(i=0;i<BROWSER_COUNT;i++)
    {
        BROWSER[i].count=0;
    }
}

/* INITIALIZE THE LIST OF OPERATING SYSTEMS */
void initOS()
{
    int i;
    strcpy(OS[0].name,"Linux");
    strcpy(OS[1].name,"Mac OS");
    strcpy(OS[2].name,"Windows");
    strcpy(OS[3].name,"\nOther");
    for(i=0;i<OS_COUNT;i++)
    {
        OS[i].count=0;
    }
    

}

/* INITIALIZE THE LIST OF BOTS */

void initBOT()
{
    int i;
    strcpy(BOT[0].name,"Yahoo! Slurp");
    strcpy(BOT[1].name,"Googlebot");
    strcpy(BOT[2].name,"Exabot");
    strcpy(BOT[3].name,"Twiceler");
    strcpy(BOT[4].name,"msnbot");
    for(i=0;i<BOT_COUNT;i++)
    {
        BOT[i].count=0;
    }
}


/* DETECT AND UPDATE BROWSER COUNT */
int updateBrowserCount(char str[])
{
    int i=0;

    if(strstr(str, "Version") && strstr(str, "Safari"))
    {
        BROWSER[4].count++;
        return 1;
    }
    for(i=0;i<(BROWSER_COUNT-1);i++)
    {
        if(strstr(str,BROWSER[i].name))
        {
            BROWSER[i].count++;
            return 1;
        }
    }
    //printf("\n\t%s", str);
    BROWSER[BROWSER_COUNT-1].count++;
    return 1;
}

/* DETECT AND UPDATE OS COUNT */
int updateOSCount(char str[])
{
    int i;
    for(i=0;i<(OS_COUNT-1);i++)
    {
        if(strstr(str,OS[i].name))
        {
            OS[i].count++;
            return 1;       
        }
    }
    //printf("\n\t%s", str);
    OS[OS_COUNT-1].count++;
    return 1;
}

/* DETECT AND UPDATE BOT COUNT */
int updateBOTCount(char str[])
{
    int i;
    for(i=0;i<BOT_COUNT;i++)
    {
        if(strstr(str,BOT[i].name))
        {
            BOT[i].count++;
            return 1;       
        }
    }
    //printf("\n\t%s", str);
    return 1;
}

/********************************************************************************************************************/
/********************************************* PARSE EVERY RECORD ***************************************************/

/* IP IS THE FIRST FIELD OF EVERY RECORD */
/* FORMAT - XXX.XXX.XXX.XXX */
int getIP(char str[])
{
    char ch;
    int i=0;
    //ch = getc(fp);
    while(!feof(fp))
    {
        ch=getc(fp);
        if(ch == ' ')
        {
            //printf("\ndone\n");
            str[i]='\0';
            return 0;
        }
        str[i]=ch;
        i++;
    }
    return -1;
}

/* FUNCTION TO GET TIME - 4/May/2009:00:10:34   -0700 */
/*                          (DATE)    (TIME)  (TIME DIFFERENCE) */
void getTime(char result[])
{
    int k=0;
    char ch;
    while((ch = getc(fp)) != '[');
    ch = getc(fp);
    while((ch = getc(fp)) != ']')
    {
        result[k++] = ch;
    }
    result[k] = '\0';
    ch = getc(fp);
    ch = getc(fp);
    //ch = getc(fp);
}

/* FUNCTION TO GET REQUEST TYPE, URL AND PROTOCOL */
void getAccessRequest(char request[], char url[], char protocol[])
{
    char ch;
    int k=0;
    while((ch = getc(fp)) != ' ')
    {
        request[k++] = ch;
    }
    request[k] = '\0';
    k=0;
    
    while((ch = getc(fp)) != '/')
    {
        url[k++] = ch;
    }
    url[k] = '\0';

    k=0;
    updateURLCount(url);
    while((ch = getc(fp)) != ' ');

    while((ch = getc(fp)) != '"')
        protocol[k++] = ch;

    ch = getc(fp);
}

/* FUNCTION TO GET RESULT CODE AND BYTES */
void getNumber(char result[])
{
    char ch;
    int k=0;
    while((ch = getc(fp)) != ' ')
        result[k++] = ch;
    result[k] = '\0';
}

/* FUNCTION TO GET TEXT IN QUOTES */
/* USED TO GET USERAGENT AND REFERRING URL */
void getQuoteContents(char result[])
{
    char ch;
    int k=0;
    ch = getc(fp);
    while((ch = getc(fp)) != '"')
    {
        result[k++] = ch;
    }
    result[k] = '\0';
    ch = getc(fp);
    
}

/* READ EVERY RECORD, PARSE AND CALL FUNCTIONS TO UPDATE STATS */
int readLogs()
{
    int length, count = 0;
    LOG *log = (LOG *)malloc(sizeof(LOG));
    while(1)
    {
        /*
        if(count == 10)
            break;
        count++;
        */
        if(getIP(log->ip) == -1)
        {
            return 0;
        }
        /* GET TIME */
        getTime(log->time);
        
        /* GET REQUEST TYPE, URL, PROTOCOL */
        getAccessRequest(log->request, log->url, log->protocol);
        
        /* GET RESULT CODE */
        getNumber(log->result);

        /* GET BYTES */
        getNumber(log->bytes);

        /* GET REFERRING URL */
        getQuoteContents(log->r_url);

        /* GET USERAGENT */
        getQuoteContents(log->userAgent);
    
        /* PROCESS RECORDS TO UPDATE STATS */
        updateResultStats(log->result);
        updateOSCount(log->userAgent);
        updateBrowserCount(log->userAgent);
        updateBOTCount(log->userAgent);

        /* WRITE RECORDS IN NEW FILE (FORMATTED) */ 
        writeLOG(log);
    }    

}

/********************************************************************************************************************/