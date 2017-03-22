#ifndef __NSPR_BOX_CELL_STSD_H_INCLUDED__
#define __NSPR_BOX_CELL_STSD_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellStsd : public BoxCell
{
public:
	BoxCellStsd();
	~BoxCellStsd();

	virtual int Divide(u_char *data, size_t len);
    virtual int Fuse(u_char *data, size_t &len);
	virtual bool isComposite();

    void SetSps(u_char *sps, int spsLen);
    void SetPps(u_char *pps, int ppsLen);

    void SetWidth(int width) { m_width = width; }
    void SetHeight(int height) { m_height = height; }

private:
    const static int m_maxSpsLen = 128;
    const static int m_maxPpsLen = 128;
    u_char m_sps[m_maxSpsLen];
    int m_spsLen;

    u_char m_pps[m_maxPpsLen];
    int m_ppsLen;

    int m_width;
    int m_height;

private:
    u_char *WriteAVCDecoderConfigurationRecord(u_char *p);
}; // class BoxCellStsd

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_STSD_H_INCLUDED__

