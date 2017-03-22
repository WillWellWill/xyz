#ifndef __NSPR_MEDIA_BRIDGE_H_INCLUDED__
#define __NSPR_MEDIA_BRIDGE_H_INCLUDED__

#include "Metadata.h"
using namespace Interact;

namespace nspr
{

class MediaBridge
{
public:
	MediaBridge(const char *ip);
	~MediaBridge();

	int BuildBridge(Metadata *metadata);

private:
	int BuildPublishBridge(Metadata *metadata);
	int BuildPlayBridge(Metadata *metadata);

private:
	char m_ip[256];

}; // class MediaBridge

} // namespace nspr

#endif // ifndef __NSPR_MEDIA_BRIDGE_H_INCLUDED__

