#ifndef __NSPR_PORT_MGMT_H_INCLUDED__
#define __NSPR_PORT_MGMT_H_INCLUDED__


namespace nspr
{

class PortMgmt
{
public:
	PortMgmt();
	~PortMgmt();

	int GetPublishPort();
	int GetPlayPort();

}; // class PortMgmt

} // namespace nspr

#endif // ifndef __NSPR_PORT_MGMT_H_INCLUDED__

