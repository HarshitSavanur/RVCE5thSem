/******************************************************************************
 * File	Name			: common.c
 * Organization         : Indian Institute of Technology Kharagpur
 * Project Involved		: Timing attack on Clefia
 * Author		    	: Chester Rebeiro 
 * Date of Creation		: 1/Aug/2009
 * Date of freezing		: 7/Dec/2009
 * Log Information regading 
 * maintanance			:
 * Synopsis			    : Contains functions used to manage the database,
 *                        find timing etc.
 *
 * Copyright(c) 2009 NTT, Corp. All Rights Reserved.
 ******************************************************************************/
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "params.h"


void BytePut(const unsigned char *data, int bytelen)
{
	while(bytelen-- > 0){
		printf("%02x", *data++);
	}
	printf("\n");
}

/********************************************************************
 * The timestamp function. Modified from Bernstein's code
 * to support flushing of cacheline via cpuid information
 ********************************************************************/
unsigned int timestamp(void)
{
	unsigned int bottom;
	unsigned int top;
	asm volatile("xorl %%eax,%%eax\n cpuid \n" ::: "%eax", "%ebx", "%ecx", "%edx"); // flush pipeline 
	asm volatile("rdtsc\n" : "=a" (bottom), "=d" (top) );                           // read rdtsc
	asm volatile("xorl %%eax,%%eax\n cpuid \n" ::: "%eax", "%ebx", "%ecx", "%edx"); // flush pipeline again
	return bottom;
}

/********************************************************************
 * The clean cache handling function.
 * The function loads a random int into each cacheline
 * to ensure caches are flushed. Note that the entire cache is
 * flushed, including the associative parts.
 *******************************************************************/
#ifdef CACHE_CLEAN

int cleanarray[CACHE_SIZE];      
int xored;

void clean_tables();

void cleancache()
{
	int i;
	clean_tables();
//	for(i=0; i<(CACHE_SIZE); i+= CACHE_LINE){
//		cleanarray[i] = random();
//	}
//	for(i=0; i<(CACHE_SIZE); i+= CACHE_LINE){
//		xored ^= cleanarray[i];
//	}
}
#else
#define cleancache()
#endif


double packets;               /* Count of the number of encryptions done */
double ttotal;                /* Sum of all encryptions times */
double t[16][256];            /* t[x][y] is time taken when plaintextByte[x] = y (all other pt bytes are random) */
double tsq[16][256];          /* square of t[x][y] */
long long tnum [16][256];     /* Number of encryption with pt[x] = y */
double u[16][256];            /* Average encryption time when pt[x] = y */
double udev[16][256];         /* Standard deviation of encryption time when pt[x] = y */




/*******************************************************************
 * updates the database with the encryption timing.
 * timing is the time for the encryption
 * n is the plaintext used for that encryption
 * size is 16
********************************************************************/
void tally(double timing, unsigned char *n, int size)
{
	int j;
	int b;
	for (j = 0;j < size;++j) {
		b = n[j] & 0xff;
		++packets;
		ttotal += timing;
		t[j][b] += timing;
		tsq[j][b] += timing * timing;
		tnum[j][b] += 1;
	}
}

/*******************************************************************
 * initilize the databases and set counters to zero.
********************************************************************/
void init()
{
	int i,j;
	packets = 0;
	ttotal = 0;
	for(i=0; i<16; ++i){
		for(j=0; j<256; ++j){
			t[i][j] = 0;
			tsq[i][j] = 0;
			tnum[i][j] = 0;
			u[i][j] = 0;
		}
	}
}

/*******************************************************************
 * compute the deviation, standard deviation etc, based on the 
 * timing collected, and write to a file.
 * collect -> is a binary 16 byte array. if true then the timing
 *            is computed and for that byte and stored in a file
 * prefix  -> the file where the timings are stored is prefixed by
 *            this string
********************************************************************/
void printpatterns(const char *collect, char *prefix)         
{
	int j;
	int b;
	double taverage;
	char buf[256], filename[64];

	taverage = ttotal / packets;
	for (j = 0;j < 16;++j)
		for (b = 0;b < 256;++b) {
			u[j][b] = t[j][b] / tnum[j][b];
			udev[j][b] = tsq[j][b] / tnum[j][b];
			udev[j][b] -= u[j][b] * u[j][b];
			udev[j][b] = sqrt(udev[j][b]);
		}
	for (j = 0;j < 16;++j) {
		if(collect[j] == FALSE) continue;

		sprintf(filename, "%s/%s%d.log", OUTPUT_DIR, prefix, j);
		FILE *fd = fopen(filename, "w");
		for (b = 0;b < 256;++b) {
			sprintf(buf, "%2d %3d %02x %lld %.3f %.3f %.3f %.6f\n"
					,j
					,b, b
					,tnum[j][b]
					,u[j][b]                         // average time for (j,b)
					,udev[j][b]                      // deviation
					,u[j][b] - taverage              
					,udev[j][b] / sqrt(tnum[j][b])); // sd


			fprintf(fd, "%s", buf);
		}
		fclose(fd);
	}
}

/*******************************************************************
 * Print whenever we double the number of samples
 *******************************************************************/
int timetoprint(long long inputs)
{
	if (inputs < 10000) 
		return 0;
	if (!(inputs & (inputs - 1)))
		return 1;
	return 0;
}




