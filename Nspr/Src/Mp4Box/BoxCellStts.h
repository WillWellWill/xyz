#ifndef __NSPR_BOX_CELL_STTS_H_INCLUDED__
#define __NSPR_BOX_CELL_STTS_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellStts : public BoxCell
{
public:
	BoxCellStts();
	~BoxCellStts();

	virtual int Divide(u_char *data, size_t len);
    virtual int Fuse(u_char *data, size_t &len);
	virtual bool isComposite();
}; // class BoxCellStts

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_STTS_H_INCLUDED__

