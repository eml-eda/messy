/* PMSIS includes */
#include "pmsis.h"
#ifdef GAP_SDK
#define AXI_BASE 0x80000000
#else
#define AXI_BASE 0x20000000
#endif
#define NUM_ITERS 2

/* Program Entry. */

int main(void)
{
    // printf("\n\n\t *** PMSIS HelloWorld ***\n\n");
    // printf("Entering main controller\n");
    uint32_t errors = 10;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();

    uint8_t* mic_click_sensor = (volatile int *)AXI_BASE+0x0;
    for(int i=0;i<NUM_ITERS;i++){
        *(mic_click_sensor + i) = i + 10;
        printf("Microphone sensor %d\n", *(mic_click_sensor + i));
    }

    return errors;
}