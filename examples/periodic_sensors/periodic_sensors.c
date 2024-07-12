/* PMSIS includes */
#include "pmsis.h"
#ifdef GAP_SDK
#define AXI_BASE 0x80000000
#else
#define AXI_BASE 0x20000000
#endif

#define NUM_ITERS 2880      // 1 day
#define PERIOD_US 3000000   // 30 sec
#define PERIOD_HR 0.00833333

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS HelloWorld ***\n\n");
    printf("Entering main controller\n");

    uint32_t errors = 10;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();

    int *mic_click_sensor = (volatile int *)AXI_BASE + 0x0;
    for (int i = 0; i < NUM_ITERS; i++)
    {
        printf("[ITER %d/%d] %.2f Hours Elapsed\n", i, NUM_ITERS, i * PERIOD_HR);
        
        // Random placeholder computation
        *mic_click_sensor = (i + 1) * 2;

        // Periodic Wait
        pi_time_wait_us(PERIOD_US);
    }

    return errors;
}