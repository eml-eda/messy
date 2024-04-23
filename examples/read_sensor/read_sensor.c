/* PMSIS includes */
#include "pmsis.h"
#ifdef GAP_SDK
#define AXI_BASE 0x80000000
#else
#define AXI_BASE 0x20000000
#endif
#define MEMORY_SENSOR_SIZE 256
#define STRIDE_ADDRESSES 1
/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS HelloWorld ***\n\n");
    printf("Entering main controller\n");

    uint32_t errors = 10;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    printf("[%d %d] Hello World!\n", cluster_id, core_id);

    int* mic_click_sensor = (volatile int *)AXI_BASE+0x0;
    printf("Messy sensor test, there should be %d iters\n\n",(MEMORY_SENSOR_SIZE)/STRIDE_ADDRESSES);
    for(int i=0;i<MEMORY_SENSOR_SIZE;i+=STRIDE_ADDRESSES){
        printf("iter n.%d\n",i/STRIDE_ADDRESSES);
        *((uint8_t*)(mic_click_sensor+i))=i%256;
        printf("Sensor read #%d : %d\n",i, *((uint8_t*)(mic_click_sensor+i)));
    }

    return errors;
}
