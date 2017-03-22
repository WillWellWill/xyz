#ifndef __NSPR_BOX_CELL_TFDT_H_INCLUDED__
#define __NSPR_BOX_CELL_TFDT_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellTfdt : public BoxCell
{
public:
	BoxCellTfdt();
	~BoxCellTfdt();

	virtual int Divide(u_char *data, size_t len);
    virtual int Fuse(u_char *data, size_t &len);
	virtual bool isComposite();

    void SetDecodeTime(int decodeTime) { m_decodeTime = decodeTime; }

private:
    int m_decodeTime;
}; // class BoxCellTfdt

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_TFDT_H_INCLUDED__

