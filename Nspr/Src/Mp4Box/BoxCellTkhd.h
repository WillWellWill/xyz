#ifndef __NSPR_BOX_CELL_TKHD_H_INCLUDED__
#define __NSPR_BOX_CELL_TKHD_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellTkhd : public BoxCell
{
public:
	BoxCellTkhd();
	~BoxCellTkhd();

	virtual int Divide(u_char *data, size_t len);
    virtual int Fuse(u_char *data, size_t &len);
	virtual bool isComposite();

    void SetWidth(int width) { m_width = width; }
    void SetHeight(int height) { m_height = height; }

private:
    int m_width;
    int m_height;
    static const int m_maxDataLen = 1024;
    u_char m_defaultData[m_maxDataLen];
    int m_defaultDataLen;

}; // class BoxCellTkhd

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_TKHD_H_INCLUDED__

