/*
   ==============================================================================
     Title  - Birthday Paradox Validator
        ------------------------------------------------------------------------
     Author - Follow The Code
        ------------------------------------------------------------------------
     Date   - 6th June 2014
        ------------------------------------------------------------------------
     Brief Description

     -This code runs trials of assigning random birthdays to a group of
      people and computes the probability of 2 birthdays being the same.   
      It then compares this probability with the pre-calculated mathematical 
      probabilty and terminates when the values become equal(+- 0.2).
   ==============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 365

//Function prototypes
int cmp(const void *,const void *);
int duplicates(int [],int);
void to_date(int);
double mathematicalprob(int );
double absolute(double );

//Structure to hold month names with their corresponding no. of days
typedef struct monthdays
{
    int days;
    char mon_name[15];
}monthdays;

struct monthdays m[12]={{31,"January"},{28,"February"},{31,"March"},{30,"April"},{31,"May"},{30,"June"},
		                 {31,"July"},{31,"August"},{30,"September"},{31,"October"},{30,"November"},{31,"December"}};

/*
     ================================
      Main Function       
     ================================
*/
int main()
{
    int people,i;
    int dup,repeated,totdup;
    int trials;
    int bdays[SIZE];
    char ch;
    double prob,mathprob;

    totdup=repeated=trials=0;

    //Set a seed for random number
    srand(time(NULL));

    //Get the number of people 
    printf("Enter number of people:");
    scanf("%d",&people);

    if(people<2)
    {
	   printf("Invalid number of people\n");
	   return 0;
    }
    
    /*
        Calculate the mathematical probability for 2 birthdays beings the same for 
        total no of people
    */
    mathprob=mathematicalprob(people);

    //Repeat until trial probability and mathematical probaility are equal
    while(1)
    { 
        trials++;
        printf("\n***************************\n");
        printf("--Trial %d--\n",trials);
        
        //Generate random birth dates for all the people and print them
        printf("\nRandomly assigned birthdays\n\n");
        for(i=0;i<people;i++)
        {
            bdays[i]=rand()%366+1;
            to_date(bdays[i]);
        }

        //Check for duplicate birth dates in the group of people
        printf("\nDuplicate birth dates:\n");
        dup=duplicates(bdays,people);
        printf("\n");
        /*
            If duplicate dates are found increment the value of "repeated" which 
            holds the number of trials with duplicate birthdays.
        */
        if(dup)
            repeated++;

        //Maintain a count of total number of duplicate birthdays found overall
        totdup+=dup;

        /*
            Calculation of manual probability of trials by dividing no. of trials 
            having repetitions with total no of trials.
        */
        prob=(double)repeated/trials;

        //Print the details of a single trial
        printf("Number of repeated values in this trial:%d\n\n",dup);
        printf("Number of repeated values through all the trials:%d\n\n",repeated);
        printf("Probability = (%d/%d)= %lf\n\n",repeated,trials,prob); 

        //If both probabilities are approximately equal, print the result and break the loop
        if(absolute((100*mathprob)-(100*prob))<=0.2)
        {
            printf("\nMathematical probability for %d people : %lf\n",people,mathprob);
            printf("\nBoth probabilities are approximately equal.\nHence, the birthday paradox has been proved valid.");
            printf("\n***************************\n");
            break;
        }
    }
}

/*
   ==============================================================
     Function Objective - Computes the absolute value of a value
        -----------------
     Parameter          - No. to be converted
        -----------------
     Return Value       - Converted absolute value 
   ==============================================================
*/
double absolute(double no)
{
    if(no<0)
        return (-1*no);
    return no;
}

//Compare function for quick-sort
int cmp(const void *a,const void *b)
{
    return (*(int *)a-*(int *)b);
}
 
/*
   ======================================================================
     Function Objective - Check for duplicate birthdays
        -----------------
     Parameters         - (1)Array containing birth dates of the people
                          (2)No. of the people
        -----------------
     Return Value       - Number of duplicate dates found
   ======================================================================
*/
int duplicates(int bdays[],int people)
{
    int i,count=0;
    qsort(bdays,people,sizeof(int),cmp);
    for(i=1;i<people;i++)
    {
        if(bdays[i]==bdays[i-1])
        {
            to_date(bdays[i]);
            count++;
        }
    }
    if(count==0)
	printf("Nil\n");
    return count;
}

/*
   ==================================================================
     Function Objective - Print birth dates in readable format
        -----------------
     Parameters         - No. of the people
        -----------------
     Return Value       - None
   ==================================================================
*/
void to_date(int n)
{
    int i;
    for(i=0;i<12;i++)
    {
	if(n<=m[i].days)
	    break;
	n-=m[i].days;
    }
    printf("--%d %s\n",n,m[i].mon_name);
}

/*
   =====================================================================================================
     Function Objective - Computes the mathematical probability for 2 birthdays to be the same
        -----------------
     Parameters         - No. of the people
        -----------------
     Return Value       - Mathematical Probability
        -----------------
     Mathematical Working
     
      1) P(2 same birthdays)= 1 - P(no same birthdays)
      2) Hence, we find the probability of no 2 people having the same birthday
      3) This is done using permutation 
      4) Example:
          For 23 people it will be (365/365)*(364/365)*(363/365)*(362/365)*(361/365)......*(343/365)
          This value is 0.492703

          So, P(2 same birthdays)= 1 - 0.492703 = 0.507297 or 50.7%

   ======================================================================================================
*/
double mathematicalprob(int people)
{
    double prob = 1.0;
    int i;
    for(i=1; i<=people; i++)
    {
        prob = prob * ((double) ( 365 - i + 1) / 365);
    }
    prob = 1.0 - prob;
    return prob;
}
