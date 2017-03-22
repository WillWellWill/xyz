#include "Nspr.h"

#include "BoxCellFtyp.h"
#include <arpa/inet.h>
#include <string.h>

namespace nspr
{

BoxCellFtyp::BoxCellFtyp()
{

}

BoxCellFtyp::~BoxCellFtyp()
{

}

int BoxCellFtyp::Divide(u_char *data, size_t len)
{
    printf("major_brand = %c%c%c%c\n", data[0], data[1], data[2], data[3]);
    printf("minor_version = %d\n", ntohl(*(uint32_t *)(data + 4)));

    u_char compatibleBrands[1024] = {0};
    memcpy(compatibleBrands, data + 8, len - 8);
    printf("compatible_brands = %s\n", compatibleBrands);
    return NSPR_OK;
}
    
int BoxCellFtyp::Fuse(u_char *data, size_t &len)
{
    char majorBrand[] = "mp42";
    uint32_t minorVersion = htonl(1);
    char compatibleBrands[] = "isommp42dash";
    memcpy(data, majorBrand, 4);
    memcpy(data + 4, &minorVersion, 4);
    memcpy(data + 8, compatibleBrands, 12);

    len = 20;
    return NSPR_OK;
}

bool BoxCellFtyp::isComposite()
{
    return false;
}

} // namespace nspr


