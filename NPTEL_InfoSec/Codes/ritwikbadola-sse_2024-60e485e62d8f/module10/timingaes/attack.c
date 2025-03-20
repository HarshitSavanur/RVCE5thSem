/******************************************************************************
 * File	Name			: attack.c
 * Organization                 : Indian Institute of Technology Kharagpur
 * Project Involved		: First Round Attack on AES
 * Author		    	: Chester Rebeiro 
 * Date of Creation		: 15/Dec/2012
 * Date of freezing		: 
 * Log Information regading 
 * maintanance			:
 * Synopsis			: 
 ******************************************************************************/
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <aes.h>

#include "params.h"



#define ITERATIONS (1 << 24)              /* The maximum iterations for making the statistics */

AES_KEY expanded;

#define TIME_THRESHOLD     8000    // for 4 tables of 1024 bytes

unsigned char pt[16];               /* Holds the Plaintext */
unsigned char ct[16];               /* Holds the ciphertext */

unsigned int ttime[16][16];         /* Holds the timing     */
unsigned int tcount[16][16];        /* Holds the count      */
double tavg[16][16];                /* ttime[x]/tcount[x]   */
double deviations[16][16];          /* Deviations from tavgavg */
double tavgavg;                     /* Average of all timings */

void printtime()
{
	int i, c;
	FILE *f;

	f = fopen("log", "w");
	for(c=4; c<16; ++c){
		fprintf(f, ".............%d.........\n", c);
		for(i=0; i<16; ++i){
			fprintf(f, "%d  %.3f  %.4f\n", i, tavg[c][i], deviations[c][i]);
		}
	}
	fclose(f);
}

unsigned int finddeviant(unsigned int c)
{
	int i, maxi;
	double ttimesum, tcountsum;
	double maxdeviation;

	/* Compute average timing for c */
	ttimesum = 0;
	tcountsum = 0;
	for(i=0; i<16; ++i){
		tavg[c][i] = ttime[c][i] / (float)tcount[c][i];	
		ttimesum += ttime[c][i];
		tcountsum += tcount[c][i];
	}	
	tavgavg = ttimesum/tcountsum;

	/* Compute deviations from the average time */
	for(i=0; i<16; ++i){
		deviations[c][i] = fabs(tavg[c][i] - tavgavg);
	}	

	/* Find the maximum deviation, this is the possible leakage */
	maxdeviation = deviations[c][0];
	maxi = 0;
	for(i=1; i<16; ++i){
		if(maxdeviation < deviations[c][i]){
			maxdeviation = deviations[c][i];
			maxi = i;
		}
	}

	return maxi;
}

void findkeys()
{
	int c=4;
	for (c=4; c<16; ++c)
		printf("%02d(%x) ", c, finddeviant(c));	
	printf("\n");
}

double attackrnd1()
{
	int ii=0, i;
	unsigned int start, end, timing;

	while(ii++ <= (ITERATIONS)){
		/* Set a random plaintext */
		for(i=0; i<16; ++i) pt[i] = random() & 0xff;
		/* Fix a few plaintext bits of some plaintext bytes */
		pt[0] = pt[0] & 0x0f;
		pt[1] = pt[1] & 0x0f;
		pt[2] = pt[2] & 0x0f;
		pt[3] = pt[3] & 0x0f;

		/* clean the cache memory of any AES data */
		cleancache();	

		/* Make the encryption */
		start = timestamp();
		AES_encrypt(pt, ct, &expanded);
		end = timestamp();

		timing = end - start;

		if(ii > 1000 && timing < TIME_THRESHOLD){      
			/* Record the timings */
			for(i=4; i<16; ++i){
				ttime[i][pt[i] >> 4] += timing;
				tcount[i][pt[i] >> 4] += 1;	
			}	
			
			/* print if its time */
			if (!(ii & (ii - 1))){
				printf("%08x\t", ii);
				findkeys();
				printtime(4);
			}
		}
	}
}


void ReadKey(const unsigned char *filename)
{
	int i;
	FILE *f;
	unsigned int i_secretkey[16]; 
	unsigned char uc_secretkey[16]; 

	/* Read key from a file */
	if((f = fopen(filename, "r")) == NULL){
		printf("Cannot open key file\n");
		exit(-1);
	}
	for(i=0; i<16; ++i){
		fscanf(f, "%x", &i_secretkey[i]);
		uc_secretkey[i] = (unsigned char) i_secretkey[i];
	}
	fclose(f);
	AES_set_encrypt_key(uc_secretkey, 128, &expanded);
}


/* 
 * The main 
 */
int main(int argc, char **argv)
{
	srandom(timestamp());

	ReadKey("key");
	printf("Getting First Round Key Relations\n");
	attackrnd1();
}

