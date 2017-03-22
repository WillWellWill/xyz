#include "Nspr.h"

#include "Utility.h"

namespace nspr
{

Utility::Utility()
{
}

Utility::~Utility()
{
}
	
void Utility::IpFromConnId(u_char *connId, u_char *ip, size_t ipLen)
{
	char ipTmp[128] = {0};
    char *ipBeg = strchr((char *)connId, '-');
    if (!ipBeg) {
        memcpy(ip, "0.0.0.0", 8);
    }
    else {
        char *ipEnd = strchr(ipBeg + 1, '-');
        if (!ipEnd) {
            memcpy(ip, "0.0.0.0", 8);
        }
        else {
			++ipBeg;
            memcpy(ipTmp, ipBeg, (ipEnd - ipBeg));
            int i, j = 0;
            bool notZeroStart = false;
            for (i = 0; i < 12; i++) {
                if (i && i % 3 == 0) {
                    ip[j++] = '.';
                    notZeroStart = false;
                }
                if (ipTmp[i] != '0' || notZeroStart || (i + 1) % 3 == 0) {
                    ip[j++] = ipTmp[i];
                    notZeroStart = true;
                }
				if (j >= (int)ipLen) {
					return;
				}
            }
			ip[j] = '\0';
        }
    }
}

} // namespace nspr

