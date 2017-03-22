#include "Nspr.h"

#include "Metadata.h"
#include "ProtocolConnection.h"
#include "PortMgmt.h"
#include "ConnMgmt.h"

namespace nspr
{

ProtocolConnection::ProtocolConnection(const u_char *connId, const u_char *url, ProtocolStatus *status)
    : MsgConnection(url)
	, m_metadata(new Metadata())
    , m_connId(std::string((const char *)connId))
	, m_connectStatus(std::string("init"))
{
	memset(m_metadata, 0, sizeof(Metadata));
    m_icb = new NsprInteractClientCB(this, status);
    m_interactClient = new InteractClient((char *)url, m_icb);
}

ProtocolConnection::~ProtocolConnection()
{
    NsprInfo("free %s", m_connId.data());
    Singleton<ConnMgmt>::GetInstance()->DelConnection((const u_char *)m_connId.data());
    delete m_interactClient;
    delete m_icb;
	delete m_metadata;
}

int ProtocolConnection::Connect(bool isPublish)
{
	m_connectStatus = "connecting";
    if (Connect()) {
        NsprError("msg connect failed");
		m_connectStatus = "error";
        return NSPR_ERROR;
    }
    Metadata metadata;
    memset(&metadata, 0, sizeof(metadata));
	if (isPublish) {
		metadata.mode = IC_MODE_PUBLISH;
		metadata.v.srcPort = Singleton<PortMgmt>::GetInstance()->GetPublishPort();
		metadata.a.srcPort = Singleton<PortMgmt>::GetInstance()->GetPublishPort();
	}
	else {
		metadata.mode = IC_MODE_PLAY;
		metadata.v.dstPort = Singleton<PortMgmt>::GetInstance()->GetPlayPort();
		metadata.a.dstPort = Singleton<PortMgmt>::GetInstance()->GetPlayPort();
	}
    if (m_interactClient->Connect(&metadata)) {
        return NSPR_ERROR;
    }
    return NSPR_OK;
}
	
int ProtocolConnection::Publish()
{
	m_connectStatus = "connected";
	return m_interactClient->Publish();
}

int ProtocolConnection::Play()
{
	m_connectStatus = "connected";
	return m_interactClient->Play();
}
    
void ProtocolConnection::Close()
{
	m_connectStatus = "disconnecting";
    m_interactClient->Close();
}

int ProtocolConnection::OnRecv(const u_char *msg, size_t len)
{
    m_interactClient->DataIn(msg, len);
    return NSPR_OK;
}

NsprInteractClientCB::NsprInteractClientCB(ProtocolConnection *protocolConnection, ProtocolStatus *status)
    : m_protocolConnection(protocolConnection)
    , m_status(status)
{
}

NsprInteractClientCB::~NsprInteractClientCB()
{
    if (m_status) {
        delete m_status;
    }
}

int NsprInteractClientCB::DataOut(const uint8_t *data, size_t len)
{
    NsprDebug("ready to send %d", len);
    return m_protocolConnection->Send(data, len);
}

int NsprInteractClientCB::OnStatus(Status status, Metadata *metadata)
{
    if (status == Interact::InteractClientCB::CONNECT_DONE) {
        NsprInfo("video dst port = %d, audio dst port = %d", metadata->v.dstPort, metadata->a.dstPort);
        m_status->OnConnected(m_protocolConnection->GetConnId(), metadata);
		if (metadata->mode == IC_MODE_PUBLISH) {
			NsprInfo("excute command publish");
			m_protocolConnection->Publish();
		}
		else if (metadata->mode == IC_MODE_PLAY) {
			NsprInfo("excute command play");
			m_protocolConnection->Play();
		}
		m_protocolConnection->SetMetadata(metadata);
    }
    else if (status == Interact::InteractClientCB::DESTROYED) {
        m_status->OnDestroyed(m_protocolConnection->GetConnId());
        ((MsgConnection *)m_protocolConnection)->Close();
    }
	else if (status == Interact::InteractClientCB::PUBLISHING
		  || status == Interact::InteractClientCB::PLAYING) {
		m_status->OnReady(m_protocolConnection->GetConnId());
	}
    return NSPR_OK;
}

void NsprInteractClientCB::Log(const uint8_t *logData, size_t logLen)
{
    nspr_app_core_write_log((const char *)logData, logLen);
}


} // namespace nspr
