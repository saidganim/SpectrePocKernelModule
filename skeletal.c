#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>

#define RAND(i) ((i * 167) + 13) & 0xff   // To beat the prefetcher
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

char KernelAddress[21]={'\0'};           // We store the value returned from the kernel module
uint8_t array[1337]= {100};             
int size_array = 1337;
int cacheThreshold= 0;
char address = 0x99;                     //Variable used to find the CacheThreshold
char secretByte =0x00 ;                  //The retrieved secretByte
char * probe = NULL ;


//Timing function
static inline uint64_t rdtsc() {
  uint64_t a = 0, d = 0;
  asm volatile("mfence");
  asm volatile("rdtscp" : "=a"(a), "=d"(d) :: "rcx");
  a = (d << 32) | a;
  asm volatile("mfence");
  return a;
}
/*
    Call this funtion to get the Address of our secretByte in the kernel space and to cache it 
*/

void __attribute__((optimize("O0")))getKernelAddress(){
    int readval = open("/proc/leakSecretByteAddress",O_RDONLY);
    int r = read (readval,KernelAddress,18);
    close(readval);
}

//function to get the cache threshold
void __attribute__((optimize("O0")))getCacheThreshold(){
    int64_t start , end;
    *(volatile char *)(&address);
	start = rdtsc();
	*(volatile char *)(&address);
	end=rdtsc();
	cacheThreshold = end-start + 30;
}

__attribute__((always_inline))
inline void flush(const char *adrs)
{
  asm __volatile__ (
     "mfence         \n"
     "clflush (%0)  \n"
     :
     : "r" ((volatile void *)adrs)
     : "memory"
  );
}
volatile int  __attribute__((optimize("O0")))speculativeexec(volatile size_t a){

        if (a < size_array){
        /*
                Do speculative exec here
        */
        
    }
     return 1;
}

int __attribute__((optimize("O0")))  main(int argc, char const *argv[])
{
    srand(time(NULL));
    getKernelAddress(); 
    printf("\nValue read from the kernel module is " RED "0x%s" RESET,KernelAddress);
    getCacheThreshold();
    printf("The cacheThreshold is " GREEN "%d \n" RESET,cacheThreshold);
    
    /*
        Now we create a Probe array of size 4096 *256 
        ie. 1 page for every ascii char 

    */

	probe =  mmap(NULL, 4096 * 256 , PROT_READ | PROT_WRITE,\
     MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    memset(probe,0x44,4096*256);

    void * kernelAddress = (void*)strtoul(KernelAddress, NULL, 16);
    uintptr_t arrayInt          = (uintptr_t)array;
    uintptr_t kernelAddressInt  = (uintptr_t)kernelAddress;
    size_t offset = kernelAddressInt - arrayInt ; // Offset of the SecretByte from out array `array`

// The Part below is the code that does speculative read
// We can do this 5 time for a the secret byte to remove the noise 

    /*
        Now we train the BPU to take the branch 
    */

    for(volatile int i = 0 ; i < 1000 ; i++) {
				
                // pass values to fn  speculativeexec() here to train the BPU into taking if
		
    }

    /*
        Now we flush probe 
        probe + 4096 * k , all the 256 byte lie on different pages in order to beat the prefetcher 
    */
    for (volatile int k = 0; k < 256; ++k) flush((const char*)probe + 4096 * k) ;

    /*
        Now we pass the offset to the speculativeexec() so that the BPU thinks the if evaluates to true
        and speculatively execute the instructions inside it, till it reads the value of size_array from the DRAM
    */

    flush((const char *)&size_array);
    char temp; //Just a temporary variable
    getKernelAddress(); // To have the secret byte in the cache so that it can be retrieved faster than size_array
    speculativeexec(???); // pass the offset here 

    /*
        Now since the secret byte is used to access probe use Flush+reload to get the secret byte here.
    */

    
    




    
    return 0;
}
