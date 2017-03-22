#ifndef __NSPR_PROTOCOL_SPONSOR_H_INCLUDED__
#define __NSPR_PROTOCOL_SPONSOR_H_INCLUDED__

namespace nspr
{

class ProtocolStatus;
class ConnMgmt;
class ProtocolConnection;
class ProtocolSponsor
{
public:
    ProtocolSponsor();
    ~ProtocolSponsor();

    ProtocolConnection *CreateConnection(const char *url, bool isPublish, ProtocolStatus *status);
    int Disconnect(const u_char *connId);
	ProtocolConnection *SearchConnection(const u_char *connId);
	int GetAllConnId(u_char ***connIdArray, int *count);

private:
    ConnMgmt *m_connMgmt;

private:
    void GenerateConnId(const char *url, bool isPublish, u_char *connId, size_t len);

}; // class ProtocolSponsor

} // namespace nspr


#endif // ifndef __NSPR_PROTOCOL_SPONSOR_H_INCLUDED__

