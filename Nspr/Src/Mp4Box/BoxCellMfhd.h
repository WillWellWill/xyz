#ifndef __NSPR_BOX_CELL_MFHD_H_INCLUDED__
#define __NSPR_BOX_CELL_MFHD_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellMfhd : public BoxCell
{
public:
	BoxCellMfhd();
	~BoxCellMfhd();

	virtual int Divide(u_char *data, size_t len);
    virtual int Fuse(u_char *data, size_t &len);
	virtual bool isComposite();

private:
    int m_sequenceNumber;

}; // class BoxCellMfhd

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_MFHD_H_INCLUDED__

