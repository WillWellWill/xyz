#include "Nspr.h"

#include "BoxCellDummy.h"


namespace nspr
{

BoxCellDummy::BoxCellDummy()
{
}

BoxCellDummy::~BoxCellDummy()
{
}

int BoxCellDummy::Divide(u_char *data, size_t len)
{
    printf("A dummy box, size = %zu\n", len);
	return NSPR_OK;
}

bool BoxCellDummy::isComposite()
{
	return false;
}

} // namespace nspr

