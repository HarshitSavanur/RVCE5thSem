/*
Author: Gnanambikai

Specific to my system's configurations.
PGSIZE:4KB ,
DCache associativity:8 , 
Cache Block size:64B ,
No of Dcache sets : 64,
Dcache size : 32kB


You will have to tweak the code a bit, if your system has different configurations than above.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define THRESHOLD 1750              /* Threshold above which, timing measurements are ignored */
#define TIME_PERIOD (1<<24)         /* the number of iterations to probe a single bit */
#define CONTENTION_THRESHOLD 17     /* If the timing difference between two sets falls less than this, we assume that nothing has been sent */


/* Cache parameters */
#define BLOCK_OFFSET_BITS 6      /* cache address offset bits */
#define SET_BITS          6      /* set address bits */
#define ASSOC_BITS        3      /* lowest 3 bits of the tag address decides the associativity in L1 cache */
#define TOT_BITS          (BLOCK_OFFSET_BITS + SET_BITS + ASSOC_BITS)

#define DCACHE_SIZE       32768
#define INT_SIZE          4

/* Enable this for verbose outputs */
//#define __DEBUG__

#ifdef __DEBUG__          
#define PRINT(x)     do{printf(x);}while(0); 
#else
#define PRINT(x)
#endif 

#ifdef __DEBUG__          
#define PRINT4(x1,x2,x3,x4)     do{printf(x1, x2, x3, x4);}while(0); 
#else
#define PRINT4(x1,x2,x3,x4)
#endif 




#if defined(__i386__)
static __inline__ unsigned long long rdtsc(void)
{
	unsigned long long int x;
	__asm__ volatile("xorl %%eax,%%eax\n cpuid \n" ::: "%eax", "%ebx", "%ecx", "%edx"); // flush pipeline
	__asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
	__asm__ volatile("xorl %%eax,%%eax\n cpuid \n" ::: "%eax", "%ebx", "%ecx", "%edx"); // flush pipeline
	return x;
}

#elif defined(__x86_64__)
static __inline__ unsigned long long rdtsc(void)
{
	unsigned hi, lo;
	__asm__ __volatile__ ("xorl %%eax,%%eax\n cpuid \n" ::: "%eax", "%ebx", "%ecx", "%edx"); // flush pipeline
	__asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
	__asm__ __volatile__ ("xorl %%eax,%%eax\n cpuid \n" ::: "%eax", "%ebx", "%ecx", "%edx"); // flush pipeline
	return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

#endif  


/* declare an array as large as the L1 D cache; align it to an 
   offset of 64 bytes to make things simple */ 
unsigned int send_array[DCACHE_SIZE / INT_SIZE] __attribute__ ((aligned(64)));

/*
 * main takes 4 command line parameters. Each parameter represents a cache set (0 - 63) and must
 * not overlap. The first two parameters are used for sending bits (0 or 1). The second two 
 * parameters are used for controlling the communication (it takes values even or odd).
 */
int main(int c,char *argv[])
{
	long int set_index_A0 = strtol(argv[1], NULL, 0);   /* This cache set is used to transfer a 0 to receiver */
	long int set_index_A1 = strtol(argv[2], NULL, 0);   /* This cache set is used to transfer a 1 to receiver */ 
	long int set_index_BO = strtol(argv[3], NULL, 0);   /* This set signals the receiver that the bit sent is at an odd index */
	long int set_index_BE = strtol(argv[4], NULL, 0);   /* This set signals the receiver that the bit sent is at an even index */

	long long int A0_0, A0_1, A0_2, A0_3, A0_4, A0_5, A0_6, A0_7;  /* 8 associate cache lines corresponding to set A0 */
	long long int A1_0, A1_1, A1_2, A1_3, A1_4, A1_5, A1_6, A1_7;  /* 8 associate cache lines corresponding to set A1 */
	long long int BO_0, BO_1, BO_2, BO_3, BO_4, BO_5, BO_6, BO_7;  /* 8 associate cache lines corresponding to set BO */
	long long int BE_0, BE_1, BE_2, BE_3, BE_4, BE_5, BE_6, BE_7;  /* 8 associate cache lines corresponding to set B1 */  

	long int send_start = (long long int) send_array; /* address of the send_array */


	long int y_start;

	unsigned long long start, end; /* Used for timing measurements */


	/* Extracting the bits (14 - 12) of the starting address of the array. 
           These bit represent the associativity */
	y_start = (send_start >> (SET_BITS + BLOCK_OFFSET_BITS)) & ((1<<ASSOC_BITS)-1) ; 



	/* Form the offsets to be accessed */
	// Construct the VA for sending bit 0
	A0_0=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | y_start ) << SET_BITS ) | set_index_A0 ) << BLOCK_OFFSET_BITS;  
	A0_1=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+1) ) << SET_BITS ) | set_index_A0 ) << BLOCK_OFFSET_BITS;  
	A0_2=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+2) ) << SET_BITS ) | set_index_A0 ) << BLOCK_OFFSET_BITS;  
	A0_3=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+3) ) << SET_BITS ) | set_index_A0 ) << BLOCK_OFFSET_BITS;  
	A0_4=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+4) ) << SET_BITS ) | set_index_A0 ) << BLOCK_OFFSET_BITS;  
	A0_5=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+5) ) << SET_BITS ) | set_index_A0 ) << BLOCK_OFFSET_BITS;  
	A0_6=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+6) ) << SET_BITS ) | set_index_A0 ) << BLOCK_OFFSET_BITS;  
	A0_7=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+7) ) << SET_BITS ) | set_index_A0 ) << BLOCK_OFFSET_BITS;  

	// Construct the VA for sending bit 1
	A1_0=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | y_start ) << SET_BITS ) | set_index_A1 ) << BLOCK_OFFSET_BITS;  	
	A1_1=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+1) ) << SET_BITS ) | set_index_A1 ) << BLOCK_OFFSET_BITS;  
	A1_2=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+2) ) << SET_BITS ) | set_index_A1 ) << BLOCK_OFFSET_BITS;  
	A1_3=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+3) ) << SET_BITS ) | set_index_A1 ) << BLOCK_OFFSET_BITS;  
	A1_4=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+4) ) << SET_BITS ) | set_index_A1 ) << BLOCK_OFFSET_BITS;  
	A1_5=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+5) ) << SET_BITS ) | set_index_A1 ) << BLOCK_OFFSET_BITS;  
	A1_6=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+6) ) << SET_BITS ) | set_index_A1 ) << BLOCK_OFFSET_BITS;  
	A1_7=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+7) ) << SET_BITS ) | set_index_A1 ) << BLOCK_OFFSET_BITS;  


	// Construct the VA for sending bit the odd bit
	BO_0=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | y_start ) << SET_BITS ) | set_index_BO ) << BLOCK_OFFSET_BITS;   
	BO_1=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+1) ) << SET_BITS ) | set_index_BO ) << BLOCK_OFFSET_BITS;  
	BO_2=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+2) ) << SET_BITS ) | set_index_BO ) << BLOCK_OFFSET_BITS;  
	BO_3=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+3) ) << SET_BITS ) | set_index_BO ) << BLOCK_OFFSET_BITS;  
	BO_4=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+4) ) << SET_BITS ) | set_index_BO ) << BLOCK_OFFSET_BITS;  
	BO_5=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+5) ) << SET_BITS ) | set_index_BO ) << BLOCK_OFFSET_BITS;  
	BO_6=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+6) ) << SET_BITS ) | set_index_BO ) << BLOCK_OFFSET_BITS;  
	BO_7=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+7) ) << SET_BITS ) | set_index_BO ) << BLOCK_OFFSET_BITS;  

	// Construct the VA for sending bit the even bit
	BE_0=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | y_start ) << SET_BITS ) | set_index_BE ) << BLOCK_OFFSET_BITS;   
	BE_1=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+1) ) << SET_BITS ) | set_index_BE ) << BLOCK_OFFSET_BITS;  
	BE_2=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+2) ) << SET_BITS ) | set_index_BE ) << BLOCK_OFFSET_BITS;  
	BE_3=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+3) ) << SET_BITS ) | set_index_BE ) << BLOCK_OFFSET_BITS;  
	BE_4=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+4) ) << SET_BITS ) | set_index_BE ) << BLOCK_OFFSET_BITS;  
	BE_5=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+5) ) << SET_BITS ) | set_index_BE ) << BLOCK_OFFSET_BITS;  
	BE_6=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+6) ) << SET_BITS ) | set_index_BE ) << BLOCK_OFFSET_BITS;  
	BE_7=((((( send_start >> TOT_BITS ) << ASSOC_BITS ) | (y_start+7) ) << SET_BITS ) | set_index_BE ) << BLOCK_OFFSET_BITS;  

	int i =0;
	double A0_freq[THRESHOLD], BO_freq[THRESHOLD];   
	double A1_freq[THRESHOLD], BE_freq[THRESHOLD];   
	int sender_started = 0;
	int a_bit, b_bit, prev_b_bit = 1;
	int counter = -1;
	while(1)
	{
		
		memset(A0_freq, 0, sizeof(A0_freq));	
		memset(A1_freq, 0, sizeof(A1_freq));	
		memset(BO_freq, 0, sizeof(BO_freq));	
		memset(BE_freq, 0, sizeof(BE_freq));	

		int a0_max=0 , a1_max=0;
		int bo_max=0 , be_max=0;


		for(i=0; i < TIME_PERIOD; i++)
		{ 

			__asm__ __volatile__("");
			// time an access to the BO cache set
			start = rdtsc();
			*(unsigned int*)BO_0 = i; 
			*(unsigned int*)BO_1 = i+1; 
			*(unsigned int*)BO_2 = i+2; 
			*(unsigned int*)BO_3 = i+3; 
			*(unsigned int*)BO_4 = i+4; 
			*(unsigned int*)BO_5 = i+5; 
			*(unsigned int*)BO_6 = i+6; 
			*(unsigned int*)BO_7 = i+7; 
			end = rdtsc();

			if(end - start <= THRESHOLD) BO_freq[(end - start)]++;

			// time an access to the BE cache set
			start = rdtsc();
			*(unsigned int*)BE_0 = i; 
			*(unsigned int*)BE_1 = i+1; 
			*(unsigned int*)BE_2 = i+2; 
			*(unsigned int*)BE_3 = i+3; 
			*(unsigned int*)BE_4 = i+4; 
			*(unsigned int*)BE_5 = i+5; 
			*(unsigned int*)BE_6 = i+6; 
			*(unsigned int*)BE_7 = i+7; 
			end = rdtsc();

			if(end - start <= THRESHOLD) BE_freq[(end - start)]++;
			
			
			// time an access to the A0 cache set
			start = rdtsc();
			*(unsigned int*)A0_0 = i; 
			*(unsigned int*)A0_1 = i+1; 
			*(unsigned int*)A0_2 = i+2; 
			*(unsigned int*)A0_3 = i+3; 
			*(unsigned int*)A0_4 = i+4; 
			*(unsigned int*)A0_5 = i+5; 
			*(unsigned int*)A0_6 = i+6; 
			*(unsigned int*)A0_7 = i+7; 
			end = rdtsc();

			if(end - start <= THRESHOLD) A0_freq[(end - start)]++;


			// time an access to the A0 cache set
			start = rdtsc();
			*(unsigned int*)A1_0 = i; 
			*(unsigned int*)A1_1 = i+1; 
			*(unsigned int*)A1_2 = i+2; 
			*(unsigned int*)A1_3 = i+3; 
			*(unsigned int*)A1_4 = i+4; 
			*(unsigned int*)A1_5 = i+5; 
			*(unsigned int*)A1_6 = i+6; 
			*(unsigned int*)A1_7 = i+7; 
			end = rdtsc();

			if(end - start <= THRESHOLD) A1_freq[(end - start)]++;




		}
		
		/* so far we have obtained four distributions. A0_freq; A1_freq; B0_freq; B1_freq
		Each distribution has the frequency with which a given clock cycle is observed.
		We next determine the peak in each distribution -- this indicates the timing that occured most often */

		for(i=0;i<THRESHOLD;i++)
			if(A0_freq[a0_max] < A0_freq[i])
				a0_max = i;

		for(i=0;i<THRESHOLD;i++)
			if(A1_freq[a1_max] < A1_freq[i])
				a1_max = i;

		for(i=0;i<THRESHOLD;i++)
			if(BO_freq[bo_max] < BO_freq[i])
				bo_max = i;

		for(i=0;i<THRESHOLD;i++)
			if(BE_freq[be_max] < BE_freq[i])
				be_max = i;

		PRINT4("\t\t\tReceiver: %d %d d=%d\t \n", a0_max, a1_max, a0_max - a1_max);
		PRINT4("\t\t\tReceiver: %d %d d=%d\t \n", bo_max, be_max, bo_max - be_max);

		/* This if condition determines if the sender has started. If the sender has
		not yet started, there will be little difference (< CONTENTION_THRESHOLD) between
		the be_max and bo_max as well as a0_max and a1_max. We just continue in such a case. */
		if (!sender_started && (abs(be_max - bo_max) < CONTENTION_THRESHOLD) && 
                    (abs(a0_max - a1_max) < CONTENTION_THRESHOLD)){ 
			PRINT("\t\t\t Sender not started (no contention detected)\n");
			continue;	
		}
		sender_started = 1;
	
		/* extract if this is an even bit or odd bit */
		b_bit = (be_max > bo_max)? 0: 1;
	
		/* determine if a next bit has been received */
		if (prev_b_bit == b_bit){
			PRINT("\t\t\t No change in b_bit\n");
		}else{
			prev_b_bit = b_bit;
			counter++;
		}

		/* extract the bit sent (either 0 or 1) */
		a_bit = (a1_max > a0_max)? 1: 0;

		/* print it out */		
		printf("%d: Reciever: %d (%s)\n", counter, a_bit, (b_bit)?"odd":"even");		

	}

	return 0;
}
