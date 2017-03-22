#ifndef __NSPR_BOX_CELL_TFRA_H_INCLUDED__
#define __NSPR_BOX_CELL_TFRA_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellTfra : public BoxCell
{
public:
	BoxCellTfra();
	~BoxCellTfra();

	virtual int Divide(u_char *data, size_t len);
	virtual bool isComposite();
}; // class BoxCellTfra

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_TFRA_H_INCLUDED__

