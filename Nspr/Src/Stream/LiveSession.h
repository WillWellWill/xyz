#ifndef __NSPR_LIVE_SESSION_H_INCLUDED__
#define __NSPR_LIVE_SESSION_H_INCLUDED__

#include "Metadata.h"

using namespace Interact;

namespace nspr
{

class MediaBridge;
class LiveSession
{
public:
	LiveSession(const u_char *connId, Metadata *metadata);
	~LiveSession();

	int CreateStream();

private:
	char m_connId[256];
	char m_ip[64];
	Metadata *m_metadata;
	MediaBridge *m_mediaBridge;

}; // class LiveSession

} // namespace nspr

#endif // ifndef __NSPR_LIVE_SESSION_H_INCLUDED__

