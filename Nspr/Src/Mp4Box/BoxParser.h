#ifndef __NSRP_BOX_PARSER_H_INCLUDED_H__
#define __NSRP_BOX_PARSER_H_INCLUDED_H__

namespace nspr
{

class InfoSummary;
class BoxFactory;
class BoxParser
{
public:
    BoxParser(const u_char *file);
    virtual ~BoxParser();

    int ParseBox();
    int CheckBox(u_char *type, size_t boxLen, bool &isComposite);

private:
    const static int m_maxFilePathLen = 1024;
    u_char m_filePath[m_maxFilePathLen];
    FILE *m_fp;
    InfoSummary *m_infoSummary;
    BoxFactory *m_boxFactory;

private:
    int ReadBox();
    
}; // class BoxParser

} // namespace nspr

#endif // ifndef __NSRP_BOX_PARSER_H_INCLUDED_H__

