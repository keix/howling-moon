/*
 * hm_runtime.h - Runtime Abstraction Layer
 *
 * Defines the vtable interface for runtime backends.
 * howling-moon core only sees this interface, never backend internals.
 */

#ifndef HM_RUNTIME_H
#define HM_RUNTIME_H

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct hm_runtime hm_runtime;
typedef struct hm_runtime_ops hm_runtime_ops;

/*
 * Runtime operations vtable
 */
struct hm_runtime_ops {
    /*
     * Initialize runtime instance.
     * Called once per worker in init_process.
     * Returns NGX_OK on success, NGX_ERROR on failure.
     */
    ngx_int_t (*init)(hm_runtime *rt, ngx_log_t *log);

    /*
     * Shutdown runtime instance.
     * Called once per worker in exit_process.
     */
    void (*shutdown)(hm_runtime *rt);

    /*
     * Execute script for request.
     * Allocates response buffer from r->pool.
     * Returns NGX_OK on success, NGX_ERROR on failure.
     */
    ngx_int_t (*execute)(hm_runtime *rt,
                         ngx_http_request_t *r,
                         ngx_chain_t **out);
};

/*
 * Runtime instance
 */
struct hm_runtime {
    const hm_runtime_ops *ops;
    void                 *backend_data;
    ngx_log_t            *log;
};

/*
 * Global runtime instance (one per worker)
 */
extern hm_runtime *hm_global_runtime;

/*
 * Backend constructors
 */
hm_runtime *hm_runtime_create_stub(ngx_pool_t *pool, ngx_log_t *log);

/* Future:
 * hm_runtime *hm_runtime_create_moonquakes(ngx_pool_t *pool, ngx_log_t *log);
 */

/*
 * Convenience macros
 */
#define hm_runtime_init(rt)         ((rt)->ops->init((rt), (rt)->log))
#define hm_runtime_shutdown(rt)     ((rt)->ops->shutdown((rt)))
#define hm_runtime_execute(rt, r, out) ((rt)->ops->execute((rt), (r), (out)))

#endif /* HM_RUNTIME_H */
