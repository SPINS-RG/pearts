#ifndef RTPOX_H
#define RTPOX_H

//todo
#define ESR_FLASH_MEMORY_SIZE 1044480 // Size of the elastic secure region
//todo
#define ESR_FLASH_MEMORY_START 0x10300000 // Start address of the elastic secure region
//todo
#define ESR_RAM_MEMORY_SIZE 122800 // Size of the elastic secure region in RAM
//todo
#define ESR_RAM_MEMORY_START 0x20062000 // Start address of the elastic secure region in RAM

#define PASSWORD_INIT 0xFF22AA22 // Initial password to test if the context is initialized

// STATUS
#define ACTIVE   0x2f2f2f2
#define INACTIVE 0

//todo
#define START_ESR_PSP_ADDRESS 0x20002000 // Start address of the elastic secure region in RAM
//todo
#define ESR_VTOR_ADDRESS 0x20003000 // Start address of the elastic secure region in RAM

typedef enum {
    FALSE = 0,
    TRUE = 1
} boolean_t;


typedef enum{
    SYSTICK_KEY = 0xF0F0F0F0,
    SHADOW_RESUME_KEY = 0xF0F01111
}  rtpox_fault_keys_t;

typedef uint32_t rtpox_status_t;

typedef struct {
    int dummy; // Placeholder for actual structure members
} rtpox_shadow_task_t;

typedef enum ACTIVE_VTOR{
    NS_VTOR = 0,
    ESR_VTOR = 1
} active_vtor_t;

typedef struct {
    uint32_t       NS_VTOR;
    uint32_t       ESR_VTOR; 
    uint32_t       esr_psp;
    uint32_t       ns_psp;
    active_vtor_t   active_vtor;
    uint32_t        exit_address;
    uint32_t        status;
    uint32_t        init_password;
    uint32_t        shadow_task_initial_psp;
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


typedef enum {
    RTPOX_ALREADY_ACTIVE,
    RTPOX_SUCESS,
} rtpox_error_t;

rtpox_error_t rtpox_init(uint32_t return_address);
rtpox_error_t rtpox_exit(void);
void rtpox_switch_esr_to_ns(void);
void rtpox_switch_ns_psp_to_esr(void);


#endif // RTPOX_H