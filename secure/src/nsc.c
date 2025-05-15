#include "nsc.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "rtpox.h"
#include "_rp2350.h"
#include "cmsis_gcc.h"
#define NSC_ENTRY __attribute__((cmse_nonsecure_entry))
#define NSC_CALL __attribute__((cmse_nonsecure_call))
#define NSC_WEAK __attribute__((weak, cmse_nonsecure_entry))
#define NSC_WEAK_CALL __attribute__((weak, cmse_nonsecure_call))
#define NSC_NORETURN __attribute__((noreturn, cmse_nonsecure_entry))
#define NSC_NORETURN_CALL __attribute__((noreturn, cmse_nonsecure_call))
#define NSC_NORETURN_WEAK __attribute__((noreturn, weak, cmse_nonsecure_entry))
#define NSC_NORETURN_WEAK_CALL __attribute__((noreturn, weak, cmse_nonsecure_call))
#define NSC_NORETURN_WEAK_CALL __attribute__((noreturn, weak, cmse_nonsecure_call))     



extern rtpox_context_t rtpox_context;

NSC_ENTRY void secure_rtpox_init_rtpox_process(void){

    if (rtpox_context.status == ACTIVE) {
        return;
    }

    // get return address
    uint32_t return_address;
    __asm volatile ("mov %0, lr" : "=r" (return_address) );

    rtpox_init(return_address);    
    
    return;
}


NSC_ENTRY void secure_rtpox_exit_rtpox_process(void){

    if (rtpox_context.status == INACTIVE) {
        return;
    }
    
    rtpox_exit();

    return;
}
