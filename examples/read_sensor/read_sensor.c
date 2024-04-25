/* PMSIS includes */
#include "pmsis.h"
#ifdef GAP_SDK
#define AXI_BASE 0x80000000
#else
#define AXI_BASE 0x20000000
#endif
#define MEMORY_SENSOR_SIZE 256
#define ITER_SIZE 128

typedef struct messy_ex_t{
    uint8_t idx;
    uint64_t val;
}messy_ex;

int main(void)
{
    printf("\n\n\t *** PMSIS HelloWorld ***\n\n");
    printf("Entering main controller\n");

    uint32_t errors = 10;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    printf("[%d %d] Hello World!\n", cluster_id, core_id);

    uint8_t* sensor_addr = AXI_BASE+0x0;
    printf("\n\nMessy uint8 sensor test, there should be %d iters\n\n",MEMORY_SENSOR_SIZE);
    for(int i=0;i<MEMORY_SENSOR_SIZE;i++){
        *((uint8_t*)sensor_addr+i)=i%256;
        printf("[uint8]\t%d : %d\n",i, *((uint8_t*)sensor_addr+i));
    }

    sensor_addr = AXI_BASE+0x0;
    printf("\n\nMessy memcpy sensor test, there should be %d iters\n\n",((int)(MEMORY_SENSOR_SIZE/ITER_SIZE)));
    uint8_t iter_data[ITER_SIZE];
    for(int i=0;i<ITER_SIZE;i++) iter_data[i]=i%256;
    for(int i=0;(i+ITER_SIZE)<=MEMORY_SENSOR_SIZE;i+=ITER_SIZE){
        memcpy(sensor_addr+i,iter_data,ITER_SIZE);
        printf("[memcpy]\t%d : %d\n",i, *((uint8_t*)sensor_addr+i));
    }

    int num_examples=MEMORY_SENSOR_SIZE/sizeof(messy_ex);
    printf("\n\nMessy examples with a struct, there should be %d iters\n\n",num_examples);
    messy_ex* messy_sensor=AXI_BASE+0x0;
    for(uint8_t i=0;i<num_examples;i++){
        messy_ex ex=(messy_ex){.idx=i,.val=i<<1};
        *messy_sensor=ex;
        printf("[struct]\t%d : %d\n", messy_sensor->idx,messy_sensor->val);
        messy_sensor++;
    }

    return errors;
}
