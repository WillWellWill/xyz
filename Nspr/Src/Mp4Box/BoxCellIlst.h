#ifndef __NSPR_BOX_CELL_ILST_H_INCLUDED__
#define __NSPR_BOX_CELL_ILST_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellIlst : public BoxCell
{
public:
	BoxCellIlst();
	~BoxCellIlst();

	virtual int Divide(u_char *data, size_t len);
	virtual bool isComposite();
}; // class BoxCellIlst

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_ILST_H_INCLUDED__

