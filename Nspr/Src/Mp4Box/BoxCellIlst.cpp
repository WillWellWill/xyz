#include "Nspr.h"

#include "BoxCellIlst.h"


namespace nspr
{

BoxCellIlst::BoxCellIlst()
{
}

BoxCellIlst::~BoxCellIlst()
{
}

int BoxCellIlst::Divide(u_char *data, size_t len)
{
    u_char *p = data;
    const uint32_t dataSize = ntohl(*((uint32_t *)p));
    p += 4;
    NsprDebug("dataSize = %d", dataSize);
	return NSPR_OK;
}

bool BoxCellIlst::isComposite()
{
	return false;
}

} // namespace nspr

