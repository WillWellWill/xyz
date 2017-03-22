#ifndef __NSPR_BOX_CELL_DREF_H_INCLUDED__
#define __NSPR_BOX_CELL_DREF_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellDref : public BoxCell
{
public:
	BoxCellDref();
	~BoxCellDref();

	virtual int Divide(u_char *data, size_t len);
    virtual int Fuse(u_char *data, size_t &len);
	virtual bool isComposite();
}; // class BoxCellDref

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_DREF_H_INCLUDED__

