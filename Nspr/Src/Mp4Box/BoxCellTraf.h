#ifndef __NSPR_BOX_CELL_TRAF_H_INCLUDED__
#define __NSPR_BOX_CELL_TRAF_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellTraf : public BoxCell
{
public:
	BoxCellTraf();
	~BoxCellTraf();

	virtual int Divide(u_char *data, size_t len);
	virtual bool isComposite();
}; // class BoxCellTraf

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_TRAF_H_INCLUDED__

