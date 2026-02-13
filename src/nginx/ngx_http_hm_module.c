/*
 * ngx_http_hm_module.c - nginx HTTP Module Definition
 *
 * Registers directives and lifecycle hooks.
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

/* External declarations */
extern ngx_int_t ngx_http_hm_handler(ngx_http_request_t *r);
extern ngx_int_t ngx_http_hm_init_process(ngx_cycle_t *cycle);
extern void ngx_http_hm_exit_process(ngx_cycle_t *cycle);

static char *ngx_http_hm(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_command_t ngx_http_hm_commands[] = {
    {ngx_string("hm"), NGX_HTTP_LOC_CONF | NGX_CONF_NOARGS, ngx_http_hm, 0, 0,
     NULL},
    ngx_null_command};

static ngx_http_module_t ngx_http_hm_module_ctx = {
    NULL, /* preconfiguration */
    NULL, /* postconfiguration */

    NULL, /* create main conf */
    NULL, /* init main conf */

    NULL, /* create server conf */
    NULL, /* merge server conf */

    NULL, /* create location conf */
    NULL  /* merge location conf */
};

ngx_module_t ngx_http_hm_module = {NGX_MODULE_V1,
                                   &ngx_http_hm_module_ctx, /* module context */
                                   ngx_http_hm_commands,    /* module directives */
                                   NGX_HTTP_MODULE,         /* module type */
                                   NULL,                    /* init master */
                                   NULL,                    /* init module */
                                   ngx_http_hm_init_process, /* init process */
                                   NULL,                     /* init thread */
                                   NULL,                     /* exit thread */
                                   ngx_http_hm_exit_process, /* exit process */
                                   NULL,                     /* exit master */
                                   NGX_MODULE_V1_PADDING};

static char *ngx_http_hm(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {
  ngx_http_core_loc_conf_t *clcf;

  clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
  clcf->handler = ngx_http_hm_handler;

  return NGX_CONF_OK;
}
