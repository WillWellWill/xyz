#include "Nspr.h"

#include "BoxPacker.h"
#include "BoxFactory.h"
#include <errno.h>

#include "BoxCellTkhd.h"
#include "BoxCellStsd.h"
#include "BoxCellTfhd.h"
#include "BoxCellTrun.h"
#include "BoxCellTfdt.h"

namespace nspr
{

BoxPacker::BoxPacker(VideoCodec::Parameter *parameter)
    : m_factory(new BoxFactory())
    , m_frameCount(0)
    , m_maxMediaDataLen(parameter->GetBitrate() * 5/*seconds*/ / 8)
    , m_mediaLen(0)
    , m_param(parameter)
    , m_prepared(false)
    , m_headerPacker(false)
    , m_totalPackerSize(0)
    , m_timestamp(0)
    , m_decodeTime(0)
{
    m_mediaData = new u_char[m_maxMediaDataLen];
    NsprInfo("max media data len = %d", m_maxMediaDataLen);
}

BoxPacker::~BoxPacker()
{
    delete []m_mediaData;
    delete m_factory;
}

int BoxPacker::WriteBox(u_char *type, u_char *data, size_t &len)
{
    u_char *p = data;
    uint32_t &size = *((uint32_t *)p);
    p += 4;
    memcpy(p, type, 4);
    p += 4;
    BoxCell *bc = m_factory->GetBoxCell(type);
    NsprDebug("bc = %p", bc);
    size_t boxLen = len - (p - data);
    if (bc->Fuse(p, boxLen) == NSPR_ERROR) {
        return NSPR_ERROR;
    }
    NsprDebug("fuse ending = %p", bc);
    p += boxLen;
    len = p - data;
    size = htonl(len);
    return NSPR_OK;
}

#define WRITE_BOX(box_type) \
    boxLen = len - (p - data); \
    if (WriteBox((u_char *)(#box_type), p, boxLen)) { \
        NsprError("write " #box_type " box failed"); \
        return NSPR_ERROR; \
    } \
    p += boxLen


#define CONTAINER_BOX_BEGIN(box_type, box_begin, box_size) \
        NsprDebug(#box_type " box pack begin"); \
        u_char *box_begin = p; \
        uint32_t &box_size = *((uint32_t *)p); \
        p += 4; \
        memcpy(p, #box_type, 4); \
        p += 4

#define CONTAINER_BOX_END(box_type, box_begin, box_size) \
        NsprDebug(#box_type " box pack end"); \
        box_size = ntohl(p - box_begin)
        

#define MAX_FRAME_COUNT 1024
int BoxPacker::ProcessH264Binary(uint8_t *data, size_t len)
{
    int zeroContinueCount = 0, i = 0, lastIndex = 0;
    
    uint8_t *start = m_mediaData + m_mediaLen,
            *p = start;
    uint32_t *lastNaluLen = nullptr;
    for (i = 0; i < (int)len; i++) {
        if (data[i] == 0) {
            *p++ = *(data + i);
            ++zeroContinueCount;
        }
        else if ((data[i] == 1) && (zeroContinueCount == 2 || zeroContinueCount == 3)) {
            if (lastNaluLen) {
                *lastNaluLen = htonl(i - zeroContinueCount - lastIndex - 1);
            }
            p -= zeroContinueCount;

            lastNaluLen = (uint32_t *)p;
            p += 4;
            lastIndex = i;
            continue;
        }
        else {
            *p++ = *(data + i);
            zeroContinueCount = 0;
        }
    }
    *lastNaluLen = htonl(len - lastIndex - 1);

    PreparePacker(start, p - start);
    
    m_mediaLen += p - start;
    m_frameSize[m_frameCount++] = p - start;
    return NSPR_OK;
}
    
int BoxPacker::InjectFrame(u_char *data, size_t len)
{
    NsprDebug("inject frame len:%d", len);
	if (!m_headerPacker && data[4] != 0x67) {
		return NSPR_ERROR;
	}
	else if (m_headerPacker && data[4] == 0x67) {
		m_headerPacker = false;
	}
    ProcessH264Binary(data, len);
    m_timestamp += 40000;
	return NSPR_OK;
}

void BoxPacker::PreparePacker(u_char *data, size_t len)
{
    if (data[4] != 0x67 || m_prepared) {
        return;
    }
    BoxCellStsd *stsd = (BoxCellStsd *)m_factory->GetBoxCell((u_char *)"stsd");
    
    u_char *sps = data + 4;
    const int spsLen = ntohl(*(uint32_t *)data);
    stsd->SetSps(sps, spsLen);
    u_char *pps = data + 4 + spsLen + 4;
    const int ppsLen = ntohl(*(uint32_t *)(data + 4 + spsLen));
    stsd->SetPps(pps, ppsLen);

    stsd->SetWidth(m_param->GetWidth());
    stsd->SetHeight(m_param->GetHeight());
    
    BoxCellTkhd *tkhd = (BoxCellTkhd *)m_factory->GetBoxCell((u_char *)"tkhd");
    tkhd->SetWidth(m_param->GetWidth());
    tkhd->SetHeight(m_param->GetHeight());

    NsprDebug("Set sps len: %d, pps len: %d, width: %d, height: %d"
            , spsLen, ppsLen, m_param->GetWidth(), m_param->GetHeight());
    m_prepared = true;
}

int BoxPacker::Pack(u_char *data, size_t& len)
{
    u_char *p = data;
    size_t boxLen = 0;
    
    BoxCellTfdt *tfdt = (BoxCellTfdt *)m_factory->GetBoxCell((u_char *)"tfdt");
    tfdt->SetDecodeTime(m_decodeTime);
    m_decodeTime = m_timestamp;
    if (1) {//!m_headerPacker) {
    WRITE_BOX(ftyp);

    CONTAINER_BOX_BEGIN(moov, moovBegin, moovSize);

      WRITE_BOX(mvhd);

      CONTAINER_BOX_BEGIN(trak, trakBegin, trakSize);
      
        WRITE_BOX(tkhd);
      
        CONTAINER_BOX_BEGIN(mdia, mdiaBegin, mdiaSize);
          WRITE_BOX(mdhd);
          WRITE_BOX(hdlr);
        
          CONTAINER_BOX_BEGIN(minf, minfBegin, minfSize);
            WRITE_BOX(vmhd);
            CONTAINER_BOX_BEGIN(dinf, dinfBegin, dinfSize);
              WRITE_BOX(dref);
            CONTAINER_BOX_END(dinf, dinfBegin, dinfSize);
            
            CONTAINER_BOX_BEGIN(stbl, stblBegin, stblSize);
              WRITE_BOX(stsd);
              WRITE_BOX(stts);
              WRITE_BOX(stsc);
              WRITE_BOX(stsz);
              WRITE_BOX(stco);
            CONTAINER_BOX_END(stbl, stblBegin, stblSize);
          CONTAINER_BOX_END(minf, minfBegin, minfSize);
        CONTAINER_BOX_END(mdia, mdiaBegin, mdiaSize);

      CONTAINER_BOX_END(trak, trakBegin, trakSize);
      
      CONTAINER_BOX_BEGIN(mvex, mvexBegin, mvexSize);
        WRITE_BOX(trex);
      CONTAINER_BOX_END(mvex, mvexBegin, mvexSize);
      CONTAINER_BOX_BEGIN(udta, udtaBegin, udtaSize);
        WRITE_BOX(meta);
      CONTAINER_BOX_END(udta, udtaBegin, udtaSize);

    CONTAINER_BOX_END(moov, moovBegin, moovSize);
    }
    // else {return NSPR_ERROR;}
    CONTAINER_BOX_BEGIN(moof, moofBegin, moofSize);
      WRITE_BOX(mfhd);

    const int moofOffset = moofBegin - data;
    NsprDebug("moov offset = %d", moofOffset);
    BoxCellTfhd *tfhd = (BoxCellTfhd *)m_factory->GetBoxCell((u_char *)"tfhd");
    tfhd->SetMoofOffset(moofOffset + m_totalPackerSize);
    tfhd->SetDefaultSampleSize(m_frameSize[0]);
    
    BoxCellTrun *trun = (BoxCellTrun *)m_factory->GetBoxCell((u_char *)"trun");
    trun->SetDataOffset(0);
    trun->SetSampleCount(m_frameCount);
    int i = 0;
    for (; i < m_frameCount; i++) {
        trun->SetSampleLen(i, m_frameSize[i]);
    }

      CONTAINER_BOX_BEGIN(traf, trafBegin, trafSize);
        WRITE_BOX(tfhd);
        WRITE_BOX(tfdt);
        WRITE_BOX(trun);
      CONTAINER_BOX_END(traf, trafBegin, trafSize);
    CONTAINER_BOX_END(moof, moofBegin, moofSize);
    
    uint32_t *dataOffset = trun->GetDataOffsetPtr();
    *dataOffset = htonl(p - moofBegin + 8);

    CONTAINER_BOX_BEGIN(mdat, mdatBegin, mdatSize);
    memcpy(p, m_mediaData, m_mediaLen);
    p += m_mediaLen;
    CONTAINER_BOX_END(mdat, mdatBegin, mdatSize);

    len = p - data;
    //m_prepared = false;
    m_headerPacker = true;
    NsprDebug("packed len: %d, media len: %d", len, m_mediaLen);
    m_mediaLen = 0;
    m_frameCount = 0;
    m_totalPackerSize += len;
	return NSPR_OK;
}

} // namespace nspr


