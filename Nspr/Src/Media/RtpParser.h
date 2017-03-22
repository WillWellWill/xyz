#ifndef __NSPR_RTP_PARSER_H_INCLUDED__
#define __NSPR_RTP_PARSER_H_INCLUDED__

#include "StreamProc.h"

namespace nspr
{

class RtpParser
{
public:
	RtpParser(StreamProc *proc);
	~RtpParser();

	int Parse(u_char *data, size_t len);

private:
	StreamProc *m_proc;
	u_char *m_frame;
	const static int m_maxFrameLen = 1024 * 1024;
	const static int m_fixedRtpHeaderLen = 12;
	int m_frameLen;

	bool m_meetIFrame;
	int m_lastSeq;

private:
	int ParseRtpHeader(u_char *data, size_t len);
	int H264Pack(u_char *data, size_t len);
	int RelayNextProc();

}; // class RtpParser

class RtpParserProc : public StreamProc
{
public:
	RtpParserProc();
	virtual ~RtpParserProc();

    virtual int OnHandleTx(u_char *data, size_t len);
    virtual int OnHandleRx(u_char *data, size_t len);

private:
    RtpParser *m_rp;
	const static int m_maxPacketLen = 1600;
	const static int m_maxPBSize = 20;
	const static int m_maxSequenceNumPlusOne = 65536;
	struct {
		u_char data[1600];
		int len;
	} m_pb[m_maxPBSize];

	uint64_t m_front;
	uint64_t m_back;
	int m_len;

}; // class RtpParserProc

} // namespace nspr

#endif // ifndef __NSPR_RTP_PARSER_H_INCLUDED__

