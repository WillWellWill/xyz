#ifndef __NSPR_BOX_CELL_MDHD_H_INCLUDED__
#define __NSPR_BOX_CELL_MDHD_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellMdhd : public BoxCell
{
public:
	BoxCellMdhd();
	~BoxCellMdhd();

	virtual int Divide(u_char *data, size_t len);
    virtual int Fuse(u_char *data, size_t &len);
	virtual bool isComposite();

private:
    static const int m_maxDataLen = 1024;
    u_char m_defaultData[m_maxDataLen];
    int m_defaultDataLen;

}; // class BoxCellMdhd

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_MDHD_H_INCLUDED__

