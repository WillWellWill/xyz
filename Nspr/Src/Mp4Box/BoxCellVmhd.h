#ifndef __NSPR_BOX_CELL_VMHD_H_INCLUDED__
#define __NSPR_BOX_CELL_VMHD_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellVmhd : public BoxCell
{
public:
	BoxCellVmhd();
	~BoxCellVmhd();

	virtual int Divide(u_char *data, size_t len);
    virtual int Fuse(u_char *data, size_t &len);
	virtual bool isComposite();
}; // class BoxCellVmhd

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_VMHD_H_INCLUDED__

