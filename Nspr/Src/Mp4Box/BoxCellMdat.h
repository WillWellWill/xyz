#ifndef __NSPR_BOX_CELL_MDAT_H_INCLUDED__
#define __NSPR_BOX_CELL_MDAT_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellMdat : public BoxCell
{
public:
	BoxCellMdat();
	~BoxCellMdat();

	virtual int Divide(u_char *data, size_t len);
	virtual bool isComposite();
}; // class BoxCellMdat

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_MDAT_H_INCLUDED__

