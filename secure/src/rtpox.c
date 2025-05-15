
#include <stdint.h>
#include <string.h>
#include "rtpox_port.h"
#include "rtpox.h"
#include "_rp2350.h"
#include "hardware/structs/scb.h"
#include "var_auto_gen.h"


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
    rtpox_sau_disable();
    rtpox_configure_sau_nonsecure(ESR_FLASH_MEMORY_START, ESR_FLASH_MEMORY_START+ESR_FLASH_MEMORY_SIZE, 5);
    rtpox_configure_sau_nonsecure(ESR_RAM_MEMORY_START, ESR_RAM_MEMORY_START+ESR_RAM_MEMORY_SIZE, 6);
    rtpox_sau_enable();    
}

void rtpox_expand(void){
    rtpox_sau_disable();
    rtpox_configure_sau_secure(ESR_FLASH_MEMORY_START, ESR_FLASH_MEMORY_START+ESR_FLASH_MEMORY_SIZE, 5);
    rtpox_configure_sau_secure(ESR_RAM_MEMORY_START, ESR_RAM_MEMORY_START+ESR_RAM_MEMORY_SIZE, 6);
    rtpox_sau_enable();
}

void rtpox_esr_reset_handler(void){
    // Reset the elastic secure region
    // rtpox_shrink();
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



#define ADDR_rtpox_esr_psp_reserve_size 255
#define ADDR_rtpox_esr_vtr_reserve_size 64 



void rtpox_config_esr_vtor(){
    uint32_t * ptr_esr = (uint32_t*) ADDR_rtpox_esr_vtr_reserve;
    
    // redirect systick
    ptr_esr[15] = 0xF0F0F0F0;
}

void rtpox_copy_vtor(){
    uint32_t * ptr_nsvtor = (uint32_t*)scb_ns_hw->vtor;
    uint32_t * ptr_esr = (uint32_t*) ADDR_rtpox_esr_vtr_reserve;
    for (int i=0;i<ADDR_rtpox_esr_vtr_reserve_size;i++) ptr_esr[i] = ptr_nsvtor[i];
    
}

__force_inline void rtpox_switch_vtor_ns_to_esr(){
    scb_ns_hw->vtor = rtpox_context.ESR_VTOR;
}

__force_inline void rtpox_switch_vtor_esr_to_ns(){
    scb_ns_hw->vtor = rtpox_context.NS_VTOR;
}

__force_inline void rtpox_switch_psp_esr_to_ns(){
    // get current esr_psp
    rtpox_context.esr_psp = (uint32_t )__TZ_get_PSP_NS();

    // set psp_ns to the current psp
    __TZ_set_PSP_NS(ADDR_rtpox_st_tmp_reserved);

    uint32_t * nspsp = (uint32_t *)ADDR_rtpox_st_tmp_reserved;
    nspsp[7] = 0x1000000;
    nspsp[6] = (uint32_t) ADDR_rtpox_trig_resume;
}

__force_inline void rtpox_switch_psp_ns_to_esr(){
    rtpox_context.ns_psp = (uint32_t )__TZ_get_PSP_NS();
    __TZ_set_PSP_NS(rtpox_context.esr_psp);
}


void rtpox_switch_esr_to_ns(){
    rtpox_expand();
    rtpox_switch_psp_esr_to_ns();
    rtpox_switch_vtor_esr_to_ns();
}
 
void rtpox_switch_ns_psp_to_esr(){
    rtpox_shrink();
    rtpox_switch_psp_ns_to_esr();
    rtpox_switch_vtor_ns_to_esr();
}




rtpox_error_t rtpox_init(uint32_t return_address)
{
    __disable_irq();
    
    // Test password
    if(rtpox_context.init_password != PASSWORD_INIT ) rtpox_reset_handler();
    
    // save est context
    rtpox_context.exit_address = (uint32_t)return_address;
    rtpox_context.active_vtor = ESR_VTOR;
    rtpox_context.NS_VTOR = (uint32_t)scb_ns_hw->vtor;
    rtpox_context.ESR_VTOR = (uint32_t)ADDR_rtpox_esr_vtr_reserve;
    rtpox_context.ns_psp = __TZ_get_PSP_NS();
    rtpox_context.esr_psp = ADDR_rtpox_esr_psp_reserve + ADDR_rtpox_esr_psp_reserve_size;
    rtpox_context.shadow_task_initial_psp = (uint32_t)rtpox_context.ns_psp ;
    
    // copy ADDR_rtpox_esr_entry to stack position 3
    rtpox_switch_ns_psp_to_esr();
    
    // Set the return address from the secure call to the entry point of the elastic secure region
    uint32_t *stack;
    stack = (uint32_t *)__get_PSP();
    stack[11] = (ADDR_rtpox_esr_entry & ~(0x1u) ); 
    
    rtpox_context.status = ACTIVE;
    
    // rtpox_init_esr_psp();
    
    
    // Copy ns vtor to 
    rtpox_copy_vtor();
    rtpox_config_esr_vtor();
    
    
    // rtpox_shrink(); 
    // rtpox_switch_ns_psp_to_esr();
    // rtpox_switch_vtor_ns_to_esr();
    rtpox_switch_ns_psp_to_esr();


    __enable_irq();
    
    return RTPOX_SUCESS;
}


rtpox_error_t rtpox_exit(void)
{
    //disable interrupt
    __disable_irq();

    // set the nonsecure pointer 
    __TZ_set_PSP_NS(rtpox_context.shadow_task_initial_psp);

    // set the return address of the secure world to the end of the shadow stack entry function 
    uint32_t *stack;
    stack = (uint32_t *)__get_PSP();
    stack[7] = (rtpox_context.exit_address & ~(0x1u) ); 
    
    rtpox_switch_vtor_esr_to_ns();
    rtpox_shrink();


    //enable interrupt
    __enable_irq();

    return RTPOX_SUCESS;
}


