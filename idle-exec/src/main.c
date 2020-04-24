
/*
    Adapted from https://github.com/bytecodealliance/wasm-micro-runtime/tree/master/samples/basic
 */

#include "wasm_export.h"
#include "bh_read_file.h"
#include "idle.h"

void nativeFunc();

int main(int argc, char *argv_main[], char *envp[])
{
    const char *s = getenv("MSG_IDLE");
    printf("PATH :%s\n", (s != NULL) ? s : "getenv returned NULL");
    static char global_heap_buf[512 * 1024];
    // char *buffer, error_buf[128];
    char error_buf[128];
    // char *wasm_path = argv_main[1];

    wasm_module_t module = NULL;
    wasm_module_inst_t module_inst = NULL;
    wasm_exec_env_t exec_env = NULL;
    // uint32 buf_size, stack_size = 8092, heap_size = 8092;
    uint32 stack_size = 8092, heap_size = 8092;
    wasm_function_inst_t func = NULL;
    int32_t wasm_buffer = 0;

    RuntimeInitArgs init_args;
    memset(&init_args, 0, sizeof(RuntimeInitArgs));

    // Define an array of NativeSymbol for the APIs to be exported.
    // Note: the array must be static defined since runtime
    //            will keep it after registration
    // For the function signature specifications, goto the link:
    // https://github.com/bytecodealliance/wasm-micro-runtime/blob/master/doc/export_native_api.md

    /*     static NativeSymbol native_symbols[] =
        {
            {
                "nativeFunc", // the name of WASM function name
                nativeFunc,   // the native function pointer
                "()"          // the function prototype signature, avoid to use i32
            }}; */

    init_args.mem_alloc_type = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

    // Native symbols need below registration phase
    /*   init_args.n_native_symbols = sizeof(native_symbols) / sizeof(NativeSymbol);
    init_args.native_module_name = "extern";
    init_args.native_symbols = native_symbols; */

    if (!wasm_runtime_full_init(&init_args))
    {
        printf("Init runtime environment failed.\n");
        return -1;
    }

    /* buffer = bh_read_file_to_buffer(wasm_path, &buf_size);

    if (!buffer)
    {
        printf("Open wasm app file [%s] failed.\n", wasm_path);
        goto fail;
    } */

    // module = wasm_runtime_load(buffer, buf_size, error_buf, sizeof(error_buf));
    module = wasm_runtime_load(idle_aot, idle_aot_len, error_buf, sizeof(error_buf));
    if (!module)
    {
        printf("Load wasm module failed. error: %s\n", error_buf);
        goto fail;
    }

    module_inst = wasm_runtime_instantiate(module,
                                           stack_size,
                                           heap_size,
                                           error_buf,
                                           sizeof(error_buf));

    if (!module_inst)
    {
        printf("Instantiate wasm module failed. error: %s\n", error_buf);
        goto fail;
    }

    exec_env = wasm_runtime_create_exec_env(module_inst, stack_size);
    if (!exec_env)
    {
        printf("Create wasm execution environment failed.\n");
        goto fail;
    }

    uint32 argv[0];

    if (!(func = wasm_runtime_lookup_function(module_inst, "_start", NULL)))
    {
        printf("The _start wasm function is not found.\n");
        goto fail;
    }

    if (!wasm_runtime_call_wasm(exec_env, func, 0, argv))
    {
        printf("call wasm function_start failed. %s\n", wasm_runtime_get_exception(module_inst));
        goto fail;
    }

    // printf("Native finished calling wasm functioncallExternalFunc()\n");

fail:
    if (exec_env)
        wasm_runtime_destroy_exec_env(exec_env);
    if (module_inst)
    {
        if (wasm_buffer)
            wasm_runtime_module_free(module_inst, wasm_buffer);
        wasm_runtime_deinstantiate(module_inst);
    }
    if (module)
        wasm_runtime_unload(module);
    // if (buffer)
    //     BH_FREE(buffer);
    wasm_runtime_destroy();
    return 0;
}
