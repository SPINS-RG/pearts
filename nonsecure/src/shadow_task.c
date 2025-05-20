
#include "FreeRTOS.h" 
#include "cmsis_os2.h"
#include "task.h"

#include "nsc.h"
#include "string.h"
#include "_rp2350.h"
#include "hardware/structs/scb.h"
#define ESR_CODE __attribute__((section(".esr_code_flash")))
#define ESR_GLOBAL_DATA_FLASH2RAM __attribute__((section(".esr_data_ram")))
#define ESR_GLOBAL_DATA_FLASH     __attribute__((section(".esr_data_flash")))
#define ESR_GLOBAL_DATA_RAM       __attribute__((section(".esr_data_ram_dynamic")))

typedef enum {
    SC_NONE = -1,
    SC_vTaskDelay= 0xF123,
} esr_syscall_id_t;

typedef struct 
{
    uint8_t size;
    uint32_t args[4];
} esr_syscall_arg_t;

#define SECURE_TRIGGER_RTPOX_RESUME 0xF0F0F0E0
#define SECURE_TRIGGER_RTPOX_SYSCALL 0xF0F0F0EF
#define SECURE_TRIGGER_RTPOX_SYSCALL_RETURN 0xF0F0F0E1

__force_inline void __syscall(esr_syscall_id_t syscall_id, esr_syscall_arg_t *args) {
    __asm volatile (
        "push {r0, r1, r2,r3,r4,r5,r6,r7}\n"
        "mov r0, %0\n"
        "mov r1, %1\n"
        "ldr r2, =%2\n"
        "ldr r2,[r2]\n"
        "pop {r0, r1, r2,r3,r4,r5,r6,r7}\n"
        :
        : "r"(syscall_id), "r"(args), "i"(SECURE_TRIGGER_RTPOX_SYSCALL)
        : "r0", "r1", "r2", "lr"
    );
}

/*
#################################
Tasks run as ESR Region
#################################
*/

ESR_GLOBAL_DATA_RAM __ALIGNED(4) uint32_t rtpox_esr_psp_reserve[256];
ESR_GLOBAL_DATA_RAM __ALIGNED(128) uint32_t rtpox_esr_vtr_reserve[64];

/// <<< APPLICATION STARTS HERE

ESR_GLOBAL_DATA_FLASH2RAM uint32_t mat1[20][20]={1};
ESR_GLOBAL_DATA_FLASH2RAM uint32_t mat2[20][20]={1};
ESR_GLOBAL_DATA_FLASH2RAM uint32_t mat3[20][20]={1};

ESR_CODE void matmul(){
    uint32_t i, j;

    // 0x200633bc
    uint32_t var = scb_hw->vtor;

    // call systemcall osDelay
    esr_syscall_arg_t args;
    args.size = 1;
    args.args[0] = 1; // delay 
    __syscall(SC_vTaskDelay, &args);
    //

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
__USED __attribute__((aligned(4))) volatile uint32_t rtpox_st_tmp_reserved[40];

extern uint32_t __s_esr_data_ram_init;
extern uint32_t __s_esr_data_ram_start;
extern uint32_t __s_esr_data_ram_end;

static void copy_flash_data_to_ram(void) {
    // this function just copy the esr data from flash to ram since the standard reset handler dont do it for this new section that we created
    // it dont need to be called here, it can be added to the reset handler
    uint32_t *src = &__s_esr_data_ram_init;  // Flash
    uint32_t *dst = &__s_esr_data_ram_start; // RAM

    while (dst < &__s_esr_data_ram_end) {
        *dst++ = *src++;
    }
}

__attribute__((naked)) void rtpox_trig_resume(void) {
    // force a secure fault
    __asm volatile (
        "ldr r0, =%0\n"
        // "bx r0\n"
        "ldr r0, [r0]\n"    
        :
        : "i"(SECURE_TRIGGER_RTPOX_RESUME)
        : "r0"
    );
}

void rtpox_sc_dispatch(esr_syscall_id_t systemcall_id, esr_syscall_arg_t * args) {
    // Dispatch the system call to the appropriate handler
    if (systemcall_id==SC_NONE) {
        return;
    }

    switch (systemcall_id) {
        case SC_vTaskDelay:
            // Call the osDelay function with the provided arguments
            vTaskDelay(args->args[0]);
            break;

        // Add more system calls as needed
        default:
            // Handle unknown system call
            break;
    }


    // syscall return
        // force a secure fault
    __asm volatile (
        "ldr r0, =%0\n"
        // "bx r0\n"
        "ldr r0, [r0]\n"    
        :
        : "i"(SECURE_TRIGGER_RTPOX_SYSCALL_RETURN)
        : "r0"
    );

}



 

void rtpox_init_task(void *pvParameters){   

    // USELESS ---- just to the compiler dont ignore
    rtpox_st_tmp_reserved[0]=1;
    if (rtpox_st_tmp_reserved[0] == 0 ){
        rtpox_trig_resume();
        rtpox_sc_dispatch(SC_NONE, NULL);
    }
    // END USELESS

    // load flash esr data into ers ram
    copy_flash_data_to_ram();

    secure_rtpox_init_rtpox_process();

    osThreadExit(); // It is not supposed to reach here
    
}

