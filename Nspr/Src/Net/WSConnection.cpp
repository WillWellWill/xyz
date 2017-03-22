#include "Nspr.h"

#include "WSConnection.h"
#include "StreamProc.h"
#include "StreamProcFactory.h"

#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

namespace nspr
{

struct WebsocketHeaderSt
{
    u_char opcode : 4;
    u_char rsv    : 3;
    u_char fin    : 1;

    u_char payloadLen : 7;
    u_char maskBit    : 1;
};

WSConnection::WSConnection(ngx_connection_t *c)
    : MsgConnection(c)
    , handshakeFinished(false)
    , m_wsStreamProc(new WSStreamProc(this))
    , m_broProc(nullptr)
{

}

WSConnection::~WSConnection()
{
    NsprDebug("websocket destroyed");
    DestroyChannel();
    delete m_wsStreamProc;
}

static int g_cnt = 0;
static int Write2File(const u_char *msg, size_t len)
{
    return NSPR_OK;
    char fn[128] = {0};
    snprintf(fn, 128, "./testmedia/%d.mp4", g_cnt++);
    FILE *fp = fopen(fn, "wb");
    fwrite((u_char *)msg, 1, len, fp);
    fclose(fp);
    return NSPR_OK;
}
int WSConnection::Send(bool isText, const u_char *msg, size_t len)
{
    const bool hasMask = false;

    size_t hLen = sizeof(WebsocketHeaderSt) + (hasMask ? 4 : 0);
    if (len > 65535) {
        hLen += 8;
    }
    else if (len >= 126) {
        hLen += 2;
    }

	if (hLen + len > m_maxSendBufferLen) {
		NsprError("exceed send buffer %dKB[sendbuffer:%dKB]", (hLen + len) / 1024, m_maxSendBufferLen / 1024);
		return NSPR_ERROR;
	}
    u_char *pointer = &m_sendBuf[0];

    WebsocketHeaderSt &wsh = *(WebsocketHeaderSt *)pointer;
    pointer += sizeof(WebsocketHeaderSt);
    
    ngx_memzero(&wsh, sizeof(WebsocketHeaderSt));
    wsh.opcode = isText ? 1 : 2;
    wsh.fin = 1;
    wsh.maskBit = 0;

    if (len > 65535) {
        wsh.payloadLen = 127;

        memset(pointer, 0, 4);
        pointer += 4;
        *(uint32_t *)pointer = htonl(len);
        pointer += 4;
    }
    else if (len >= 126) {
        wsh.payloadLen = 126;
        
        *(uint16_t *)pointer = htons(len);
        pointer += 2;
    }
    else {
        wsh.payloadLen = len;
    }

    assert(!hasMask);
    ngx_memcpy(pointer, msg, len);
    Write2File(msg, len);
    pointer += len;

    assert(pointer == m_sendBuf + hLen + len);
    
    return MsgConnection::Send(m_sendBuf, hLen + len);
}

int WSConnection::OnRecv(const u_char *msg, size_t len)
{
    if (len < 4) {
        return NSPR_AGAIN;
    }
    if (!handshakeFinished && strncasecmp((const char *)msg, "GET ", 4)) {
        NsprError("handshake not finished, and NOT begin with 'GET'");
        Close();
        return NSPR_ERROR;
    }

    if (!handshakeFinished) {
        return ProcessHandshake((u_char *)msg, len);
    }

    return ProcessData((u_char *)msg, len);
}
    
int WSConnection::ProcessHandshake(u_char *data, size_t dataLen)
{
    char msg[4096] = {0};
    size_t len = snprintf(msg, dataLen + 1 < sizeof(msg) ? dataLen + 1 : sizeof(msg), "%s", (char *)data);
    
    u_char *endStr = (u_char *)ngx_strstr(msg, "\r\n\r\n");
    if (!endStr) {
        NsprError("was not a complete websocket handshake message");
        return NSPR_AGAIN;
    }

    if (!(ngx_strstr(msg, "Upgrade: websocket")/* && ngx_strstr(msg, "Connection: Upgrade")*/)) {
        NsprError("was not websocket handshake message");
        return NSPR_ERROR;
    }

    char respKey[1024] = {0};
    int keyLen = sizeof(respKey) - 1;
    
    if (GenerateKeyInfo((u_char *)msg, len, (u_char *)&respKey[0], &keyLen)) {
        NsprError("generator response key failed");
        return NSPR_ERROR;
    }
    NsprDebug("key info %s dataLen = %d", respKey, dataLen);

    if (ResponseHandshake(respKey)) {
        return NSPR_ERROR;
    }
    
    handshakeFinished = true;
    BuildChannel((u_char *)msg);
    
    return endStr + 4 - (u_char *)msg;
}

int WSConnection::BuildChannel(u_char *msg)
{
    char url[512] = {0};
    char *webUrlBeg = strchr((char *)msg, '/');
    if (!webUrlBeg) {
        return NSPR_ERROR;
    }
    webUrlBeg++;
	char *webUrlEnd = (char *)strstr((char *)msg, "\r\n");
	if (!webUrlEnd) {
		NsprError("get websocket url ending failed");
		return NSPR_ERROR;
	}
	char webUrl[512] = {0};
	memcpy(webUrl, webUrlBeg, webUrlEnd - webUrlBeg);
	NsprInfo("get websocket url:%s", webUrl);

    if (!strncmp(webUrl, "interact/", 9)) {
        memcpy(url, "interact://", 11);
		int i = 0;
		do {
			url[i + 11] = *(&webUrl[9] + i);
			++i;
		} while (!isspace(*(&webUrl[9] + i)));
		
    }
    else if (!strncmp(webUrl, "log", 3)) {
        memcpy(url, "log://", 6);
    }

    m_broProc = Singleton<StreamProcFactory>::GetInstance()->GetStreamProc(url);
    m_wsStreamProc->AttachRxChannel(m_broProc);
    m_broProc->AttachTxChannel(m_wsStreamProc);
    if (m_broProc->IsTextMode()) {
        m_wsStreamProc->EnableTextMode();
    }
    return NSPR_OK;
}

void WSConnection::DestroyChannel()
{
    m_wsStreamProc->DetachRxChannel(m_broProc);
    m_broProc->DetachTxChannel(m_wsStreamProc);
}

static int NsprBase64(const unsigned char *buffer, int length, u_char *base64Data, int *base64Len)
{
    BIO *bio, *b64;
	BUF_MEM *bufferPtr;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
	BIO_write(bio, buffer, length);
	int ret = BIO_flush(bio);
	BIO_get_mem_ptr(bio, &bufferPtr);
	ret = BIO_set_close(bio, BIO_NOCLOSE);
    (void)ret;

    const int blength = (*bufferPtr).length;
    if (blength > *base64Len) {
        NsprDebug("%d > %d", blength, *base64Len);
	    BIO_free_all(bio);
        return NSPR_ERROR;
    }
    *base64Len = blength;
    memcpy(base64Data, (*bufferPtr).data, *base64Len);
	BIO_free_all(bio);
    (void)bio;
    return NSPR_OK;
}

int WSConnection::GenerateKeyInfo(u_char *msg, size_t len, u_char *respKey, int *respKeyLen)
{
    char *wsKeyStr = nullptr;
    size_t wsKeyStrLen = 0;
    if (GetValue(msg, len, "Sec-WebSocket-Key", &wsKeyStr, &wsKeyStrLen)) {
        NsprError("get value of Sec-Websocket-key failed");
        return nullptr;
    }

    char keyStr[512] = {0};
    memcpy(keyStr, wsKeyStr, wsKeyStrLen);
    memcpy(keyStr + wsKeyStrLen, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11", 36);
    size_t keyStrLen = strlen(keyStr);

    NsprDebug("key string: %s, keyStrLen = %d", keyStr, keyStrLen);

	uint8_t sha1Result[SHA_DIGEST_LENGTH] = {0};
    SHA1((u_char *)keyStr, keyStrLen, sha1Result);

    return NsprBase64(sha1Result, SHA_DIGEST_LENGTH, respKey, respKeyLen);
}

int WSConnection::GetValue(uint8_t *message, size_t len, const char *key, char **value, size_t *valueLen)
{
    char *pointer = strcasestr((char *)message, key);
    if (!pointer || (size_t)((uint8_t *)pointer - message) >= len)
    {
        return NSPR_ERROR;
    }
    pointer += strlen(key);

    while (isspace((int)*pointer) || *pointer == ':')
    {
        if ((size_t)((uint8_t *)pointer - message) > len)
        {
            return NSPR_ERROR;
        }
        ++pointer;
    }
    if (*pointer == 0)
    {
        return NSPR_ERROR;
    }
    
    char *valuePos = pointer;
    do
    {
        if ((size_t)((uint8_t *)pointer - message) > len)
        {
            return NSPR_ERROR;
        }
        ++pointer;
    } while (*pointer != 0 && !isspace(*pointer));

    *value = valuePos;
    *valueLen = pointer - valuePos;
    return NSPR_OK;
}

int WSConnection::ResponseHandshake(char *keyInfo)
{
    char response[4096] = {0};
    int len = snprintf(response, sizeof(response), "HTTP/1.1 101 Switching Protocols\r\n"
                                         "Upgrade: websocket\r\n"
                                         "Connection: Upgrade\r\n"
                                         "Sec-WebSocket-Accept: %s\r\n\r\n"
                                       , keyInfo
                                         );
    return MsgConnection::Send((u_char *)response, len);
}

int WSConnection::ProcessData(u_char *msg, size_t len)
{
    u_char *pointer = msg;
    size_t hLen = sizeof(WebsocketHeaderSt);
    NsprDebug("Currently, header len = %d, len = %d", hLen, len);
    if (len < hLen) {
        return NSPR_AGAIN;
    }
    WebsocketHeaderSt *wsHeader = (WebsocketHeaderSt *)msg;
    pointer += hLen;
    if (wsHeader->maskBit) {
        hLen += 4;
        NsprDebug("Currently, header len = %d after mask bit detected", hLen);
    }
    int dLen = wsHeader->payloadLen;
    if (dLen == 126) {
        hLen += 2;
        NsprDebug("Currently, header len = %d, payload len: %d after extended payload length detected", hLen, dLen);
        if (len < hLen) {
            return NSPR_AGAIN;
        }
        dLen = ntohs(*(uint16_t *)pointer);
        pointer += 2;
    }
    else if (dLen == 127) {
        NsprError("not support payload len > 64K");
        return NSPR_ERROR;
    }

    if (len < dLen + hLen) {
        return NSPR_AGAIN;
    }

    u_char *dstBuf = new u_char[dLen + 1];
    if (wsHeader->maskBit) {
        u_char *maskBytes = pointer;
        pointer += 4;
        int i = 0;
        for (; i + 4 <= dLen; i += 4) {
            *((uint32_t *)(dstBuf + i)) = *((uint32_t *)(pointer + i))^*((uint32_t *)maskBytes);
        }
        pointer += i;
        for (i = 0; i < dLen % 4; i++) {
            const int j = dLen - (dLen % 4) + i;
            *(dstBuf + j) = *(pointer + i)^*(maskBytes + i);
        }
    }
    else {
        ngx_memcpy(dstBuf, pointer, dLen);
    }

    switch (wsHeader->opcode) {
        case 0: NsprDebug("continual frame"); break;
        case 1: dstBuf[dLen] = 0; NsprDebug("is text: %s", dstBuf); break;
        case 2: NsprDebug("is binary, len = %d", dLen); break;
        case 8: NsprDebug("close from peer"); Close(); break;
        case 9: NsprDebug("ping from peer"); break;
        case 10: NsprDebug("pong from peer"); break;
        default: NsprError("unsupport opcode"); break;
    }

    if (m_wsStreamProc->OnHandleRx(dstBuf, dLen)) {
        return NSPR_ERROR;
    }
    
    NsprDebug("len = %d, dLen(%d) + hLen(%d) = %d", len, dLen, hLen, dLen + hLen);
    delete []dstBuf;
    return dLen + hLen;
}

} // namespace nspr

