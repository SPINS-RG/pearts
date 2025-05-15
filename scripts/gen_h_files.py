import json
import subprocess


def get_function_addresses(file_path, function_names):
    """
    Returns a dictionary mapping function names to their symbol addresses
    using arm-none-eabi-readelf, matching exact function names only.
    """
    try:
        result = subprocess.run(
            ["arm-none-eabi-readelf", "-s", file_path],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=True,
            text=True
        )
    except subprocess.CalledProcessError as e:
        print("Error running readelf:", e.stderr)
        return {}

    addresses = {}
    lines = result.stdout.splitlines()

    for func in function_names:
        for line in lines:
            # Skip empty lines or lines that don’t have the function name
            if func not in line:
                continue
            # Match exact name at the end of the line
            parts = line.strip().split()

            if parts and parts[-1] == func:
                address = parts[1]  # The symbol value is usually the 2nd column
                addresses[func] = address
                break  # Stop after the first exact match

        if func not in addresses:
            addresses[func] = 0

    return addresses


binary_path = "build_nonsecure/nonsecure/nonsecure.elf"
# binary_path = "build_nonsecure/nonsecure/nonsecure.elf"

definefile = "secure/include/var_auto_gen.h"

functions = [
    "rtpox_esr_entry",
    "rtpox_esr_resume",
    "rtpox_syscall_dispatch",
    "rtpox_esr_psp_reserve",
    "rtpox_esr_vtr_reserve",
    "SysTick_Handler",
    "rtpox_st_tmp_reserved",
    "rtpox_trig_resume"
]


if __name__ == "__main__":

    # Check if the binary file exists
    try:
        with open(binary_path, "rb") as f:
            pass
    except FileNotFoundError:
        print(f"Error: The file {binary_path} does not exist.")
        exit(1)

    addrs = get_function_addresses(binary_path, functions)

    with open(definefile, "w") as f:
        f.write("#ifndef VAR_AUTO_GEN_H\n")
        f.write("#define VAR_AUTO_GEN_H\n")
        f.write("\n")
        f.write("#include <stdint.h>\n")
        f.write("\n")
        f.write("/* This file is auto-generated. Do not edit manually! */\n")
        f.write("\n")

        for func, addr in addrs.items():
            if addr == 0:
                print(f"Warning: {func} not found in the binary.")
            else:
                f.write(f"#define ADDR_{func} 0x{addr}\n")

        f.write("\n")
        f.write("#endif /* VAR_AUTO_GEN_H */\n")
    print(f"Generated {definefile} with function addresses.")