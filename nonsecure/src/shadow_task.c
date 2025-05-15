
#include "cmsis_os2.h"
#include "nsc.h"
#include "string.h"
#include "_rp2350.h"
#include "hardware/structs/scb.h"
#define ESR_CODE __attribute__((section(".esr_code_flash")))
#define ESR_GLOBAL_DATA_FLASH2RAM __attribute__((section(".esr_data_ram")))
#define ESR_GLOBAL_DATA_FLASH     __attribute__((section(".esr_data_flash")))
#define ESR_GLOBAL_DATA_RAM       __attribute__((section(".esr_data_ram_dynamic")))

/*
#################################
Tasks run as Secure Region
#################################
*/

ESR_GLOBAL_DATA_RAM uint32_t rtpox_esr_psp_reserve[256];
ESR_GLOBAL_DATA_RAM __ALIGNED(128) uint32_t rtpox_esr_vtr_reserve[64];

/// <<< APPLICATION STARTS HERE

ESR_GLOBAL_DATA_FLASH2RAM uint32_t mat1[20][20]={1};
ESR_GLOBAL_DATA_FLASH2RAM uint32_t mat2[20][20]={1};
ESR_GLOBAL_DATA_FLASH2RAM uint32_t mat3[20][20]={1};

ESR_CODE void matmul(){
    uint32_t i, j;

    uint32_t var = scb_hw->vtor;

    for (i = 0; i < 2000000; i++) mat3[0][0]++;
    
    for (i = 0; i < 20; i++) {
        for (j = 0; j < 20; j++) {
            mat3[i][j] += mat1[i][j] * mat2[i][j];
        }
    }
}
/// <<< APPLICATION STOP HERE


ESR_CODE void rtpox_esr_entry(){
    // call the main function of the application
    matmul();
    
    // Call the secure function to exit 
    secure_rtpox_exit_rtpox_process();
}

/*
#################################
Tasks run as  Non Secure Region
#################################
*/
__USED volatile uint32_t rtpox_st_tmp_reserved[40];

extern uint32_t __s_esr_data_ram_init;
extern uint32_t __s_esr_data_ram_start;
extern uint32_t __s_esr_data_ram_end;

static void copy_flash_data_to_ram(void) {
    uint32_t *src = &__s_esr_data_ram_init;  // Flash
    uint32_t *dst = &__s_esr_data_ram_start; // RAM

    while (dst < &__s_esr_data_ram_end) {
        *dst++ = *src++;
    }
}


void rtpox_syscall_dispatch(){
}


#define SECURE_TRIGGER_RTPOX_RESUME 0xF0F01111
__USED void  rtpox_trig_resume(){
    // call the function with address SECURE_TRIGGER_RTPOX_RESUME
    ((void (*)(void))((volatile uintptr_t)SECURE_TRIGGER_RTPOX_RESUME))();

}

 

void rtpox_init_task(void *pvParameters){

    // USELESS ---- just to the compiler dont ignore
    rtpox_st_tmp_reserved[0]=1;
    if (rtpox_st_tmp_reserved[0] == 0 ){
        rtpox_trig_resume();
        rtpox_syscall_dispatch();
    }
    // END USELESS

    // load flash esr data into ers ram
    copy_flash_data_to_ram();

    secure_rtpox_init_rtpox_process();

    osThreadExit(); // It is not supposed to reach here
    
    
    
}