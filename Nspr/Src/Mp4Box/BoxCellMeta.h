#ifndef __NSPR_BOX_CELL_META_H_INCLUDED__
#define __NSPR_BOX_CELL_META_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellMeta : public BoxCell
{
public:
	BoxCellMeta();
	~BoxCellMeta();

	virtual int Divide(u_char *data, size_t len);
    virtual int Fuse(u_char *data, size_t &len);
	virtual bool isComposite();
}; // class BoxCellMeta

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_META_H_INCLUDED__

