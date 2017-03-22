#ifndef __NSPR_BOX_CELL_TRUN_H_INCLUDED__
#define __NSPR_BOX_CELL_TRUN_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellTrun : public BoxCell
{
public:
	BoxCellTrun();
	~BoxCellTrun();

	virtual int Divide(u_char *data, size_t len);
    virtual int Fuse(u_char *data, size_t &len);
	virtual bool isComposite();

    void SetDataOffset(int offset) { m_dataOffset = offset; }
    void SetSampleCount(int sampleCount) { m_sampleCount = sampleCount; }
    void SetSampleLen(int index, int len) { m_sampleLen[index] = len; }
    uint32_t *GetDataOffsetPtr() { return m_dataOffsetPtr; }

private:
    const static int m_maxSampleCount = 100;
    int m_dataOffset;
    int m_sampleCount;
    int m_sampleLen[m_maxSampleCount];
    uint32_t *m_dataOffsetPtr;
}; // class BoxCellTrun

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_TRUN_H_INCLUDED__

