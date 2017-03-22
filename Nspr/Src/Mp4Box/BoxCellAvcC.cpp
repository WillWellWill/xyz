#include "Nspr.h"

#include "BoxCellAvcC.h"


namespace nspr
{

BoxCellAvcC::BoxCellAvcC()
{
}

BoxCellAvcC::~BoxCellAvcC()
{
}

struct AVCDecoderConfigurationRecord {
    uint32_t configurationVersion;
    u_char avcProfileIndication;
    u_char profile_compatibility;
    u_char avcLevelIndication;
    u_char lengthSizeMinusOne;
    u_char numOfSequenceParameterSets;
};

int BoxCellAvcC::Divide(u_char *data, size_t len)
{
    u_char *p = data;
    NsprDebug("AVCDecoderConfigurationRecord start len: %d", len);

    uint32_t avcLen = ntohl(*((uint32_t *)p));
    p += 4;
    u_char name[5] = {0};
    memcpy(name, p, 4);
    p += 4;
    NsprDebug("checking %s, len %d", name, avcLen);

    if (avcLen <= 8) {
        NsprWarn("no data in avcC, size = %d", avcLen);
        return NSPR_OK;
    }
    
    const u_char configurationVersion = *p++;
    const u_char avcProfileIndication = *p++;
    const u_char profileCompatibility = *p++;
    const u_char avcLevelIndication = *p++;
    const u_char lengthSizeMinusOne = (*p++) & 0x03;
    const u_char numOfSequenceParameterSets = (*p++) & 0x1f;
    NsprDebug("configurationVersion = %d", configurationVersion);
    NsprDebug("avcProfileIndication = %d", avcProfileIndication);
    NsprDebug("profile_compatibility = %d", profileCompatibility);
    NsprDebug("avcLevelIndication = %d", avcLevelIndication);
    NsprDebug("lengthSizeMinusOne = %d", lengthSizeMinusOne);
    NsprDebug("numOfSequenceParameterSets = %d", numOfSequenceParameterSets);

    int i = 0;
    for (; i < numOfSequenceParameterSets; i++) {
        const uint16_t sequenceParameterSetLength = ntohs(*((uint16_t *)p));
        p += 2;
        NsprDebug("sequenceParameterSetLength = %d", sequenceParameterSetLength);
        char sps[256] = {0};
        int j = 0;
        for (; j < sequenceParameterSetLength; j++) {
            snprintf(sps + strlen(sps), sizeof(sps) - strlen(sps), "0x%x, ", *p++);
        }
        NsprDebug("sps: %s", sps);
    }
    const u_char numOfPictureParameterSets = *p++;
    for (i = 0; i < numOfPictureParameterSets; i++) {
        const uint16_t pictureParameterSetLength = ntohs(*((uint16_t *)p));
        p += 2;
        NsprDebug("pictureParameterSetLength = %d", pictureParameterSetLength);
        char pps[256] = {0};
        int j = 0;
        for (; j < pictureParameterSetLength; j++) {
            snprintf(pps + strlen(pps), sizeof(pps) - strlen(pps), "0x%x, ", *p++);
        }
        NsprDebug("pps: %s", pps);
    }

    assert(p - data == (int)len);
    NsprDebug("AVCDecoderConfigurationRecord end");
	return NSPR_OK;
}

bool BoxCellAvcC::isComposite()
{
	return false;
}

} // namespace nspr

