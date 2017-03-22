#include "Nspr.h"

#include "StreamProc.h"

#include <algorithm>

namespace nspr
{

int StreamProc::AttachTxChannel(StreamProc *txChannel) 
{
    m_txChannels.push_back(txChannel); 
    return txChannel->AttachedTxBy(this); 
}

int StreamProc::DetachTxChannel(StreamProc *txChannel) 
{
    if (txChannel->DetachedTxBy(this)) {
        return NSPR_ERROR;
    }
    std::list<StreamProc *>::iterator it = find(m_txChannels.begin(), m_txChannels.end(), txChannel);
    if (it == m_txChannels.end()) {
        return NSPR_ERROR;
    }
    m_txChannels.erase(it);
    return NSPR_OK;
}

int StreamProc::AttachRxChannel(StreamProc *rxChannel) 
{
    m_rxChannel = rxChannel;
    return rxChannel->AttachedRxBy(this);
}

int StreamProc::DetachRxChannel(StreamProc *rxChannel) 
{
    m_rxChannel = nullptr;
    return rxChannel->DetachedRxBy(this);
}
    
int StreamProc::RelayTx(u_char *data, size_t len)
{
    std::list<StreamProc *>::iterator it;
    for (it = m_txChannels.begin(); it != m_txChannels.end(); ++it) {
        (*it)->OnHandleTx(data, len);
    }
    return NSPR_OK;
}

} // namespace nspr

