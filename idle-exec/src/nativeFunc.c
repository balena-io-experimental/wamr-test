#include "bh_platform.h"
#include "wasm_export.h"

void nativeFunc(wasm_exec_env_t exec_env)
{
    printf("calling into native function: %s\n", __FUNCTION__);
}