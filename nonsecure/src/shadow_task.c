
#include "cmsis_os2.h"
#include "nsc.h"
#include "string.h"

#define ESR_CODE __attribute__((section(".esr_code_flash")))

#define ESR_GLOBAL_DATA_FLASH2RAM __attribute__((section(".esr_data_ram")))
#define ESR_GLOBAL_DATA_FLASH     __attribute__((section(".esr_data_flash")))
#define ESR_GLOBAL_DATA_RAM       __attribute__((section(".esr_data_ram_dynamic")))


/*
#################################
Tasks run as Secure Region
#################################
*/

ESR_CODE void rtpox_entry(){
    // Initialize the secure context
    // Initialize the secure context
    while (1) {
        // Wait for the task to be resumed
        osDelay(1000);
    }
    
}


ESR_CODE void rtpox_resume(){
    // Initialize the secure context
    while (1) {
        // Wait for the task to be resumed
        osDelay(1000);
    }
}


ESR_CODE void rtpox_exit_task(){
    // Exit the secure context
    // secure_rtpox_exit();
}


/// <<< APPLICATION STARTS HERE
ESR_GLOBAL_DATA_FLASH2RAM uint32_t mat1[20][20]={1};
ESR_GLOBAL_DATA_FLASH2RAM uint32_t mat2[20][20]={1};
ESR_GLOBAL_DATA_FLASH2RAM uint32_t mat3[20][20]={1};

ESR_CODE void matmul(){
    uint32_t i, j;

    for (i = 0; i < 20; i++) {
        for (j = 0; j < 20; j++) {
            mat3[i][j] += mat1[i][j] * mat2[i][j];
        }
    }
}
/// <<< APPLICATION STOP HERE



/*
#################################
Tasks run as  Non Secure Region
#################################
*/

extern uint32_t __s_esr_data_ram_init;
extern uint32_t __s_esr_data_ram_start;
extern uint32_t __s_esr_data_ram_end;

static void copy_flash_data(void) {
    uint32_t *src = &__s_esr_data_ram_init;  // Flash
    uint32_t *dst = &__s_esr_data_ram_start; // RAM

    while (dst < &__s_esr_data_ram_end) {
        *dst++ = *src++;
    }
}

void rtpox_syscall(){

}

void rtpox_exit(){
    // exit point
    osThreadExit();
}


void rtpox_init_task(void *pvParameters){

    // load flash esr data into ers ram
    copy_flash_data();


    secure_rtpox_init_rtpox_process();
    
}