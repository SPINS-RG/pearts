
#include "_rp2350.h"
#include "rtpox.h"
#include "var_auto_gen.h"

#include "hardware/structs/scb.h"
// void hard_fault_handler_c(uint32_t* stack)
// {
//     uint32_t r0  = stack[0];
//     uint32_t r1  = stack[1];
//     uint32_t r2  = stack[2];
//     uint32_t r3  = stack[3];
//     uint32_t r12 = stack[4];
//     uint32_t lr  = stack[5];
//     uint32_t pc  = stack[6];  // <- Where the fault occurred
//     uint32_t psr = stack[7];

//     volatile uint32_t _CFSR  = SCB->CFSR;   // Configurable Fault Status Register
//     volatile uint32_t _HFSR  = SCB->HFSR;   // HardFault Status Register
//     volatile uint32_t _MMAR  = SCB->MMFAR;  // MemManage Fault Address
//     volatile uint32_t _BFAR  = SCB->BFAR;   // BusFault Address
//     volatile uint32_t _SHCSR = SCB->SHCSR;

//     // You can place a breakpoint here
//     while (1);  // halt for inspection
// }


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

void SecureFault_Handler(void) {
    // get msp-ns
    uint32_t * msp_ns = (uint32_t *) __TZ_get_MSP_NS();

    // check if fault came from the systick key and if systick is active
    if ((msp_ns[6] == SYSTICK_KEY)
        && ( scb_ns_hw->shcsr & (1 << 11) )){ 
        
        // change retunr address from the ns-msp stack
        msp_ns[6] = ADDR_SysTick_Handler;
        // set thumb on xpsr
        msp_ns[7] |= 0x1000000;
        
        rtpox_switch_esr_to_ns();
        return;
    }
    
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



