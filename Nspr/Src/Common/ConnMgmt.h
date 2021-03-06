#ifndef __NSPR_CONN_MGMT_H_INCLUDED__
#define __NSPR_CONN_MGMT_H_INCLUDED__

#include <map>

namespace nspr
{

class ProtocolConnection;
class ConnMgmt
{
public:
	ConnMgmt();
	~ConnMgmt();

    void AddConnection(const u_char *connId, ProtocolConnection *pc);
	ProtocolConnection *GetConnection(const u_char *connId);
    ProtocolConnection *DelConnection(const u_char *connId);
	int GetAllConnId(u_char ***connIdArray, int *count);

private:
    std::map<std::string, ProtocolConnection *> m_store;

}; // class ConnMgmt

} // namespace nspr

#endif // ifndef __NSPR_CONN_MGMT_H_INCLUDED__

