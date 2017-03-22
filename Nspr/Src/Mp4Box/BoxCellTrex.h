#ifndef __NSPR_BOX_CELL_TREX_H_INCLUDED__
#define __NSPR_BOX_CELL_TREX_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellTrex : public BoxCell
{
public:
	BoxCellTrex();
	~BoxCellTrex();

	virtual int Divide(u_char *data, size_t len);
    virtual int Fuse(u_char *data, size_t &len);
	virtual bool isComposite();
}; // class BoxCellTrex

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_TREX_H_INCLUDED__

