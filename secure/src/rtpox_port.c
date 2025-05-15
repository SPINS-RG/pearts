#include "hardware/structs/sau.h"
#include "hardware/structs/scb.h"
// #include "."
#include "_rp2350.h"
#include "rtpox_port.h"

/* 
##########################################
Code related to the secure access unit (SAU)
##########################################
*/

__always_inline void rtpox_configure_sau_nonsecure(uint32_t address_init, uint32_t address_end, uint32_t region_number){
    SAU->RNR  = region_number;
    SAU->RBAR = address_init & SAU_RBAR_BADDR_Msk;
    SAU->RLAR = (address_end & SAU_RLAR_LADDR_Msk) & ~SAU_RLAR_ENABLE_Msk;
    __DSB();
    __ISB();
}

__always_inline void rtpox_configure_sau_secure(uint32_t address_init, uint32_t address_end, uint32_t region_number){
    SAU->RNR  = region_number;
    SAU->RBAR = address_init & SAU_RBAR_BADDR_Msk;
    SAU->RLAR = (address_end & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;
    __DSB();
    __ISB();
}

__always_inline void rtpox_configure_sau_nsc(uint32_t address_init, uint32_t address_end, uint32_t region_number){
    SAU->RNR  = region_number;
    SAU->RBAR = address_init & SAU_RBAR_BADDR_Msk;
    SAU->RLAR = (address_end & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk | SAU_RLAR_NSC_Msk;
}   



__always_inline void rtpox_sau_disable(void){
    // Disable SAU
    SAU->CTRL &= ~SAU_CTRL_ENABLE_Msk ;
}

__always_inline void rtpox_sau_enable(void){
    // Enable SAU
    SAU->CTRL |= SAU_CTRL_ENABLE_Msk ;
}





__always_inline void rtpox_sau_nonsecure_when_deactivate(){
    SAU->CTRL |= 0b1u;

}

/* 
##########################################
Code related to the hash
##########################################
*/

pico_sha256_state_t sha256_state;

uint32_t rtpox_hash_init(){
    int rc = pico_sha256_start_blocking(&sha256_state, SHA256_BIG_ENDIAN, true); 
    if (rc != PICO_OK) {
        // Handle error
        return 1;
    }
    return 0;
}

void rtpox_hash_update(uint8_t *addr, size_t size){
    pico_sha256_update_blocking(&sha256_state, (const uint8_t*)addr, size);
    // pico_sha256_update(&sha256_state, (const uint8_t*)addr, size);
}

void rtpox_hash_finalize(char *sha256_result){
    pico_sha256_finish(&sha256_state, (sha256_result_t*)sha256_result);
    pico_sha256_cleanup(&sha256_state);
}


/* 
##########################################
Code related to the Interrupts
##########################################
*/




/* 
##########################################
Code related to the MPU
##########################################
*/
