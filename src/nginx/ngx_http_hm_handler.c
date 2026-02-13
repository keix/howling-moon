/*
 * ngx_http_hm_handler.c - Content Phase Handler
 *
 * Delegates request processing to runtime.
 * Does not know runtime internals.
 */

#include "../runtime/hm_runtime_api.h"

ngx_int_t ngx_http_hm_handler(ngx_http_request_t *r) {
  ngx_int_t rc;
  ngx_chain_t *out;

  /* Check runtime availability */
  if (hm_global_runtime == NULL) {
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                  "howling-moon: runtime not initialized");
    return NGX_HTTP_INTERNAL_SERVER_ERROR;
  }

  /* Set response headers */
  r->headers_out.status = NGX_HTTP_OK;
  ngx_str_set(&r->headers_out.content_type, "text/plain");

  /* Execute via runtime */
  rc = hm_runtime_execute(hm_global_runtime, r, &out);
  if (rc != NGX_OK) {
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                  "howling-moon: runtime execution failed");
    return NGX_HTTP_INTERNAL_SERVER_ERROR;
  }

  /* Calculate content length from chain */
  r->headers_out.content_length_n = 0;
  for (ngx_chain_t *cl = out; cl != NULL; cl = cl->next) {
    if (cl->buf != NULL) {
      r->headers_out.content_length_n += cl->buf->last - cl->buf->pos;
    }
  }

  /* Send headers */
  rc = ngx_http_send_header(r);
  if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
    return rc;
  }

  /* Send body */
  return ngx_http_output_filter(r, out);
}
