
#include "_rp2350.h"
#include "rtpox.h"
#include "var_auto_gen.h"

#include "hardware/structs/scb.h"
#include "hardware/structs/sau.h"

__attribute__((naked)) void HardFault_Handler(void)
{
    while (1)
    {
        /* code */
    }
    
}

// void HardFault_Handler(void) {
//     // Handle HardFault exception
//     // This is a placeholder function. Actual implementation will depend on the specific requirements.
//     while(1){}
// }

void MemoryManagementFault_Handler(void) {
    // Handle Memory Management Fault exception
    // This is a placeholder function. Actual implementation will depend on the specific requirements.
    while(1){}
}

void BusFault_Handler(void) {
    // Handle Bus Fault exception
    // This is a placeholder function. Actual implementation will depend on the specific requirements.
    while(1){}
}

void UsageFault_Handler(void) {
    // Handle Usage Fault exception
    // This is a placeholder function. Actual implementation will depend on the specific requirements.
    while(1){}
}

#define RETURN_ADDR_POSITION_MSP_NS  6
#define RETURN_ADDR_POSITION_XPSR_NS 7
#define RETURN_ADDR_POSITION_PSP_NS  6


// Return to Handler PSP_NS
#define SPSEL_BIT_MASK 0b1100U

#define SECURE_TRIGGER_RTPOX_RESUME 0xF0F0F0E0
#define SECURE_TRIGGER_RTPOX_SYSCALL 0xF0F0F0EF
#define SECURE_TRIGGER_RTPOX_SYSCALL_RETURN 0xF0F0F0E1

#define RESET_SECURE_FAULT_STATUS_REGISTER() SAU->SFSR = 0xFFFFFFFF

typedef enum {
    SC_osDelay = 0,
} esr_syscall_id_t;

typedef struct 
{
    uint8_t size;
    uint32_t args[4];
} esr_syscall_arg_t;

void SecureFault_Handler(void) {
    // get msp-ns
    uint32_t * msp_ns = (uint32_t *) __TZ_get_MSP_NS();

    // check if fault came from the systick key and if systick is active
    if ((msp_ns[RETURN_ADDR_POSITION_MSP_NS] == SYSTICK_KEY)
        && ( scb_ns_hw->shcsr & (1 << 11) )){ 
        __disable_irq();
        // change retunr address from the ns-msp stack
        msp_ns[RETURN_ADDR_POSITION_MSP_NS] = ADDR_SysTick_Handler;
        // set thumb on xpsr
        msp_ns[RETURN_ADDR_POSITION_XPSR_NS] |= 0x1000000;
        
        rtpox_switch_esr_to_ns();
    
        __enable_irq();
        return;
    }

    uint32_t lr;
    __ASM volatile ("mov %0, lr" : "=r" (lr));
    uint32_t * psp_ns = (uint32_t *) __TZ_get_PSP_NS();


    // Resume from the shadow task
    if (( SAU->SFAR== SECURE_TRIGGER_RTPOX_RESUME )&&
        ( (lr & SPSEL_BIT_MASK) == SPSEL_BIT_MASK) &&
        !rtpox_is_esr( psp_ns[RETURN_ADDR_POSITION_PSP_NS])){ 
        __disable_irq();
        RESET_SECURE_FAULT_STATUS_REGISTER(); //reset the fault status register
        rtpox_switch_resume_to_esr();
        __enable_irq();
        return;
    }


        // Resume from system call
    if (( SAU->SFAR == SECURE_TRIGGER_RTPOX_SYSCALL_RETURN )&&
        ( (lr & SPSEL_BIT_MASK) == SPSEL_BIT_MASK) &&
        !rtpox_is_esr( psp_ns[RETURN_ADDR_POSITION_PSP_NS])){ 
        __disable_irq();
        RESET_SECURE_FAULT_STATUS_REGISTER();//reset the fault status register
        rtpox_switch_resume_to_esr();
        __enable_irq();
        return;
        
    }

    // System call from esr
    if (( SAU->SFAR == SECURE_TRIGGER_RTPOX_SYSCALL )&&
        ( (lr & SPSEL_BIT_MASK) == SPSEL_BIT_MASK) &&
        rtpox_is_esr( psp_ns[RETURN_ADDR_POSITION_PSP_NS])){ 
        __disable_irq();
        RESET_SECURE_FAULT_STATUS_REGISTER(); //reset the fault status register
        rtpox_switch_esr_to_syscall();
        __enable_irq();
        return;
    }

    // check RTPOX_VIOLATION


    
    // trap the fault otherwise
    while(1){}
}

void Default_Handler(void) {
    // Handle Default exception
    // This is a placeholder function. Actual implementation will depend on the specific requirements.
    while(1){}
}

void SVC_Handler(void) {
    // Handle Supervisor Call (SVC)
    // This is a placeholder function. Actual implementation will depend on the specific requirements.
    while(1){}
}
void PendSV_Handler(void) {
    // Handle PendSV exception
    // This is a placeholder function. Actual implementation will depend on the specific requirements.
    while(1){}
}
void SysTick_Handler(void) {
    // Handle SysTick timer interrupt
    // This is a placeholder function. Actual implementation will depend on the specific requirements.
    while(1){}
}

void NMI_Handler(void) {
    // Handle Non-Maskable Interrupt (NMI)
    // This is a placeholder function. Actual implementation will depend on the specific requirements.
    while(1){}
}

void DebugMonitor_Handler(void) {
    // Handle Debug Monitor exception
    // This is a placeholder function. Actual implementation will depend on the specific requirements.
    while(1){}
}

/////////////////////////////////////////////////////////////////////////////////////////

/*
Generic interrupt control funtions
*/
void enable_interrupt(IRQn_Type irq) {
    NVIC_EnableIRQ(irq);
}

void disable_interrupt(IRQn_Type irq) {
    NVIC_DisableIRQ(irq);
}



