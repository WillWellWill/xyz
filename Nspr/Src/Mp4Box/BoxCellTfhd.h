#ifndef __NSPR_BOX_CELL_TFHD_H_INCLUDED__
#define __NSPR_BOX_CELL_TFHD_H_INCLUDED__

#include "BoxFactory.h"

namespace nspr
{

class BoxCellTfhd : public BoxCell
{
public:
	BoxCellTfhd();
	~BoxCellTfhd();

	virtual int Divide(u_char *data, size_t len);
    virtual int Fuse(u_char *data, size_t &len);
	virtual bool isComposite();

    void SetMoofOffset(int moofOffset) { m_moofOffset = moofOffset; }
    void SetDefaultSampleSize(int defaultSampleSize) { m_defaultSampleSize = defaultSampleSize; }

private:
    int m_moofOffset;
    int m_defaultSampleSize;

}; // class BoxCellTfhd

} // namespace nspr

#endif // ifndef __NSPR_BOX_CELL_TFHD_H_INCLUDED__

