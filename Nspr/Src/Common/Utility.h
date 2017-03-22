#ifndef __NSPR_UTILITY_H_INCLUDED__
#define __NSPR_UTILITY_H_INCLUDED__


namespace nspr
{

class Utility
{
public:
	Utility();
	~Utility();

	void IpFromConnId(u_char *connId, u_char *ip, size_t ipLen);

}; // class Utility

} // namespace nspr

#endif // ifndef __NSPR_UTILITY_H_INCLUDED__

