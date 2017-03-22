#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <nginx.h>

#include "ngx_nspr.h"

extern ngx_int_t nspr_app_core_api_process(ngx_http_request_t *r, nspr_http_response_cb cb);
extern void nspr_app_core_api_init();
extern void nspr_app_core_all_connection_init();

static ngx_int_t ngx_nspr_api_init_process(ngx_cycle_t *cycle);
static char *ngx_nspr_api(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_nspr_api_postconfiguration(ngx_conf_t *cf);
static void * ngx_nspr_api_create_loc_conf(ngx_conf_t *cf);
static char * ngx_nspr_api_merge_loc_conf(ngx_conf_t *cf,
        void *parent, void *child);

static void ngx_nspr_response_cb_with_finalize(ngx_http_request_t *r, ngx_chain_t *resp);

static time_t                       start_time;


/*
 * global: api-{bufs-{total,free,used}, total bytes in/out, bw in/out} - cscf
*/


typedef struct {
    ngx_uint_t                      api;
} ngx_nspr_api_loc_conf_t;

#define NGX_NSPR_STAT_ALL           0xff                                                               
#define NGX_NSPR_STAT_GLOBAL        0x01                                                               
#define NGX_NSPR_STAT_LIVE          0x02                                                               
#define NGX_NSPR_STAT_CLIENTS       0x04                                                               
#define NGX_NSPR_STAT_PLAY          0x08

static ngx_conf_bitmask_t           ngx_nspr_api_masks[] = {
    { ngx_string("all"),            NGX_NSPR_STAT_ALL           },
    { ngx_string("global"),         NGX_NSPR_STAT_GLOBAL        },
    { ngx_string("live"),           NGX_NSPR_STAT_LIVE          },
    { ngx_string("clients"),        NGX_NSPR_STAT_CLIENTS       },
    { ngx_null_string,              0 }
};


static ngx_command_t  ngx_nspr_api_commands[] = {

    { ngx_string("nspr_api"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_1MORE,
        ngx_nspr_api,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_nspr_api_loc_conf_t, api),
        ngx_nspr_api_masks },

    ngx_null_command
};


static ngx_http_module_t  ngx_nspr_api_module_ctx = {
    NULL,                               /* preconfiguration */
    ngx_nspr_api_postconfiguration,    /* postconfiguration */

    NULL,                               /* create main configuration */
    NULL,                               /* init main configuration */

    NULL,                               /* create server configuration */
    NULL,                               /* merge server configuration */

    ngx_nspr_api_create_loc_conf,      /* create location configuration */
    ngx_nspr_api_merge_loc_conf,       /* merge location configuration */
};


ngx_module_t  ngx_nspr_api_module = {
    NGX_MODULE_V1,
    &ngx_nspr_api_module_ctx,          /* module context */
    ngx_nspr_api_commands,             /* module directives */
    NGX_HTTP_MODULE,                    /* module type */
    NULL,                               /* init master */
    NULL,                               /* init module */
    ngx_nspr_api_init_process,         /* init process */
    NULL,                               /* init thread */
    NULL,                               /* exit thread */
    NULL,                               /* exit process */
    NULL,                               /* exit master */
    NGX_MODULE_V1_PADDING
};

extern ngx_connection_t *ngx_nspr_create_udp_connection(ngx_int_t port);
static ngx_int_t
ngx_nspr_api_init_process(ngx_cycle_t *cycle)
{
    /*
     * HTTP process initializer is called
     * after event module initializer
     * so we can run posted events here
     */

    //ngx_event_process_posted(cycle, &ngx_nspr_init_queue);
    ngx_log_error(NGX_LOG_INFO, cycle->log, 0, "my slot is %d", ngx_process_slot); 
	if (ngx_process_slot > 0) {
		//ngx_close_listening_sockets(cycle);
		//return NGX_OK;
	}
    nspr_app_core_all_connection_init();
    nspr_app_core_api_init();
    
    ngx_nspr_create_udp_connection(60001);

    return NGX_OK;
}


static void ngx_nspr_post_body_cb(ngx_http_request_t *r)
{
    if (!r) {
        ngx_log_debug0(NGX_LOG_ERR, r->connection->log, 0, "request is null");
        return;
    }
    if(r->uri.data == NULL) {
        ngx_log_debug0(NGX_LOG_ERR, r->connection->log, 0, "uri is null");
        return;
    }

    if (!nspr_app_core_api_process(r, ngx_nspr_response_cb_with_finalize)) {
        ngx_http_finalize_request(r, NGX_HTTP_FORBIDDEN);
        return;
    }
}

extern void nspr_app_core_log(const char *logTxt, size_t logLen);
static ngx_int_t
ngx_nspr_api_handler(ngx_http_request_t *r)
{
    ngx_nspr_api_loc_conf_t       *slcf;
    ngx_core_conf_t  *ccf;
    ccf = (ngx_core_conf_t *) ngx_get_conf(ngx_cycle->conf_ctx,
                                           ngx_core_module);
    ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "process count %d", ccf->worker_processes);

    slcf = ngx_http_get_module_loc_conf(r, ngx_nspr_api_module);
    if (slcf->api == 0) {
        return NGX_DECLINED;
    }

    return ngx_http_read_client_request_body(r, ngx_nspr_post_body_cb);
}


static void *
ngx_nspr_api_create_loc_conf(ngx_conf_t *cf)
{
    ngx_nspr_api_loc_conf_t       *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_nspr_api_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    conf->api = 0;

    return conf;
}


static char *
ngx_nspr_api_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_nspr_api_loc_conf_t       *prev = parent;
    ngx_nspr_api_loc_conf_t       *conf = child;

    ngx_conf_merge_bitmask_value(conf->api, prev->api, 0);

    return NGX_CONF_OK;
}


static char *
ngx_nspr_api(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t  *clcf;

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_nspr_api_handler;

    return ngx_conf_set_bitmask_slot(cf, cmd, conf);
}


static ngx_int_t
ngx_nspr_api_postconfiguration(ngx_conf_t *cf)
{
    start_time = ngx_cached_time->sec;

    return NGX_OK;
}

static void ngx_nspr_response_cb_with_finalize(ngx_http_request_t *r, ngx_chain_t *cl)
{
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "----api result %s", cl->buf->pos); 
    ngx_str_set(&r->headers_out.content_type, "application/vnd.plcm.plcm-csc+json");
    r->headers_out.content_length_n = cl->buf->last - cl->buf->pos;
    r->headers_out.status = NGX_HTTP_OK;
    cl->buf->last_buf = (r == r->main) ? 1 : 0;
    cl->buf->last_in_chain = 1;
    ngx_http_send_header(r);
    ngx_http_finalize_request(r, ngx_http_output_filter(r, cl));

}

