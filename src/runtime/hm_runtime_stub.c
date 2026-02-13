/*
 * hm_runtime_stub.c - Stub Runtime Backend
 *
 * Returns fixed response for testing.
 * No external dependencies.
 */

#include "hm_runtime_api.h"

static ngx_int_t stub_init(hm_runtime *rt, ngx_log_t *log);
static void stub_shutdown(hm_runtime *rt);
static ngx_int_t stub_execute(hm_runtime *rt, ngx_http_request_t *r,
                              ngx_chain_t **out);

static const hm_runtime_ops stub_ops = {
    .init = stub_init, .shutdown = stub_shutdown, .execute = stub_execute};

static ngx_int_t stub_init(hm_runtime *rt, ngx_log_t *log) {
  ngx_log_error(NGX_LOG_NOTICE, log, 0,
                "howling-moon: stub runtime initialized");
  return NGX_OK;
}

static void stub_shutdown(hm_runtime *rt) {
  ngx_log_error(NGX_LOG_NOTICE, rt->log, 0,
                "howling-moon: stub runtime shutdown");
}

static ngx_int_t stub_execute(hm_runtime *rt, ngx_http_request_t *r,
                              ngx_chain_t **out) {
  ngx_buf_t *b;
  ngx_chain_t *cl;

  static const u_char response[] = "howling-moon\n";
  static const size_t response_len = sizeof(response) - 1;

  cl = ngx_pcalloc(r->pool, sizeof(ngx_chain_t));
  if (cl == NULL) {
    return NGX_ERROR;
  }

  b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
  if (b == NULL) {
    return NGX_ERROR;
  }

  b->pos = (u_char *)response;
  b->last = (u_char *)response + response_len;
  b->memory = 1;
  b->last_buf = 1;

  cl->buf = b;
  cl->next = NULL;

  *out = cl;

  return NGX_OK;
}

hm_runtime *hm_runtime_create_stub(ngx_pool_t *pool, ngx_log_t *log) {
  hm_runtime *rt;

  rt = ngx_pcalloc(pool, sizeof(hm_runtime));
  if (rt == NULL) {
    return NULL;
  }

  rt->ops = &stub_ops;
  rt->backend_data = NULL;
  rt->log = log;

  return rt;
}
