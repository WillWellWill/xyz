#ifndef __NSPR_BOX_CELL_MFRO_H_INCLUDED__
#define __NSPR_BOX_CELL_MFRO_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellMfro : public BoxCell
{
public:
	BoxCellMfro();
	~BoxCellMfro();

	virtual int Divide(u_char *data, size_t len);
	virtual bool isComposite();
}; // class BoxCellMfro

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_MFRO_H_INCLUDED__

