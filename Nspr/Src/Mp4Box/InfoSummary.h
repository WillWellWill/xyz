#ifndef __NSPR_INFO_SUMMARY_H_INCLUDED__
#define __NSPR_INFO_SUMMARY_H_INCLUDED__

namespace nspr
{

class InfoSummary
{
public:
    InfoSummary();
    ~InfoSummary();

    int Summary(const u_char *info);
    int Print();

}; // class InfoSummary
} // namespace nspr

#endif // ifndef __NSPR_INFO_SUMMARY_H_INCLUDED__

