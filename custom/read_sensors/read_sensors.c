#include <stdio.h>
#include "pmsis.h"
/* Program Entry. */
int main(void)
{
    printf("\n\t *** PMSIS HelloWorld ***\n\n");
    printf("Entering main controller\n");

    uint32_t errors = 10;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    printf("[%d %d] Hello World!\n", cluster_id, core_id);

    uint32_t last_value = *(volatile int *)0x1c000000;

    int* air_quality = (volatile int *)0x80000000;
    // int* temperature = (volatile int *)0x80000004;
    // int* mick_click  = (volatile int *)0x8000000C;
    // int* methane     = (volatile int *)0x80000008;
    // int* radio_freq  = (volatile int *)0x80000010;

    while(1){
        printf("Air_quality Read %d\n", *air_quality);
        // printf("Temperature Read %d\n", *temperature);
        // printf("Methane Read %d\n", *methane);
        // printf("MicroPhone Read %d\n", *mick_click);
        // printf("Radio Frequency Transmission %d\n", *radio_freq);
    }

    return errors;
}
