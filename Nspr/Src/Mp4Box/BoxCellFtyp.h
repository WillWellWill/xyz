#ifndef __NSPR_BOX_CELL_FTYP_H_INCLUDED_H__
#define __NSPR_BOX_CELL_FTYP_H_INCLUDED_H__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellFtyp : public BoxCell
{
public:
    BoxCellFtyp();
    virtual ~BoxCellFtyp();

    virtual int Divide(u_char *data, size_t len);
    virtual int Fuse(u_char *data, size_t &len);
    virtual bool isComposite();

}; // class BoxCellFtyp

} // namespace BoxCell

#endif // ifndef __NSPR_BOX_FACTORY_H_INCLUDED_H__

