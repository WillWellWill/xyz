#include "Nspr.h"

#include "PortMgmt.h"


namespace nspr
{

PortMgmt::PortMgmt()
{
}

PortMgmt::~PortMgmt()
{
}
	
int PortMgmt::GetPublishPort()
{
	return 12000;
}
	
int PortMgmt::GetPlayPort()
{
	return 22000;
}

} // namespace nspr

