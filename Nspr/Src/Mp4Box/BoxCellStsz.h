#ifndef __NSPR_BOX_CELL_STSZ_H_INCLUDED__
#define __NSPR_BOX_CELL_STSZ_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellStsz : public BoxCell
{
public:
	BoxCellStsz();
	~BoxCellStsz();

	virtual int Divide(u_char *data, size_t len);
    virtual int Fuse(u_char *data, size_t &len);
	virtual bool isComposite();
}; // class BoxCellStsz

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_STSZ_H_INCLUDED__

