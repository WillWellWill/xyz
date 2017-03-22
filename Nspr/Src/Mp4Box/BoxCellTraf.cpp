#include "Nspr.h"

#include "BoxCellTraf.h"


namespace nspr
{

BoxCellTraf::BoxCellTraf()
{
}

BoxCellTraf::~BoxCellTraf()
{
}

int BoxCellTraf::Divide(u_char *data, size_t len)
{
    NsprDebug("ignore the traf box, size = %d", len);
	return NSPR_OK;
}

bool BoxCellTraf::isComposite()
{
	return true;
}

} // namespace nspr

