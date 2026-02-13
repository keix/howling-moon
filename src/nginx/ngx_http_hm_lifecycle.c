/*
 * ngx_http_hm_lifecycle.c - Worker Lifecycle Management
 *
 * Creates and destroys runtime instance per worker process.
 */

#include "../runtime/hm_runtime_api.h"

/* Global runtime instance (one per worker) */
hm_runtime *hm_global_runtime = NULL;

/* Pool for runtime allocation (survives entire worker lifetime) */
static ngx_pool_t *ngx_http_hm_pool = NULL;

ngx_int_t ngx_http_hm_init_process(ngx_cycle_t *cycle) {
  /*
   * Create a dedicated pool for runtime.
   * This pool lives for the entire worker process lifetime.
   */
  ngx_http_hm_pool = ngx_create_pool(4096, cycle->log);
  if (ngx_http_hm_pool == NULL) {
    ngx_log_error(NGX_LOG_EMERG, cycle->log, 0,
                  "howling-moon: failed to create runtime pool");
    return NGX_ERROR;
  }

  /* Create stub runtime (will be replaced by moonquakes later) */
  hm_global_runtime = hm_runtime_create_stub(ngx_http_hm_pool, cycle->log);
  if (hm_global_runtime == NULL) {
    ngx_log_error(NGX_LOG_EMERG, cycle->log, 0,
                  "howling-moon: failed to create runtime");
    ngx_destroy_pool(ngx_http_hm_pool);
    ngx_http_hm_pool = NULL;
    return NGX_ERROR;
  }

  /* Initialize runtime */
  if (hm_runtime_init(hm_global_runtime) != NGX_OK) {
    ngx_log_error(NGX_LOG_EMERG, cycle->log, 0,
                  "howling-moon: runtime initialization failed");
    ngx_destroy_pool(ngx_http_hm_pool);
    ngx_http_hm_pool = NULL;
    hm_global_runtime = NULL;
    return NGX_ERROR;
  }

  return NGX_OK;
}

void ngx_http_hm_exit_process(ngx_cycle_t *cycle) {
  if (hm_global_runtime != NULL) {
    hm_runtime_shutdown(hm_global_runtime);
    hm_global_runtime = NULL;
  }

  if (ngx_http_hm_pool != NULL) {
    ngx_destroy_pool(ngx_http_hm_pool);
    ngx_http_hm_pool = NULL;
  }
}
