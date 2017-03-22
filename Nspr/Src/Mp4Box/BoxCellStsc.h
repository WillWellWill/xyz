#ifndef __NSPR_BOX_CELL_STSC_H_INCLUDED__
#define __NSPR_BOX_CELL_STSC_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellStsc : public BoxCell
{
public:
	BoxCellStsc();
	~BoxCellStsc();

	virtual int Divide(u_char *data, size_t len);
    virtual int Fuse(u_char *data, size_t &len);
	virtual bool isComposite();
}; // class BoxCellStsc

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_STSC_H_INCLUDED__

