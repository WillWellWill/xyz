#ifndef __NSPR_BOX_CELL_STCO_H_INCLUDED__
#define __NSPR_BOX_CELL_STCO_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellStco : public BoxCell
{
public:
	BoxCellStco();
	~BoxCellStco();

	virtual int Divide(u_char *data, size_t len);
    virtual int Fuse(u_char *data, size_t &len);
	virtual bool isComposite();
}; // class BoxCellStco

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_STCO_H_INCLUDED__

