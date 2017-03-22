#ifndef __NSPR_BOX_CELL_AVC_C_H_INCLUDED__
#define __NSPR_BOX_CELL_AVC_C_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellAvcC : public BoxCell
{
public:
	BoxCellAvcC();
	~BoxCellAvcC();

	virtual int Divide(u_char *data, size_t len);
	virtual bool isComposite();
}; // class BoxCellAvcC

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_AVC_C_H_INCLUDED__

