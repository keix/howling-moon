#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

ngx_int_t hm_content_handler(ngx_http_request_t *r) {
  ngx_buf_t *b;
  ngx_chain_t out;

  ngx_str_t type = ngx_string("text/plain");
  ngx_str_t response = ngx_string("howling-moon\n");

  r->headers_out.status = NGX_HTTP_OK;
  r->headers_out.content_length_n = response.len;
  r->headers_out.content_type = type;

  if (ngx_http_send_header(r) != NGX_OK) {
    return NGX_HTTP_INTERNAL_SERVER_ERROR;
  }

  b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
  if (b == NULL) {
    return NGX_HTTP_INTERNAL_SERVER_ERROR;
  }

  b->pos = response.data;
  b->last = response.data + response.len;
  b->memory = 1;
  b->last_buf = 1;

  out.buf = b;
  out.next = NULL;

  return ngx_http_output_filter(r, &out);
}
