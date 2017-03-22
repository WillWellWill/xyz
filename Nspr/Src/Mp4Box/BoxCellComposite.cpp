#include "Nspr.h"

#include "BoxCellComposite.h"

namespace nspr
{

BoxCellComposite::BoxCellComposite()
{

}

BoxCellComposite::~BoxCellComposite()
{

}

int BoxCellComposite::Divide(u_char *data, size_t len)
{
    printf("should not divide\n");
    return NSPR_ERROR;
}

bool BoxCellComposite::isComposite()
{
    return true;
}

} // namespace nspr

