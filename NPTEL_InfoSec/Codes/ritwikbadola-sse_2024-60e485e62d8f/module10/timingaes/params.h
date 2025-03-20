/******************************************************************************
 * File	Name			: params.h
 * Organization         : Indian Institute of Technology Kharagpur
 * Project Involved		: Timing attack on Clefia
 * Author		    	: Chester Rebeiro 
 * Date of Creation		: 1/Aug/2009
 * Date of freezing		: 7/Dec/2009
 * Log Information regading 
 * maintanance			:
 * Synopsis			    : Confiugrations and other constant parameters
 *
 * Copyright(c) 2009 NTT, Corp. All Rights Reserved.
 ******************************************************************************/

#ifndef _PARAMS_H_
#define _PARAMS_H_
 
#define TRUE  1         
#define FALSE 0

#define RK4   4
#define RK5   5
#define RK0RK1 1
#define RK2RK3 2

#define CACHE_CLEAN 1             /* Define if cache cleaning between encryption is required */

#ifdef CACHE_CLEAN
#define CACHE_SIZE ((32*1024)/4)  /* cache size in terms of 32 bit words */
#define CACHE_LINE (64/4)         /* cache line size in terms of 32 bit words */
#endif

/* The output directory to store the statistics */
#define OUTPUT_DIR   "out/timing"

/* 
 * IMP: This should set depending on the machine. 
 * To set this, run several times for on the target machine
 * and find the average. Set around 1.2 to 1.3 times higher than
 * average. 
 */
//#define TIME_THRESHOLD 		6500        /* Intel Core 2 */
//#define TIME_THRESHOLD 		45000        /* Intel Core 2 */
//#define TIME_THRESHOLD 	18500        /* Intel Pentium 4 */


/* Function Prototypes */
void tally(double timing, unsigned char *n, int size);
void printpatterns(const char *collect, char *prefix);
int timetoprint(long long inputs);


#endif
