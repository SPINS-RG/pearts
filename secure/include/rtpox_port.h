#ifndef RTPOX_PORT_H
#define RTPOX_PORT_H

#include "hardware/sha256.h"
#include "pico/sha256.h"
// SAU
void rtpox_configure_sau_secure(uint32_t address_init, uint32_t address_end, uint32_t region_number);
void rtpox_configure_sau_nonsecure(uint32_t address_init, uint32_t address_end, uint32_t region_number);
void rtpox_configure_sau_nsc(uint32_t address_init, uint32_t address_end, uint32_t region_number);
void rtpox_sau_disable(void);
void rtpox_sau_enable(void);
void rtpox_sau_enable_region(uint32_t region);
void rtpox_sau_disable_region(uint32_t region);


// HASH
uint32_t rtpox_hash_init();
void rtpox_hash_update(uint8_t *addr, size_t size);
void rtpox_hash_finalize(char *sha256_result);


#endif // RTPOX_PORT_H