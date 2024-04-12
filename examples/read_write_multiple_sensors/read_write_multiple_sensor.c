/* PMSIS includes */
#include "pmsis.h"
#ifdef GAP_SDK
#define AXI_BASE 0x80000000
#else
#define AXI_BASE 0x20000000
#endif
#define NUM_ITERS 10
/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS HelloWorld ***\n\n");
    printf("Entering main controller\n");

    uint32_t errors = 10;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    printf("[%d %d] Hello World!\n", cluster_id, core_id);

    int* mic_click_sensor = (volatile int *)AXI_BASE+0x0;
    int* mic_click_sensor2 = (volatile int *)AXI_BASE+256;

    for(int i=0;i<NUM_ITERS;i++){
        printf("Iter %d\n", i);
        printf("Microphone sensor %d\n", *mic_click_sensor);
        *(mic_click_sensor2 + i) = i;
    }
    
    for(int i=0; i<NUM_ITERS; i++){
        printf("Iter_puppo %d\n", i);
        printf("Microphone sensor 2 %d\n", *(mic_click_sensor2 + i));
    }

    return errors;
}