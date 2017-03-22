
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>
#include <ngx_nspr.h>

extern void nspr_app_core_tcp_create_connection(ngx_connection_t *c);
extern void nspr_app_core_tcp_destroy_connection(ngx_connection_t *c);
extern void nspr_app_core_all_connection_init();
extern ngx_buf_t *nspr_app_core_get_send_buffer(ngx_nspr_session_t *session);
extern void nspr_app_core_confirm_send_buffer(ngx_nspr_session_t *session);
extern ngx_buf_t *nspr_app_core_get_recv_buffer(ngx_nspr_session_t *session);
extern void nspr_app_core_confirm_recv_buffer(ngx_nspr_session_t *session);
extern void nspr_app_core_set_callback(ngx_nspr_callback_t cb);
extern void nspr_app_core_log(const char *logTxt, size_t logLen);

static void ngx_nspr_init_session(ngx_connection_t *c);
static void ngx_nspr_close_connection(ngx_connection_t *c);
static u_char *ngx_nspr_log_error(ngx_log_t *log, u_char *buf, size_t len);
static ngx_connection_t *ngx_nspr_connect_peer(u_char *url);
static void ngx_nspr_post_ioevent(ngx_event_t *rev);
static void nspr_app_core_tcp_recv(ngx_event_t *rev);
static void nspr_app_core_tcp_send(ngx_event_t *wev);

static void ngx_nspr_cycle(ngx_nspr_session_t *s);
static ngx_connection_t *ngx_nspr_tcp_connect_peer(u_char *url);
extern ngx_connection_t *ngx_nspr_udp_connect_peer(u_char *url);

void
ngx_nspr_init_connection(ngx_connection_t *c)
{
    size_t                     len;
    ngx_uint_t                 i;
    ngx_nspr_port_t           *port;
    struct sockaddr           *sa;
    struct sockaddr_in        *sin;
    ngx_nspr_log_ctx_t        *ctx;
    ngx_nspr_in_addr_t        *addr;
    ngx_nspr_session_t        *s;
    ngx_nspr_addr_conf_t      *addr_conf;
    ngx_nspr_core_srv_conf_t  *cscf;
    u_char                     text[NGX_SOCKADDR_STRLEN];
#if (NGX_HAVE_INET6)
    struct sockaddr_in6       *sin6;
    ngx_nspr_in6_addr_t       *addr6;
#endif


    /* find the server configuration for the address:port */

    port = c->listening->servers;

    if (port->naddrs > 1) {

        /*
         * There are several addresses on this port and one of them
         * is the "*:port" wildcard so getsockname() is needed to determine
         * the server address.
         *
         * AcceptEx() already gave this address.
         */

        if (ngx_connection_local_sockaddr(c, NULL, 0) != NGX_OK) {
            ngx_nspr_close_connection(c);
            return;
        }

        sa = c->local_sockaddr;

        switch (sa->sa_family) {

#if (NGX_HAVE_INET6)
        case AF_INET6:
            sin6 = (struct sockaddr_in6 *) sa;

            addr6 = port->addrs;

            /* the last address is "*" */

            for (i = 0; i < port->naddrs - 1; i++) {
                if (ngx_memcmp(&addr6[i].addr6, &sin6->sin6_addr, 16) == 0) {
                    break;
                }
            }

            addr_conf = &addr6[i].conf;

            break;
#endif

        default: /* AF_INET */
            sin = (struct sockaddr_in *) sa;

            addr = port->addrs;

            /* the last address is "*" */

            for (i = 0; i < port->naddrs - 1; i++) {
                if (addr[i].addr == sin->sin_addr.s_addr) {
                    break;
                }
            }

            addr_conf = &addr[i].conf;

            break;
        }

    } else {
        switch (c->local_sockaddr->sa_family) {

#if (NGX_HAVE_INET6)
        case AF_INET6:
            addr6 = port->addrs;
            addr_conf = &addr6[0].conf;
            break;
#endif

        default: /* AF_INET */
            addr = port->addrs;
            addr_conf = &addr[0].conf;
            break;
        }
    }

    s = ngx_pcalloc(c->pool, sizeof(ngx_nspr_session_t));
    if (s == NULL) {
        ngx_nspr_close_connection(c);
        return;
    }

    s->signature = NGX_NSPR_MODULE;

    s->main_conf = addr_conf->ctx->main_conf;
    s->srv_conf = addr_conf->ctx->srv_conf;

    s->addr_text = &addr_conf->addr_text;

    c->data = s;
    s->connection = c;

    cscf = ngx_nspr_get_module_srv_conf(s, ngx_nspr_core_module);

    ngx_set_connection_log(c, cscf->error_log);

    len = ngx_sock_ntop(c->sockaddr, c->socklen, text, NGX_SOCKADDR_STRLEN, 1);

    ngx_log_error(NGX_LOG_INFO, c->log, 0, "*%uA client %*s connected to %V",
                  c->number, len, text, s->addr_text);

    ctx = ngx_palloc(c->pool, sizeof(ngx_nspr_log_ctx_t));
    if (ctx == NULL) {
        ngx_nspr_close_connection(c);
        return;
    }

    ctx->client = &c->addr_text;
    ctx->session = s;

    c->log->connection = c->number;
    c->log->handler = ngx_nspr_log_error;
    c->log->data = ctx;
    c->log->action = "sending client greeting line";

    c->log_error = NGX_ERROR_INFO;

    ngx_nspr_init_session(c);
    ngx_nspr_cycle(s);
}


void nspr_app_core_all_connection_init()
{
    ngx_nspr_callback_t cb = {NULL, NULL};
    cb.nspr_connect_peer_t = ngx_nspr_connect_peer;
    cb.nspr_post_ioevent_t = ngx_nspr_post_ioevent;
    nspr_app_core_set_callback(cb);
}

static void ngx_nspr_post_ioevent(ngx_event_t *ev)
{
    ngx_post_event(ev, &ngx_posted_events);
}

static void
ngx_nspr_close_session_handler(ngx_event_t *e)
{
    ngx_nspr_session_t                 *s;
    ngx_connection_t                   *c;

    s = e->data;
    c = s->connection;
    nspr_app_core_tcp_destroy_connection(c);

    ngx_log_error(NGX_LOG_DEBUG, c->log, 0, "close session");
    
    ngx_nspr_close_connection(c);                                                                                                                                                                                
}

static void
ngx_nspr_init_session(ngx_connection_t *c)
{
    ngx_nspr_session_t        *s;
    s = c->data;

    ngx_event_t        *e;
    e = &s->close;
    e->data = s;
    e->handler = ngx_nspr_close_session_handler;
    e->log = c->log;

    nspr_app_core_tcp_create_connection(c);
}


void
ngx_nspr_close_connection(ngx_connection_t *c)
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

ngx_connection_t *ngx_nspr_connect_peer(u_char *url)
{
    if (!ngx_strncmp(url, "udp", 3)) {
        return ngx_nspr_udp_connect_peer(url);
    }
    return ngx_nspr_tcp_connect_peer(url);
}

static ngx_connection_t *ngx_nspr_tcp_connect_peer(u_char *url)
{
    ngx_int_t rc;
    ngx_url_t u;
    ngx_nspr_session_t        *s;
    ngx_connection_t              *c;
    ngx_pool_t                    *pool;

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

    ngx_peer_connection_t peer;
    ngx_memzero(&peer, sizeof(peer));

    peer.sockaddr = u.addrs->sockaddr;
    peer.socklen = u.addrs->socklen;
    peer.name = &u.host;
    peer.get = ngx_event_get_peer;
    peer.log = ngx_cycle->log;
    peer.log_error = NGX_ERROR_INFO;

    if (!ngx_strncmp(url, "udp", 3)) {   
        peer.type = SOCK_DGRAM;
    }

    rc = ngx_event_connect_peer(&peer);

    if (rc == NGX_ERROR || rc == NGX_BUSY || rc == NGX_DECLINED) {
        ngx_log_error(NGX_LOG_ERR, ngx_cycle->log, 0, "oceanus connect peer failed");
        return NULL;
    }
    ngx_log_error(NGX_LOG_ERR, ngx_cycle->log, 0, "connect result:%d, EINPROGRESS:%d", rc, EINPROGRESS);

    s = ngx_pcalloc(pool, sizeof(ngx_nspr_session_t));
    if (s == NULL) {
        return NULL;
    }

    c = peer.connection;
    c->pool = pool;
    c->data = s;
    c->log = ngx_cycle->log;
    c->read->handler = nspr_app_core_tcp_recv;
    c->write->handler = nspr_app_core_tcp_send;
    c->log_error = NGX_ERROR_INFO;

    s->signature = NGX_NSPR_MODULE;
    s->connection = c;
    
    ngx_nspr_init_session(c);
    return c;
}

void ngx_nspr_disconnect_peer(ngx_connection_t *c)
{
    (void)ngx_nspr_log_error;
}

u_char *
ngx_nspr_log_error(ngx_log_t *log, u_char *buf, size_t len)
{
    u_char              *p = 0;
    ngx_nspr_session_t  *s;
    ngx_nspr_log_ctx_t  *ctx;

    if (log->action) {
        p = ngx_snprintf(buf, len, " while %s", log->action);
        len -= p - buf;
        buf = p;
    }

    ctx = log->data;

    /*
    p = ngx_snprintf(buf, len, ", client: %V", ctx->client);
    len -= p - buf;
    buf = p;
    */

    s = ctx->session;

    if (s == NULL) {
        return p;
    }

    p = ngx_snprintf(buf, len, "%s, server: %V",
                     /*s->starttls*/0 ? " using starttls" : "",
                     s->addr_text);
    len -= p - buf;
    buf = p;

    if (s->login.len == 0) {
        return p;
    }

    p = ngx_snprintf(buf, len, ", login: \"%V\"", &s->login);
    len -= p - buf;
    buf = p;

    /*
    if (s->proxy == NULL) {
        return p;
    }

    p = ngx_snprintf(buf, len, ", upstream: %V", s->proxy->upstream.name);
    */

    return p;
}

void nspr_app_core_tcp_recv(ngx_event_t *rev)
{
    ngx_int_t                   n;
    ngx_connection_t           *c;
    ngx_nspr_session_t     *s;

    c = (ngx_connection_t *)rev->data;
    s = (ngx_nspr_session_t *)c->data;

    if (c->destroyed) {
        return;
    }

    if (rev->timedout) {
        ngx_log_error(NGX_LOG_WARN, c->log, NGX_ETIMEDOUT, 
                "cycle: recv: client timed out");
        c->timedout = 1;
        ngx_post_event(&s->close, &ngx_posted_events);
        return;
    }

    if (rev->timer_set) {
        ngx_del_timer(rev); 
    }

    ngx_buf_t *b = nspr_app_core_get_recv_buffer(s);

    u_char *last = b->last;
    do {
        n = c->recv(c, b->last, b->end - b->last);

        if (n == NGX_ERROR || n == 0) {
            ngx_post_event(&s->close, &ngx_posted_events);
            return;
        }

        ngx_uint_t time_out = 6000000;
        if (n == NGX_AGAIN) {
            ngx_add_timer(rev, time_out);
            if (ngx_handle_read_event(c->read, 0) != NGX_OK) {
                ngx_log_error(NGX_LOG_ERR, c->log, 0, "ngx_handle_read_event error");
                ngx_post_event(&s->close, &ngx_posted_events);
                return;
            }
            if (b->last > last) {
                nspr_app_core_confirm_recv_buffer(s);
            }
            return;
        }
        b->last += n;
    } while (b->last != b->end);
    nspr_app_core_confirm_recv_buffer(s);

    ngx_post_event(rev, &ngx_posted_events);
}

void nspr_app_core_tcp_send(ngx_event_t *wev)
{
    ngx_int_t                   n;
    ngx_connection_t           *c;
    ngx_nspr_session_t     *s;

    c = (ngx_connection_t *)wev->data;
    s = (ngx_nspr_session_t *)c->data;

    ngx_log_error(NGX_LOG_DEBUG, c->log, 0, "tcp send was posted");
    if (c->destroyed) {
        return;
    }

    if (wev->timedout) {
        ngx_log_error(NGX_LOG_WARN, c->log, NGX_ETIMEDOUT, 
                "cycle: send: client timed out");
        c->timedout = 1;
        ngx_post_event(&s->close, &ngx_posted_events);
        return;
    }

    if (wev->timer_set) {
        ngx_del_timer(wev);    
    }

    if (!wev->ready) {
        ngx_log_error(NGX_LOG_ERR, c->log, 0, "connected.");         
        //wev->ready = 1;
        return;
    }

    ngx_buf_t *b = nspr_app_core_get_send_buffer(s);

    if (!b) {
        ngx_log_error(NGX_LOG_WARN, c->log, 0, "no data to send");
        return;
    }

    while (b->last != b->pos) {
        n = c->send(c, b->pos, b->last - b->pos);

        if (n == NGX_ERROR) {
            ngx_post_event(&s->close, &ngx_posted_events);
            return;
        }

        ngx_uint_t time_out = 6000000;
        if (n == NGX_AGAIN || n == 0) {
            ngx_add_timer(c->write, time_out);
            if (ngx_handle_write_event(c->write, 0) != NGX_OK) {
                ngx_log_error(NGX_LOG_ERR, c->log, 0, "ngx_handle_write_event error");
                ngx_post_event(&s->close, &ngx_posted_events);
                return;
            }
            return;
        }
        b->pos += n;
    }

    nspr_app_core_confirm_send_buffer(s);
    
    if (wev->active) {
        ngx_del_event(wev, NGX_WRITE_EVENT, 0);
    }
}

void ngx_nspr_cycle(ngx_nspr_session_t *s)
{
    ngx_connection_t           *c;  

    c = s->connection;
    c->read->handler =  nspr_app_core_tcp_recv;
    c->write->handler = nspr_app_core_tcp_send;

    ngx_log_debug0(NGX_LOG_INFO, s->connection->log, 0,
            "cycle: start server cycle");

    (c->read->handler)(c->read);
}

