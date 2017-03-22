#include "Nspr.h"

#include "BoxCellMdat.h"


namespace nspr
{

BoxCellMdat::BoxCellMdat()
{
}

BoxCellMdat::~BoxCellMdat()
{
}

int BoxCellMdat::Divide(u_char *data, size_t len)
{
    NsprDebug("media data size = %d", len);
#if 0
    NsprDebug("%d, %d, %d, %d", data[0], data[1], data[2], data[3]);
    int i = 0;
    int zeroContinueCount = 0;
    bool startCode = false;
    int naluCount = 0;
    for (; i < len; i++) {
        if (data[i] == 0) {
            ++zeroContinueCount;
        }
        else if (data[i] == 1) {
            if (zeroContinueCount == 3) {
                startCode = true;
                ++naluCount;
                continue;
            }
            zeroContinueCount = 0;
        }
        else {
            zeroContinueCount = 0;
        }
    }
    NsprDebug("nalu count = %d", naluCount);
#endif
    FILE *fw = fopen("naldata.dat", "wb");
    fwrite(data, 1, len, fw);
    fclose(fw);
	return NSPR_OK;
}

bool BoxCellMdat::isComposite()
{
	return false;
}

} // namespace nspr

