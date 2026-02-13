#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

extern ngx_int_t hm_content_handler(ngx_http_request_t *r);

static char *hm(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_command_t hm_commands[] = {
    {ngx_string("hm"), NGX_HTTP_LOC_CONF | NGX_CONF_NOARGS, hm, 0, 0, NULL},
    ngx_null_command};

static ngx_http_module_t hm_module_ctx = {
    NULL, /* preconfiguration */
    NULL, /* postconfiguration */

    NULL, /* create main conf */
    NULL, /* init main conf */

    NULL, /* create server conf */
    NULL, /* merge server conf */

    NULL, /* create location conf */
    NULL  /* merge location conf */
};

ngx_module_t ngx_http_hm_module = {
    NGX_MODULE_V1, &hm_module_ctx, hm_commands, NGX_HTTP_MODULE,
    NULL,          NULL,           NULL,        NULL,
    NULL,          NULL,           NULL,        NGX_MODULE_V1_PADDING};

static char *hm(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {
  ngx_http_core_loc_conf_t *clcf;

  clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
  clcf->handler = hm_content_handler;

  return NGX_CONF_OK;
}
