#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>
#include <ngx_nspr.h>

extern void nspr_app_core_udp_send(ngx_event_t *wev);
extern void nspr_app_core_udp_recv(ngx_event_t *rev);
ngx_connection_t *ngx_nspr_create_udp_connection(ngx_int_t port);

static void ngx_nspr_close_connection(ngx_connection_t *c)
{
    ngx_pool_t  *pool;

    ngx_log_debug1(NGX_LOG_DEBUG_NSPR, c->log, 0,
                   "close nspr connection: %d", c->fd);

#if (NGX_STAT_STUB)
    (void) ngx_atomic_fetch_add(ngx_stat_active, -1);
#endif

    c->destroyed = 1;

    pool = c->pool;

    ngx_close_connection(c);

    ngx_destroy_pool(pool);
}

static void ngx_nspr_init_addr_by_port(ngx_int_t port, ngx_pool_t *pool, ngx_addr_t **addr)
{
    struct sockaddr_in *sin = ngx_pcalloc(pool, sizeof(struct sockaddr_in));;
    sin->sin_addr.s_addr = INADDR_ANY;
    sin->sin_port = htons((in_port_t) port);

    ngx_addr_t *addrOut = ngx_pcalloc(pool, sizeof(ngx_addr_t));
    if (addrOut == NULL) {
        return;
    }

    addrOut->sockaddr = (struct sockaddr *) sin;
    addrOut->socklen = sizeof(struct sockaddr_in);

    u_char *p = ngx_pnalloc(pool, 7 + sizeof(":65535") - 1);
    if (p == NULL) {
        return;
    }

    addrOut->name.len = ngx_sprintf(p, "%s:%d",
                                       "0.0.0.0", port) - p;
    addrOut->name.data = p;
    *addr = addrOut;
}

static ngx_connection_t *create_udp_connection(ngx_pool_t *pool, ngx_log_t *log, ngx_int_t port)
{
    ngx_socket_t       s;
    ngx_event_t       *rev, *wev;
    ngx_connection_t  *c;

    s = ngx_socket(AF_INET, SOCK_DGRAM, 0);

    ngx_log_error(NGX_LOG_DEBUG, log, 0, "dgram socket %d", s);

    if (s == (ngx_socket_t) -1) {
        ngx_log_error(NGX_LOG_ALERT, log, ngx_socket_errno,
                      ngx_socket_n " failed");
        return NULL;
    }

    c = ngx_get_connection(s,log);

    if (c == NULL) {
        if (ngx_close_socket(s) == -1) {
            ngx_log_error(NGX_LOG_ALERT, log, ngx_socket_errno,
                          ngx_close_socket_n "failed");
        }

        return NULL;
    }

    c->type = SOCK_DGRAM;

    if (ngx_nonblocking(s) == -1) {
        ngx_log_error(NGX_LOG_ALERT, log, ngx_socket_errno,
                      ngx_nonblocking_n " failed");

        goto failed;
    }

    ngx_addr_t *addr = NULL;
    ngx_nspr_init_addr_by_port(port, pool, &addr);

    if (bind(s, addr->sockaddr, addr->socklen)) {
        ngx_log_error(NGX_LOG_CRIT, log, ngx_socket_errno,
                      "bind(%V) failed", &addr->name);

        goto failed;
    }
    ngx_log_error(NGX_LOG_INFO, log, 0, "bind(%V) successfully", &addr->name);
    c->local_sockaddr = addr->sockaddr;
    c->local_socklen = addr->socklen;

    c->recv = ngx_udp_recv;
    c->send = ngx_send;

    rev = c->read;
    wev = c->write;

    rev->log = log;
    wev->log = log;
    wev->ready = 0;

    c->number = ngx_atomic_fetch_add(ngx_connection_counter, 1);
    return c;

failed:

    ngx_close_connection(c);

    return NULL;

}

static int s_localPort = 12000;
ngx_connection_t *ngx_nspr_udp_connect_peer(u_char *url)
{
    ngx_url_t u;
    ngx_connection_t              *c;
    ngx_pool_t                    *pool;

    ngx_log_error(NGX_LOG_INFO, ngx_cycle->log, 0, "connect peer via udp protocol, %s", url);
    pool = ngx_create_pool(NGX_DEFAULT_POOL_SIZE, ngx_cycle->log);
    if (!pool) {
        ngx_log_debug0(NGX_LOG_ERR, ngx_cycle->log, 0, "ngx create pool failed");
        return NULL;
    }

    ngx_memzero(&u, sizeof(ngx_url_t));
    u_char *prefix = (u_char *)ngx_strstr(url, "://");
    if (prefix) {
        u.url.data = prefix + 3;
        u.url.len = (size_t)ngx_strlen(u.url.data);
    }
    else {
        u.url.data = url;
        u.url.len = (size_t)ngx_strlen(url);
    }

    u.default_port = 80;
    u.uri_part = 1;

    if (ngx_parse_url(pool, &u) != NGX_OK) {
        if (u.err) {
            ngx_log_error(NGX_LOG_WARN, ngx_cycle->log, 0,
                          "parse url failed! "
                          "%s when connect \"%V\"", u.err, &u.url);
            return NULL;
        }
        return NULL;
    }
    do {
        c = ngx_nspr_create_udp_connection(s_localPort++);
    } while (!c);
    
    c->sockaddr = u.addrs->sockaddr;
    c->socklen = u.addrs->socklen;
    return c;
}

ngx_connection_t *ngx_nspr_create_udp_connection(ngx_int_t port)
{
    ngx_cycle->log->log_level = 0xff;
    ngx_connection_t              *c;
    ngx_nspr_session_t        *s;
    ngx_pool_t                    *pool;
    ngx_int_t               event;

    pool = ngx_create_pool(NGX_DEFAULT_POOL_SIZE, ngx_cycle->log);
    if (!pool) {
        ngx_log_debug0(NGX_LOG_ERR, ngx_cycle->log, 0, "ngx create pool failed");
        return 0;
    }

    c = create_udp_connection(pool, ngx_cycle->log, port);

    if (!c) {
        ngx_log_error(NGX_LOG_ERR, ngx_cycle->log, 0, "create udp client connection failed");
        return 0;
    }

    if (ngx_add_conn) {
        if (ngx_add_conn(c) == NGX_ERROR) {
            ngx_log_debug0(NGX_LOG_ERR, ngx_cycle->log, 0, "ngx_add_conn failed");
            return 0;
        }
    }

    if (ngx_event_flags & NGX_USE_CLEAR_EVENT) {
        /* kqueue */
        event = NGX_CLEAR_EVENT;

    } else {
        /* select, poll, /dev/poll */
        event = NGX_LEVEL_EVENT;
    }

    if (ngx_add_event(c->read, NGX_READ_EVENT, event) != NGX_OK) {
        ngx_nspr_close_connection(c);
        ngx_log_debug0(NGX_LOG_ERR, ngx_cycle->log, 0, "ngx add event failed");
        return NULL;
    }

    c->pool = pool;
    c->log = ngx_cycle->log;
    c->read->handler = nspr_app_core_udp_recv;
    c->write->handler = nspr_app_core_udp_send;
    c->log_error = NGX_ERROR_INFO;

    s = ngx_pcalloc(pool, sizeof(ngx_nspr_session_t));
    if (s == NULL) {
        return NULL;
    }

    c->data = s;
    s->signature = NGX_NSPR_MODULE;
    s->connection = c;
    return c;
}


