#include "Nspr.h"

#include "BoxFactory.h"
#include "BoxCellDummy.h"
#include "BoxCellFtyp.h"
#include "BoxCellComposite.h"
#include "BoxCellMvhd.h"
#include "BoxCellTkhd.h"
#include "BoxCellMdhd.h"
#include "BoxCellHdlr.h"
#include "BoxCellVmhd.h"
#include "BoxCellDref.h"
#include "BoxCellStsd.h"
#include "BoxCellAvcC.h"
#include "BoxCellStts.h"
#include "BoxCellStsc.h"
#include "BoxCellStsz.h"
#include "BoxCellStco.h"
#include "BoxCellTrex.h"
#include "BoxCellHdlr.h"
#include "BoxCellIlst.h"
#include "BoxCellMeta.h"
#include "BoxCellMfhd.h"
#include "BoxCellTraf.h"
#include "BoxCellMdat.h"
#include "BoxCellTfra.h"
#include "BoxCellMfro.h"
#include "BoxCellTfhd.h"
#include "BoxCellTfdt.h"
#include "BoxCellTrun.h"

#include <string.h>

namespace nspr
{

BoxFactory::BoxFactory()
    : m_boxFactoryMap(BoxFactoryMap())
{
    m_boxFactoryMap[std::string("ftyp")] = new BoxCellFtyp();
    m_boxFactoryMap[std::string("moov")] = new BoxCellMoov();
    m_boxFactoryMap[std::string("mvhd")] = new BoxCellMvhd();
    m_boxFactoryMap[std::string("trak")] = new BoxCellTrak();
    m_boxFactoryMap[std::string("tkhd")] = new BoxCellTkhd();
    m_boxFactoryMap[std::string("mdia")] = new BoxCellMdia();
    m_boxFactoryMap[std::string("mdhd")] = new BoxCellMdhd();
    m_boxFactoryMap[std::string("hdlr")] = new BoxCellHdlr();
    m_boxFactoryMap[std::string("minf")] = new BoxCellMinf();
    m_boxFactoryMap[std::string("vmhd")] = new BoxCellVmhd();
    m_boxFactoryMap[std::string("dinf")] = new BoxCellDinf();
    m_boxFactoryMap[std::string("dref")] = new BoxCellDref();
    m_boxFactoryMap[std::string("stbl")] = new BoxCellStbl();
    m_boxFactoryMap[std::string("stsd")] = new BoxCellStsd();
    m_boxFactoryMap[std::string("stts")] = new BoxCellStts();
    m_boxFactoryMap[std::string("stsc")] = new BoxCellStsc();
    m_boxFactoryMap[std::string("stsz")] = new BoxCellStsz();
    m_boxFactoryMap[std::string("stco")] = new BoxCellStco();
    m_boxFactoryMap[std::string("mvex")] = new BoxCellMvex();
    m_boxFactoryMap[std::string("trex")] = new BoxCellTrex();
    m_boxFactoryMap[std::string("udta")] = new BoxCellUdta();
    m_boxFactoryMap[std::string("meta")] = new BoxCellMeta();
    m_boxFactoryMap[std::string("ilst")] = new BoxCellIlst();
    m_boxFactoryMap[std::string("moof")] = new BoxCellMoof();
    m_boxFactoryMap[std::string("mfhd")] = new BoxCellMfhd();
    m_boxFactoryMap[std::string("traf")] = new BoxCellTraf();
    m_boxFactoryMap[std::string("tfhd")] = new BoxCellTfhd();
    m_boxFactoryMap[std::string("tfdt")] = new BoxCellTfdt();
    m_boxFactoryMap[std::string("trun")] = new BoxCellTrun();
    m_boxFactoryMap[std::string("mdat")] = new BoxCellMdat();
    m_boxFactoryMap[std::string("mfra")] = new BoxCellMfra();
    m_boxFactoryMap[std::string("tfra")] = new BoxCellTfra();
    m_boxFactoryMap[std::string("mfro")] = new BoxCellMfro();
}

BoxFactory::~BoxFactory()
{
}

BoxCell *BoxFactory::GetBoxCell(const u_char *type)
{
    //NsprDebug("get box cell by type %s", type);
    BoxFactoryMapIt it = m_boxFactoryMap.find(std::string((const char *)type));
    if (it != m_boxFactoryMap.end()) {
        //NsprDebug("got box cell %p", it->second);
        return it->second;
    }
    return new BoxCellDummy();
}

} // namespace nspr

