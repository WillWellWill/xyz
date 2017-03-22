#ifndef _NGX_NSPR_H_INCLUDED_
#define _NGX_NSPR_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>
#include <ngx_event_connect.h>
#include <nginx.h>
#include <ngx_http.h>

#define NGX_NSPR_VERSION                0

#define NGX_LOG_DEBUG_NSPR              NGX_LOG_DEBUG_CORE

#define NGX_NSPR_MODULE                 0x11111111  /* "NSPR" */

#define NGX_NSPR_MAIN_CONF              0x020000
#define NGX_NSPR_SRV_CONF               0x040000

#define NGX_NSPR_MAIN_CONF_OFFSET  offsetof(ngx_nspr_conf_ctx_t, main_conf)
#define NGX_NSPR_SRV_CONF_OFFSET   offsetof(ngx_nspr_conf_ctx_t, srv_conf)

#define ngx_nspr_conf_get_module_main_conf(cf, module)                       \
    ((ngx_nspr_conf_ctx_t *) cf->ctx)->main_conf[module.ctx_index]
#define ngx_nspr_conf_get_module_srv_conf(cf, module)                        \
    ((ngx_nspr_conf_ctx_t *) cf->ctx)->srv_conf[module.ctx_index]

#define ngx_nspr_get_module_main_conf(s, module)                             \
    (s)->main_conf[module.ctx_index]
#define ngx_nspr_get_module_srv_conf(s, module)  (s)->srv_conf[module.ctx_index]

typedef struct {
    void                  **main_conf;
    void                  **srv_conf;
} ngx_nspr_conf_ctx_t;


typedef struct
{
    union {
        struct sockaddr     sockaddr;
        struct sockaddr_in  sockaddr_in;
#if (NGX_HAVE_INET6)
        struct sockaddr_in6 sockaddr_in6;
#endif
#if (NGX_HAVE_UNIX_DOMAIN)
        struct sockaddr_un  sockaddr_un;
#endif
        u_char              sockaddr_data[NGX_SOCKADDRLEN];
    } u;

    socklen_t               socklen;

    /* server ctx */
    ngx_nspr_conf_ctx_t    *ctx;

    unsigned                bind:1;
    unsigned                wildcard:1;
    unsigned                proxy_protocol:1;
#if (NGX_MAIL_SSL)
    unsigned                ssl:1;
#endif
#if (NGX_HAVE_INET6 && defined IPV6_V6ONLY)
    unsigned                ipv6only:1;
#endif
    unsigned                so_keepalive:2;
#if (NGX_HAVE_KEEPALIVE_TUNABLE)
    int                     tcp_keepidle;
    int                     tcp_keepintvl;
    int                     tcp_keepcnt;
#endif
    int                     backlog;
} ngx_nspr_listen_t;

typedef struct {
    ngx_array_t             servers;    /* ngx_nspr_core_srv_conf_t */
    ngx_array_t             listen;     /* ngx_nspr_listen_t */
} ngx_nspr_core_main_conf_t;

typedef struct ngx_nspr_core_srv_conf_s {
    ngx_msec_t              timeout;
    ngx_msec_t              resolver_timeout;

    ngx_str_t               server_name;

    u_char                 *file_name;
    ngx_int_t               line;

    ngx_resolver_t         *resolver;
    ngx_log_t              *error_log;

    /* server ctx */
    ngx_nspr_conf_ctx_t    *ctx;
} ngx_nspr_core_srv_conf_t;

typedef struct {
    void       *(*create_main_conf)(ngx_conf_t *cf);
    char       *(*init_main_conf)(ngx_conf_t *cf, void *conf);

    void       *(*create_srv_conf)(ngx_conf_t *cf);
    char       *(*merge_srv_conf)(ngx_conf_t *cf, void *prev, void *conf);

} ngx_nspr_module_t;

typedef struct {
    void                   *addrs;
    ngx_uint_t              naddrs;
} ngx_nspr_port_t;


typedef struct {
    int                     family;
    in_port_t               port;
    ngx_array_t             addrs;       /* array of ngx_nspr_conf_addr_t */
} ngx_nspr_conf_port_t;

typedef struct {
    ngx_nspr_conf_ctx_t    *ctx;
    ngx_str_t               addr_text;
    unsigned                proxy_protocol:1;
} ngx_nspr_addr_conf_t;

typedef struct {
    struct sockaddr        *sockaddr;
    socklen_t               socklen;

    ngx_nspr_conf_ctx_t    *ctx;
    ngx_nspr_listen_t       opt;

    unsigned                bind:1;
    unsigned                wildcard:1;
    unsigned                proxy_protocol:1;
    unsigned                so_keepalive:2;
} ngx_nspr_conf_addr_t;

typedef struct {
    in_addr_t               addr;
    ngx_nspr_addr_conf_t    conf;
} ngx_nspr_in_addr_t;

typedef struct {
    uint32_t                signature;         /* "NSPR" */

    ngx_connection_t       *connection;

    ngx_str_t               out;
    ngx_buf_t              *buffer;

    void                  **ctx;
    void                  **main_conf;
    void                  **srv_conf;

    ngx_resolver_ctx_t     *resolver_ctx;

    ngx_str_t               login;
    ngx_str_t               passwd;

    ngx_str_t               salt;
    ngx_str_t               tag;
    ngx_str_t               tagged_line;
    ngx_str_t               text;

    ngx_str_t              *addr_text;
    ngx_str_t               host;
    ngx_str_t               smtp_helo;
    ngx_str_t               smtp_from;
    ngx_str_t               smtp_to;

    ngx_str_t               cmd;

    ngx_uint_t              command;
    ngx_array_t             args;

    ngx_uint_t              login_attempt;
    ngx_int_t               quit;
    ngx_int_t               blocked;
    ngx_event_t             close;

    void                   *nspr_connection;
} ngx_nspr_session_t;

typedef struct {
    ngx_str_t              *client;
    ngx_nspr_session_t     *session;
} ngx_nspr_log_ctx_t;

extern ngx_uint_t  ngx_nspr_max_module;
extern ngx_module_t  ngx_nspr_core_module;
extern ngx_nspr_core_main_conf_t      *ngx_nspr_core_main_conf;

typedef ngx_connection_t *(*nspr_connect_peer_cb)(u_char *url);
typedef void (*nspr_post_ioevent_cb)(ngx_event_t *ev);
typedef void (*nspr_http_response_cb)(ngx_http_request_t *r, ngx_chain_t *resp);
typedef struct {
    nspr_connect_peer_cb nspr_connect_peer_t;
    nspr_post_ioevent_cb nspr_post_ioevent_t;
} ngx_nspr_callback_t;

extern ngx_nspr_callback_t nspr_callback_set;

void ngx_nspr_init_connection(ngx_connection_t *c);

#endif /* _NGX_NSPR_H_INCLUDED_ */

