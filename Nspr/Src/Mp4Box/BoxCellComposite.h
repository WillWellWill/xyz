#ifndef __NSPR_BOX_CELL_COMPOSITE_H_INCLUDED_H__
#define __NSPR_BOX_CELL_COMPOSITE_H_INCLUDED_H__

#include "BoxFactory.h"

namespace nspr
{
class BoxCellComposite : public BoxCell
{
public:
    BoxCellComposite();
    virtual ~BoxCellComposite();

    virtual int Divide(u_char *data, size_t len);
    virtual bool isComposite();

}; // class BoxCellComposite

class BoxCellMoov : public BoxCellComposite
{
public:
    BoxCellMoov() {}
    virtual ~BoxCellMoov() {}

}; // class BoxCellMoov

class BoxCellTrak : public BoxCellComposite
{
public:
    BoxCellTrak() {}
    virtual ~BoxCellTrak() {}

}; // class BoxCellTrak

class BoxCellMdia : public BoxCellComposite
{
public:
    BoxCellMdia() {}
    virtual ~BoxCellMdia() {}

}; // class BoxCellMdia

class BoxCellMinf : public BoxCellComposite
{
public:
    BoxCellMinf() {}
    virtual ~BoxCellMinf() {}

}; // class BoxCellMinf

class BoxCellDinf : public BoxCellComposite
{
public:
    BoxCellDinf() {}
    virtual ~BoxCellDinf() {}

}; // class BoxCellDinf

class BoxCellStbl : public BoxCellComposite
{
public:
    BoxCellStbl() {}
    virtual ~BoxCellStbl() {}

}; // class BoxCellStbl

/*
class BoxCellStsd : public BoxCellComposite
{
public:
    BoxCellStsd() {}
    virtual ~BoxCellStsd() {}

}; // class BoxCellStsd

class BoxCellAvc1 : public BoxCellComposite
{
public:
    BoxCellAvc1() {}
    virtual ~BoxCellAvc1() {}

}; // class BoxCellAvc1
*/

class BoxCellMvex : public BoxCellComposite
{
public:
    BoxCellMvex() {}
    virtual ~BoxCellMvex() {}

}; // class BoxCellAvc1

class BoxCellUdta : public BoxCellComposite
{
public:
    BoxCellUdta() {}
    virtual ~BoxCellUdta() {}

}; // class BoxCellUdta

/*
class BoxCellMeta : public BoxCellComposite
{
public:
    BoxCellMeta() {}
    virtual ~BoxCellMeta() {}

}; // class BoxCellMeta
*/

class BoxCellMoof : public BoxCellComposite
{
public:
    BoxCellMoof() {}
    virtual ~BoxCellMoof() {}

}; // class BoxCellMeta

/*
class BoxCellTraf : public BoxCellComposite
{
public:
    BoxCellTraf() {}
    virtual ~BoxCellTraf() {}

}; // class BoxCellTraf
*/

class BoxCellMfra : public BoxCellComposite
{
public:
    BoxCellMfra() {}
    virtual ~BoxCellMfra() {}

}; // class BoxCellMfra

} // namespace BoxCell

#endif // ifndef __NSPR_BOX_CELL_COMPOSITE_H_INCLUDED_H__

