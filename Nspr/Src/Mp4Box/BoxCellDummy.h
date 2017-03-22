#ifndef __NSPR_BOX_CELL_DUMMY_H_INCLUDED__
#define __NSPR_BOX_CELL_DUMMY_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellDummy : public BoxCell
{
public:
	BoxCellDummy();
	~BoxCellDummy();

	virtual int Divide(u_char *data, size_t len);
	virtual bool isComposite();
}; // class BoxCellDummy

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_DUMMY_H_INCLUDED__

