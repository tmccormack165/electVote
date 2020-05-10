/*
Pledge : I have abided by the Wheaton College Honor Code in this work.
Name   : Timothy McCormack
Summary: This program simulates an American election, it uses the POSIX library
to execute the random number generation in parallel. The executable(./out) takes
no arguments, and the output of the program can be found in stdout. 
The election results consists of:
	1. The breakdown of the election per state (winner, popular vote)
	2. The final winner of the election and the final electoral and popular vote
	3. The total cost (dollars) of the election
	4. The amound of time (days) of the election


*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>  //POSIX API
#include <sys/time.h> //includes timing structures
#define NUMTHREADS 8

//structure to hold the data for all fifty states(including DC)
struct State{
	int stateID;			//unique ID
	int electoralVotes;		//amount electoral votes
	int zone;				//geographic zone
	int distanceFromCenter; //distance from agency (Iowa: zone 3)
	int isCenter;			//is this state the central agency?
	long population;		//state population
	long amountRegistered;	//registered voters in the state
	float registeredPerc;	//percent of population registered
	char stateName[20];		//name of state
	int actualVoters;		//how many people actually voted

	char practiceRegister[20]; //used for ascii conversion
	char practicePerc[20];     //used for ascii conversion   
	char practicePop[20];      //used for ascii conversion

	int mailCost;				//cost for the mail coming from Iowa
	int mailTime;				//the time it takes to get mail from Iowa

	int red;					//the number of red votes
	int blue;					//the number of blue votes

	char winner;				//the winning party
};

//a structure to keep track of aggregating metrics
struct Country{
	long totalCost;		//the total cost of the election
	int totalTime;		//the total time for the election (MAX(mailTime) + 3)

	int redElectorals;	//the number of electoral votes for red (republican)
	int blueElectorals; //the number of electoral votes for blue (democrat)

	int redPopVote;		//the popular votes for red (republican)
	int bluePopVote;    //the popular votes for blue (democrat)

	char countryName[50];//the name of the country

	char winner;		 //the final winner
};

//a thread to keep track of data for each thread
struct threadData{
	int start, stop;			//the partitioning values for which states belong
	struct State* stateArray;	//a pointer to gain access to the state data structure

};

//PRE: states[index].mailTime == NULL && states[index].mailCost == NULL
//POST: states[index].mailTime == {3,4,5,7,8,9} && states[index].mailCost == {5,7,10,12,15,20,40}
void mail(struct State* states, int index){
	if(states[index].distanceFromCenter == 0){
		states[index].mailTime = 3;
		states[index].mailCost = 5;
	}
	else if(states[index].distanceFromCenter == 1){
		states[index].mailTime = 4;
		states[index].mailCost = 7;
	}
	else if(states[index].distanceFromCenter == 2){
		states[index].mailTime = 5;
		states[index].mailCost = 10;
	}
	else if(states[index].distanceFromCenter == 3){
		states[index].mailTime = 7;
		states[index].mailCost = 12;
	}
	else if(states[index].distanceFromCenter == 4){
		states[index].mailTime = 8;
		states[index].mailCost = 15;
	}
	else if(states[index].distanceFromCenter == 5){
		states[index].mailTime = 9;
		states[index].mailCost = 20;
	}
	else if(states[index].distanceFromCenter >= 6){
		states[index].mailTime = 10;
		states[index].mailCost = 40;
	}

}


//PRE: stateArrayT[i].red == 0 && stateArrayT[i].red == 0 
//POST: stateArrayT[i].red has the correct number of popularvotes
void* vote(void* td){
	//cast the void pointer to a threadData pointer
	struct threadData* data = (struct threadData *) td;
	int start = data -> start;			
	int stop = data -> stop;
	struct State* stateArrayT = data->stateArray;
	int cap;	//cap == the amount of actual voters in a state

	//each thread collects the votes for seven threads (except NTHREADS-1)
	for(short i = start; i < stop; i++ ){
		cap = stateArrayT[i].actualVoters;
		int vote;
		for(int j = 0; j < cap; j++){
			vote = rand() % 2;			//generate the vote
			if(vote == 0)
				stateArrayT[i].red++;	//update red
			else
				stateArrayT[i].blue++;	//update blue
		}
		//outout stateName, popular vote to stdout, and calculate the winner in each state
		if(stateArrayT[i].red >= stateArrayT[i].blue){
			stateArrayT[i].winner = 'r';
			printf("%s: red (red = %d, blue = %d)\n", stateArrayT[i].stateName, stateArrayT[i].red,stateArrayT[i].blue);
		}
		else{
			stateArrayT[i].winner = 'w';
			printf("%s: blue  (red = %d, blue = %d)\n", stateArrayT[i].stateName, stateArrayT[i].red,stateArrayT[i].blue);

		}
	}
	
	//remember the thread functions return void pointers...
	return NULL;
}
//PRE: Country *usaPtr.winner == NULL (the final winner has not been calculated)
//POST: Country *usaPtr.winner == 1 || Country *usaPtr.winner == 0
void aggregateVotes(struct State*states, int index, struct Country usaPtr, 
	int *rdPtr, int *bluPtr,int numberOfStates, int *totalTime, long *totalCost, int* redPop, int* bluePop){

	if(states[index].winner == 'r'){
		*rdPtr += states[index].electoralVotes; //collect electoral votes
	}
	else{
		*bluPtr += states[index].electoralVotes; //collect electoral votes
	}

	//calculate the incoming mail cost
	*totalCost += states[index].mailCost;

	//account for outgoing mail (constant intrazone rate)
	*totalCost += 5*states[index].amountRegistered;	//amountRegistered NOT actual voters

	//update the popular vote
	*redPop += states[index].red;
	*bluePop += states[index].blue; 

	//output results to stdout
	if(index == (numberOfStates - 1)){
		usaPtr.redElectorals = *rdPtr;
		usaPtr.blueElectorals = *bluPtr;
		if(usaPtr.redElectorals >= usaPtr.blueElectorals){
			usaPtr.winner = 'r';
		}
		if(usaPtr.redElectorals < usaPtr.blueElectorals){
			usaPtr.winner ='b';
		}
		
		//account for outgoing mail (intrazone)
		printf("\n\n\nTOTAL COST: $%ld.00 \nTOTAL TIME: 11 days\n", *totalCost);
		if(usaPtr.winner == 'r'){
			printf("WINNER: Republican (RED)\n");
		}
		else{
			printf("WINNER: Democrat (BLUE)\n");
		}
		printf("\n---------- ELECTORAL VOTE ----------\n");
		printf("%d RED, %d BLUE\n\n", usaPtr.redElectorals, usaPtr.blueElectorals);
		printf("---------- POPULAR VOTE  -----------\n");
		printf("%d RED, %d BLUE\n",*redPop, *bluePop);


	}



}

int main(int argc, char ** argv){
	int numberOfStates = 51; //50 states + DC
	int agency = 9;			 //9 == ID for IOWA	
	float turn_out = 0.5;    //how likely the registered voters will vote
	struct timeval start, end; //for timing

	//seed random time generator
	srand(time(NULL));




	//allocate an array of fifty one states using malloc (will be freed)
	struct State* states;
	states = (struct State*) malloc(numberOfStates*sizeof(struct State));

	//create a Country structure to aggregate the data at the end
	struct Country USA;
	struct Country usaPtr;

	usaPtr = USA;
	strcpy(usaPtr.countryName, "Unites States of America");

	//assign each state a unique identifier (ID)
	for(short i = 0; i < numberOfStates; i++){
		states[i].stateID = i;
	}

	//open electorals.txt to get the number of electoral votes per state
	FILE * fp_elect;
	fp_elect = fopen("electorals.txt", "r");

	if(fp_elect == NULL){
		printf("ERROR: 'electorals.txt' missing in directory \n");
		return 1;
	}
	char delimeter;	//store delimeter value (throwaway)
	char n[2];	//store electoral votes before typecast

	for(short j = 0; j < numberOfStates; j++){
		fscanf(fp_elect, "%s", n);
		states[j].electoralVotes = atoi(&n);
	}

	//open up data.csv (State, totalRegistered, registeredPerc, Pop)
	//this file gives us the stateName, the population of registered voters,
	//the percentage of registered voters to the general population,
	//and the general population
	FILE * fp_data;
	fp_data = fopen("data.csv", "r");

	if(fp_elect == NULL){
		printf("ERROR: 'data.csv' missing in directory \n");
		return 1;
	}
	char *throw = (char*) malloc(100 *sizeof(char));
	//TODO dynamically allocate the throw string
	//throw away colunmn headers
	fscanf(fp_data, "%s", throw);
	free(throw);

	char c; // c = fgetc() reusable
	int k,l; //k = iterator per word, l = iterator per step

	//csv parser
	for(short l = 0; l < numberOfStates; l++){
		k = 0;
		while(c != ','){
			c = fgetc(fp_data);
			
			if(c != ','){
				states[l].stateName[k] = c;
				k++;
			}
		}

		k = 0;
		do{
			c = fgetc(fp_data);
			if(c != ','){
				//tmp[k] = c;
				states[l].practiceRegister[k] = c;
				k++;
			}

		}while(c != ',');

		k = 0;

		do{
			c = fgetc(fp_data);
			if(c != ','){

				states[l].practicePerc[k] = c;
				
				k++;
			}
		}while(c != ',');

		k = 0;
		do{
			c = fgetc(fp_data);
			if(c != ',' && c != '\n'){
				states[l].practicePop[k] = c;			
				k++;
			}
		}while(c != '\n');
		//atoi/atof conversions because the .csv file is read as an ascii file
		states[l].amountRegistered = atoi(&states[l].practiceRegister);
		states[l].registeredPerc = atof(&states[l].practicePerc);
		states[l].population = atoi(states[l].practicePop);		

	}


	//final file to open contains the zoning data for each state
	int m;
	char z[2]; //zone char
	FILE *fp_zone;
	fp_zone = fopen("zone.txt", "r");
	if(fp_zone == NULL){
		printf("ERROR: zone.txt is nonexistent in directory");
	}

	for(m = 0; m < numberOfStates; m++){
		fscanf(fp_zone, "%s", z);
		states[m].zone = atoi(&z);

	}


	//now we want one state to be the voting agency, that state is Iowa in zone 3
	//also calculate actual voters


	for(short o = 0; o < numberOfStates; o++){
		states[o].isCenter = 0;
		if(o== agency)
			states[o].isCenter = 1;
		float result = turn_out * states[o].amountRegistered;
		states[o].actualVoters = (long)result;
	}


	//calculate the zoning difference from the agency for each state
	for(short p = 0; p < numberOfStates; p++){
		states[p].distanceFromCenter = abs(states[p].zone - states[agency].zone);
	}


	//START TIMER
	gettimeofday(&start, NULL); //start of measurement


	//calculate the mailing costs with mail().
	for(short q = 0; q < numberOfStates; q++){
		mail(states, q);
	}
	//now it is time to simulate voting
	//headers for stdout
	printf("\n---------- STATE ELECTIONS ----------\n");
	printf("Stae: winner (popular votes)\n");

	//thread id's for all threads
	pthread_t tid[NUMTHREADS];

	//each thread has struct for params
	struct threadData data[NUMTHREADS];
	int i;	//iterator

	//work per thread
    int taskPerThread = (numberOfStates + NUMTHREADS -1)/NUMTHREADS;

    //preparing parameters for each thread
    for(int i = 0; i < NUMTHREADS; i++){
        data[i].start = i*taskPerThread;
        data[i].stop = (i+1) *taskPerThread;
        data[i].stateArray = states;
    }
    //the last stop value will run over the array without the line below
    data[NUMTHREADS-1].stop = numberOfStates;

	//Launch the threads    
    for(i = 0; i < NUMTHREADS; i++){
        pthread_create(&tid[i], NULL, vote, &data[i]);
    }
    //Wait for the threads to execute
    for(i = 0; i < NUMTHREADS; i++){
        pthread_join(tid[i], NULL);
    }
	

	int red = 0; 			//red electoral votes
	int blue = 0;			//blue electoral votes
	int popVoteRed = 0;	    //popular votes for red
	int popVoteBlue = 0;    //popular votes for blue
	//ensure that the total cost is reset
	usaPtr.totalCost = 0;

	//aggregate all metrics (electorals, popular votes, time , cost) and declare final winner
	for(short s = 0; s < numberOfStates; s++){
		aggregateVotes(states,s, usaPtr, &red, &blue, numberOfStates, &usaPtr.totalTime, 
			&usaPtr.totalCost, &popVoteRed, &popVoteBlue);
	}

	//END
	gettimeofday(&end, NULL); //start of measurement

	//every malloc needs a free()
	free(states);

	//time calculation
  	unsigned long secs = end.tv_sec - start.tv_sec;
  	unsigned long usecs = end.tv_usec - start.tv_usec;
  	long total = ((((long)(secs))*1000000) + ((long)(usecs)))/1000; /*total time in milliseconds*/
  	printf("Runtime: %ld ms\n", total);


	return 0;

}