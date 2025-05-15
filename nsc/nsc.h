
#ifndef NSC_H
#define NSC_H

#include <stdint.h>

void Secure_Test_Call(void);
uint32_t TZ_InitContextSystem_S (void);

void secure_rtpox_init_rtpox_process(void);
void secure_rtpox_exit_rtpox_process(void);

#endif  