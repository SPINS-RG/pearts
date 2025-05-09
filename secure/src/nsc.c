#include "nsc.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "rtpox.h"


#define NSC_ENTRY __attribute__((cmse_nonsecure_entry))
#define NSC_CALL __attribute__((cmse_nonsecure_call))
#define NSC_WEAK __attribute__((weak, cmse_nonsecure_entry))
#define NSC_WEAK_CALL __attribute__((weak, cmse_nonsecure_call))
#define NSC_NORETURN __attribute__((noreturn, cmse_nonsecure_entry))
#define NSC_NORETURN_CALL __attribute__((noreturn, cmse_nonsecure_call))
#define NSC_NORETURN_WEAK __attribute__((noreturn, weak, cmse_nonsecure_entry))
#define NSC_NORETURN_WEAK_CALL __attribute__((noreturn, weak, cmse_nonsecure_call))
#define NSC_NORETURN_WEAK_CALL __attribute__((noreturn, weak, cmse_nonsecure_call))     

NSC_ENTRY void Secure_Test_Call(void){
    // printf("Hello from NSC secure world\n");
    // sleep_ms(10);
    return;
}

NSC_ENTRY void secure_rtpox_init_rtpox_process(void){
    // printf("Hello from NSC secure world\n");
    // sleep_ms(10);
    return;
}

