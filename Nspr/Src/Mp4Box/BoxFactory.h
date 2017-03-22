#ifndef __NSPR_BOX_FACTORY_H_INCLUDED_H__
#define __NSPR_BOX_FACTORY_H_INCLUDED_H__

#include <map>
#include <string>

#ifdef NsprDebug
#undef NsprDebug
#define NsprDebug(format, args...) NsprLog(NSPR_LOG_DEBUG1, format, ##args)
#endif

namespace nspr
{

class BoxCell
{
public:
    BoxCell() {}
    virtual ~BoxCell() {}

    virtual int Divide(u_char *data, size_t len) = 0;
    virtual int Fuse(u_char *data, size_t &len) { return NSPR_OK; }
    virtual bool isComposite() = 0;

}; // class BoxCell

typedef std::map<std::string, BoxCell *> BoxFactoryMap;
typedef std::map<std::string, BoxCell *>::iterator BoxFactoryMapIt;
class BoxFactory
{
public:
    BoxFactory();
    ~BoxFactory();

    BoxCell *GetBoxCell(const u_char *type);

private:
    BoxFactoryMap m_boxFactoryMap;

}; // class BoxFactory
} // namespace nspr

#endif // ifndef __NSPR_BOX_FACTORY_H_INCLUDED_H__

