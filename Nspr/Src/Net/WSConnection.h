#ifndef __NSPR_WEBSOCKET_CONNECTION__
#define __NSPR_WEBSOCKET_CONNECTION__

#include "MsgConnection.h"
#include "StreamProc.h"

namespace nspr
{

class WSStreamProc;
class WSConnection : public MsgConnection
{
public:
    WSConnection(ngx_connection_t *c);
    virtual ~WSConnection();

    int Send(bool isText, const u_char *msg, size_t len);
    virtual int OnRecv(const u_char *msg, size_t len);
    

private:
    bool handshakeFinished;
    WSStreamProc *m_wsStreamProc;
    StreamProc *m_broProc;

private:
    int ProcessHandshake(u_char *msg, size_t len);
    int ProcessData(u_char *msg, size_t len);
    int GenerateKeyInfo(u_char *msg, size_t len, u_char *respKey, int *respKeyLen);
    int ResponseHandshake(char *keyInfo);
    int GetValue(uint8_t *message, size_t len, const char *key, char **value, size_t *valueLen);

    int BuildChannel(u_char *msg);
    void DestroyChannel();

	static const int m_maxSendBufferLen = 1024 * 1024;
	u_char m_sendBuf[m_maxSendBufferLen];
}; // class WSConnection

class WSStreamProc : public StreamProc
{
public:
    WSStreamProc(WSConnection *wsConnection) : m_wsConnection(wsConnection), m_isText(false) {}

    void EnableTextMode() { m_isText = true; }
    virtual int OnHandleTx(u_char *data, size_t len) { return m_wsConnection->Send(m_isText, data, len); }
    virtual int OnHandleRx(u_char *data, size_t len) { return RelayRx(data, len); }

private:
    WSConnection *m_wsConnection;
    bool m_isText;

}; // class WSStreamProc

} // namespace nspr


#endif // __NSPR_WEBSOCKET_CONNECTION__

