#include "Nspr.h"

typedef unsigned char u_char;
#include "InfoSummary.h"

namespace nspr
{

InfoSummary::InfoSummary()
{

}

InfoSummary::~InfoSummary()
{

}

int InfoSummary::Summary(const u_char *info)
{
    return NSPR_OK;
}

int InfoSummary::Print()
{
    return NSPR_OK;
}

} // namespace nspr

