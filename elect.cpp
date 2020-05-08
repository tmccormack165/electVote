#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<iostream>


struct State{
	char stateName[20];
	int stateID;
	int electoralVotes;
	int zone;
	int isCenter;
	long population;
	long amountRegistered;
	float registeredPerc;

	char practiceRegister[20];
	char practicePerc[20];
	char practicePop[20];

};

int main(int argc, char ** argv){
	//in our simulation D.C. has electoral votes
	int numberOfStates = 51;
	printf("Hello Electorals \n");

	//allocate an array of fifty states...
	struct State* states;
	states = (struct State*) malloc(numberOfStates*sizeof(struct State));

	//assign each state a unique identifier
	for(short i = 0; i < numberOfStates; i++){
		states[i].stateID = i;
		printf("%d \n", states[i].stateID);
	}

	//open electorals.txt
	//State: Number of votes
	FILE * fp_elect;
	fp_elect = fopen("electorals.txt", "r");

	if(fp_elect == NULL){
		printf("ERROR: 'electorals.txt' missing in directory \n");
		return 1;
	}
	char delimeter;	//store delimeter value (throwaway)
	string n;	//store electoral votes before typecast

	for(short j = 0; j < numberOfStates; j++){
		//collect the state Name
		fscanf(fp_elect, "%s", states[j].stateName);
		//ignore the delimeter to get to the int
		delimeter = fgetc(fp_elect);
		//get the electoral Votes attribute
		fscanf(fp_elect, "%s", n);
		states[j].electoralVotes = (int)n;
		//print results
		printf("%s \n", states[j].stateName);
		printf("%d \n", states[j].electoralVotes);

	}
	//open up data.csv (State, totalRegistered, registeredPerc, Pop)
	FILE * fp_data;
	fp_data = fopen("data.csv", "r");
	char throw[100];

	if(fp_elect == NULL){
		printf("ERROR: 'data.csv' missing in directory \n");
		return 1;
	}
	//TODO dynamically allocate the throw string
	//throw away colunmn headers
	fscanf(fp_data, "%s", throw);

	//state
	char c;
	char* tmp = (char *) malloc(100 * sizeof(char));
	int k,l;

	for(short l = 0; l < 3; l++){
		if(l ==2 ){
			printf("LOOK HERE: %s \n", tmp);
		}
		memset(tmp, 0, sizeof(tmp));
		//start name
		k = 0;
		while(c != ','){
			c = fgetc(fp_data);
			
			//printf("%c \n", c);
			if(c != ','){
				//states[0].practice[k] = c;
				tmp[k] = c;
				if(l == 2){
					printf("%c \n", c);
				}
				k++;
			}
		}
		printf("%s \n", tmp);
		//end name
		memset(tmp, 0, sizeof(tmp));

		//start Registered
		k = 0;
		do{
			c = fgetc(fp_data);
			if(c != ','){
				tmp[k] = c;
				k++;
			}

		}while(c != ',');
		printf("%s \n", tmp);
		//end Registered
		memset(tmp, 0, sizeof(tmp));

		
		//start perc
		k = 0;
		

		do{
			c = fgetc(fp_data);
			if(c != ','){
				tmp[k] = c;
				k++;
			}
		}while(c != ',');
		printf("%s \n", tmp);
		//end perc
		memset(tmp, 0, sizeof(tmp));


		//start pop
		k = 0;
		do{
			c = fgetc(fp_data);
			if(c != ',' && c != '\n'){
				tmp[k] = c;
				k++;
			}
		}while(c != '\n');
		printf("%s \n \n \n", tmp);
		//end pop
		memset(tmp, 0, sizeof(tmp));

	}



	

	

	//TODO: free throw


	free(tmp);
	free(states);

	return 0;

}