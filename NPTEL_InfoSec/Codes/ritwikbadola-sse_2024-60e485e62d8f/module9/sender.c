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
#include <time.h>

#define TIME_PERIOD (1<<24)      /* the number of iterations to send a single bit */

/* Cache parameters */
#define BLOCK_OFFSET_BITS 6      /* cache address offset bits */
#define SET_BITS          6      /* set address bits */
#define ASSOC_BITS        3      /* lowest 3 bits of the tag address decides the associativity in L1 cache */
#define TOT_BITS          (BLOCK_OFFSET_BITS + SET_BITS + ASSOC_BITS)

#define DCACHE_SIZE       32768
#define INT_SIZE          4
#define DATASIZE          10

#define EPOCH             128ULL

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
	unsigned long long start, end;


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

	unsigned long long i=0 , k=0;
	float time = 0;
	unsigned int data[DATASIZE]={0,1,1,0, 1, 1, 0, 0, 0, 1};       
	long long int tA_0 , tA_1 , tA_2, tA_3, tA_4, tA_5, tA_6, tA_7; 
	long long int tB_0 , tB_1 , tB_2, tB_3, tB_4, tB_5, tB_6, tB_7; 


	printf("---------Starting to send --------\n");
	while( k < DATASIZE)
	{

		/* set tA_* to the bit to be sent (0 or 1) */
		printf("%d : ", k);
		if(data[k] == 0) {
			tA_0 = A0_0; tA_1 = A0_1; tA_2 = A0_2; tA_3 = A0_3; 
			tA_4 = A0_4; tA_5 = A0_5; tA_6 = A0_6; tA_7 = A0_7;                                        
			printf("Sender : Tx 0 ");
		}
		else {

			tA_0 = A1_0; tA_1 = A1_1; tA_2 = A1_2; tA_3 = A1_3; 
			tA_4 = A1_4; tA_5 = A1_5; tA_6 = A1_6; tA_7 = A1_7;                                        
			printf("Sender : Tx 1 ");
		}

		/* set tB_* to the bit to the control (E or O bit) */
		if(k & 1) {
			tB_0 = BO_0; tB_1 = BO_1; tB_2 = BO_2; tB_3 = BO_3; 
			tB_4 = BO_4; tB_5 = BO_5; tB_6 = BO_6; tB_7 = BO_7;                                        
			printf("Bit : odd \n");
		}
		else {

			tB_0 = BE_0; tB_1 = BE_1; tB_2 = BE_2; tB_3 = BE_3; 
			tB_4 = BE_4; tB_5 = BE_5; tB_6 = BE_6; tB_7 = BE_7;                                        
			printf("Bit : even  \n");
		}


		/* Note the 128ULL. This is required because the sender needs to be made
                   much slower than the receiver */
		for(i=0; i< EPOCH*TIME_PERIOD; i++){ // just access
			__asm__ __volatile__("");
			// load/store operations to send a 0 or a 1
			*(unsigned int*)tA_0 = i; 
			*(unsigned int*)tA_1 = i+1; 
			*(unsigned int*)tA_2 = i+2; 
			*(unsigned int*)tA_3 = i+3; 
			*(unsigned int*)tA_4 = i+4; 
			*(unsigned int*)tA_5 = i+5; 
			*(unsigned int*)tA_6 = i+6; 
			*(unsigned int*)tA_7 = i+7; 

			// load/store operations to send if the bit sent is at an odd or even index
			*(unsigned int*)tB_0 = i; 
			*(unsigned int*)tB_1 = i+1; 
			*(unsigned int*)tB_2 = i+2; 
			*(unsigned int*)tB_3 = i+3; 
			*(unsigned int*)tB_4 = i+4; 
			*(unsigned int*)tB_5 = i+5; 
			*(unsigned int*)tB_6 = i+6; 
			*(unsigned int*)tB_7 = i+7; 

		}
		k++;
	}
	return 0;
}
