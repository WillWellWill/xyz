#ifndef __NSPR_PROTOCOL_CONNECTION_H_INCLUDED__
#define __NSPR_PROTOCOL_CONNECTION_H_INCLUDED__

#include "Metadata.h"
#include "MsgConnection.h"
#include "InteractClient.h"

using namespace Interact;

namespace nspr
{

class ProtocolStatus
{
public:
    virtual ~ProtocolStatus() {}
    virtual void OnConnected(const u_char *connId, Metadata *metadata) = 0;
    virtual void OnDestroyed(const u_char *connId) = 0;
    virtual void OnReady(const u_char *connId) = 0;

}; // class ProtocolStatus

class NsprInteractClientCB;
class ProtocolConnection : public MsgConnection
{
public:
    ProtocolConnection(const u_char *connId, const u_char *url, ProtocolStatus *status);
    virtual ~ProtocolConnection();

    virtual int OnRecv(const u_char *msg, size_t len);
    virtual int Connect() { return MsgConnection::Connect(); }
    int Connect(bool isPublish);
	int Publish();
	int Play();

    virtual void Close();

    const u_char *GetConnId() { return (const u_char *)m_connId.data(); }
    const u_char *GetConnSt() { return (const u_char *)m_connectStatus.data(); }

	void SetMetadata(Metadata *metadata) { *m_metadata = *metadata; }
	Metadata *GetMetadata() { return m_metadata; }

private:
    NsprInteractClientCB *m_icb;
    InteractClient *m_interactClient;
	Metadata *m_metadata;
    std::string m_connId;
	std::string m_connectStatus;

}; // class ProtocolConnection

class NsprInteractClientCB : public Interact::InteractClientCB
{
public:
    NsprInteractClientCB(ProtocolConnection *protocolConnection, ProtocolStatus *status);
    virtual ~NsprInteractClientCB();

    virtual int DataOut(const uint8_t *data, size_t len);
    virtual int OnStatus(Status status, Metadata *metadata);
    virtual void Log(const uint8_t *logData, size_t logLen);

private:
    ProtocolConnection *m_protocolConnection;
    ProtocolStatus *m_status;

}; // class NsprInteractClientCB

} // namespace nspr


#endif // ifndef __NSPR_PROTOCOL_CONNECTION_H_INCLUDED__

