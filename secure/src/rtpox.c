
#include <stdint.h>
#include <string.h>
#include "rtpox_port.h"
#include "_rp2350.h"

#define ESR_FLASH_MEMORY_SIZE 0x1000 // Size of the elastic secure region
#define ESR_FLASH_MEMORY_START 0x20000000 // Start address of the elastic secure region

#define ESR_RAM_MEMORY_SIZE 0x1000 // Size of the elastic secure region in RAM
#define ESR_RAM_MEMORY_START 0x20001000 // Start address of the elastic secure region in RAM

#define PASSWORD_INIT 0xFF22AA22 // Initial password to test if the context is initialized

// STATUS
#define ACTIVE   0x2f2f2f2
#define INACTIVE 0

typedef enum {
    FALSE = 0,
    TRUE = 1
} boolean_t;


typedef enum {
    RTPOX_ALREADY_ACTIVE,
    RTPOX_SUCESS,
} rtpox_error_t;


typedef uint32_t rtpox_status_t;

typedef struct {
    int dummy; // Placeholder for actual structure members
} rtpox_shadow_task_t;


typedef enum ACTIVE_VTOR{
    NS_VTOR = 0,
    ESR_VTOR = 1
} active_vtor_t;

typedef struct {
    uint32_t *      NS_VTOR;
    uint32_t *      ESR_VTOR; 
    uint32_t *      esr_task_psp;
    active_vtor_t   active_vtor;
    uint32_t        status;
    uint32_t        init_password;
} rtpox_context_t;

typedef struct {
    int dummy; // Placeholder for actual structure members
} rtpox_esr_t;



typedef struct {
    uint32_t clock;
    uint32_t status[3];
}   rtpox_time_record_t;
    

typedef struct{
    char key[16]; // Placeholder for actual key ->>> not safe 
    char binary_signature[16];
    rtpox_time_record_t time_record[20];
    char hash_binary[32];
} rtpox_report_t;

rtpox_context_t rtpox_context;


rtpox_report_t report;

void rtpox_esr_reset_handler(void);

/*
#########################################
Code related to generic functionality
#########################################
*/

void my_trap(const char *message) {
    while(1){}
}

boolean_t my_assert(int condition, const char *message) {
    if (!condition) {
        my_trap(message);
    }
    return TRUE;
}

/*
#########################################
Code related to clean or initialize structs
#########################################
*/

void rtpox_init_esr_struct(void){
}

void rtpox_reset_rtpox_context_struct(void){
    rtpox_context.status = FALSE; // Initialize the status to inactive
    rtpox_context.init_password = PASSWORD_INIT; // set password to initial value
}

void rtpox_reset_handler(){
    // Reset the RTP over RTSP context
    rtpox_esr_reset_handler();
    rtpox_init_esr_struct();
    rtpox_reset_rtpox_context_struct();
}

/*
#########################################
Code related to binary attestation
#########################################
*/

uint32_t rtpox_attest_esr(void){

    if (rtpox_hash_init()) {
        return 1; // Error initializing hash
    }

    // Hash the context
    rtpox_hash_update((uint8_t *)ESR_FLASH_MEMORY_START, ESR_FLASH_MEMORY_SIZE);
    rtpox_hash_update((uint8_t *)ESR_RAM_MEMORY_START, ESR_RAM_MEMORY_SIZE);

    // Finalize the hash
    rtpox_hash_finalize((char*)report.hash_binary);

    return 0; 
}

/*
#########################################
Code related to the elastic secure region control
#########################################
*/

boolean_t rtpox_is_esr(uint32_t address){
    // Check if the address is within the elastic secure region
    return (address >= ESR_FLASH_MEMORY_START && address < (ESR_FLASH_MEMORY_START + ESR_FLASH_MEMORY_SIZE)) ||
           (address >= ESR_RAM_MEMORY_START && address < (ESR_RAM_MEMORY_START + ESR_RAM_MEMORY_SIZE));
}


void rtpox_shrink(void){
    __disable_irq();
    rtpox_sau_disable();
    rtpox_configure_sau_nonsecure(ESR_FLASH_MEMORY_START, ESR_FLASH_MEMORY_START+ESR_FLASH_MEMORY_SIZE, 5);
    rtpox_configure_sau_nonsecure(ESR_RAM_MEMORY_START, ESR_RAM_MEMORY_START+ESR_RAM_MEMORY_SIZE, 6);
    // Memory barrier
    __DSB();
    __ISB();
    rtpox_sau_enable();
    __enable_irq();
}

void rtpox_expand(void){
    __disable_irq();
    rtpox_sau_disable();
    rtpox_configure_sau_secure(ESR_FLASH_MEMORY_START, ESR_FLASH_MEMORY_START+ESR_FLASH_MEMORY_SIZE, 5);
    rtpox_configure_sau_secure(ESR_RAM_MEMORY_START, ESR_RAM_MEMORY_START+ESR_RAM_MEMORY_SIZE, 6);
    // Memory barrier
    __DSB();
    __ISB();
    rtpox_sau_enable();
    __enable_irq();
}

void rtpox_esr_reset_handler(void){
    // Reset the elastic secure region
    rtpox_shrink();
}

/*
#########################################
Code related to non secure process management
#########################################
*/



void rtpox_ns_stack_to_entry_task(){

}

/*  
#########################################
Code related to interrupt/peripehral configuration
#########################################
*/

void rtpox_register_peripheral(void){
}

void rtpox_unregister_peripheral(void){
}

void rtpox_interrupt_config(void){
}

void rtpox_initialize_esr_vtor(void){
}

void rtpox_switch_to_esr(void){
    // Switch to the elastic secure region
    rtpox_expand();
    //    
}

void rtpox_switch_to_ns(void){
}



/*
#########################################
#########################################
*/


rtpox_error_t rtpox_init(void)
{
    // Test password
    if(rtpox_context.init_password != PASSWORD_INIT ) rtpox_reset_handler();

    // test if rtpox is active
    if(rtpox_context.status == ACTIVE) {
        return RTPOX_ALREADY_ACTIVE;
    }



    // expand esr
    rtpox_expand();

    // attest the context
    rtpox_attest_esr();


    
    rtpox_ns_stack_to_entry_task();

    return RTPOX_SUCESS;
}


rtpox_error_t rtpox_exit(void)
{
    return RTPOX_SUCESS;
}


