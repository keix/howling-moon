/*
 * mq.h - Moonquakes Embedding API (Frozen ABI)
 *
 * Minimal public API for embedding Moonquakes runtime.
 * This header is the contract between howling-moon and Moonquakes.
 *
 * Ownership rules:
 *   - mq_vm* is owned by caller, freed via mq_vm_free()
 *   - mq_last_error() returns VM-owned string, valid until next mq_* call
 *   - mq_load_string() src is caller-owned, referenced only during call
 */

#ifndef MQ_H
#define MQ_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque VM handle */
typedef struct mq_vm mq_vm;

/* Status codes */
typedef enum mq_status {
    MQ_OK       = 0,
    MQ_ERR      = 1,  /* generic runtime error */
    MQ_OOM      = 2,  /* out of memory */
    MQ_SYNTAX   = 3,  /* parse/compile error */
    MQ_TYPE     = 4,  /* type error */
    MQ_INTERNAL = 5   /* bug / unreachable */
} mq_status;

/* VM configuration (optional) */
typedef struct mq_config {
    void *userdata;

    /* Allocator hooks (optional, NULL for defaults) */
    void *(*alloc)(void *ud, size_t n);
    void  (*free)(void *ud, void *p, size_t n);
} mq_config;

/*
 * Lifecycle
 */
mq_vm      *mq_vm_new(const mq_config *cfg);
void        mq_vm_free(mq_vm *vm);

/*
 * Loading
 */
mq_status   mq_load_file(mq_vm *vm, const char *path, const char *chunkname);
mq_status   mq_load_string(mq_vm *vm, const uint8_t *src, size_t len,
                           const char *chunkname);

/*
 * Execution
 *   Executes the loaded chunk. For v0, no args/returns.
 */
mq_status   mq_pcall(mq_vm *vm, int nargs, int nresults);

/*
 * Error reporting
 *   Returns pointer owned by VM, valid until next mq_* call that mutates state.
 */
const char *mq_last_error(const mq_vm *vm);

/*
 * Versioning
 */
uint32_t    mq_version_abi(void);     /* increments only on ABI break */
const char *mq_version_string(void);

#ifdef __cplusplus
}
#endif

#endif /* MQ_H */
