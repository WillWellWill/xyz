#include "Nspr.h"

#include "RtpParser.h"

namespace nspr
{

RtpParser::RtpParser(StreamProc *proc)
	: m_proc(proc)
	, m_frame(new u_char[m_maxFrameLen])
	, m_frameLen(0)
	, m_meetIFrame(false)
	, m_lastSeq(-1)
{
	(void)m_frame;
	(void)m_frameLen;
	(void)m_proc;
}

RtpParser::~RtpParser()
{
	delete []m_frame;
}
	
int RtpParser::Parse(u_char *data, size_t len)
{
	if (ParseRtpHeader(data, len)) {
		return NSPR_ERROR;
	}
	u_char *pointer = data + m_fixedRtpHeaderLen;
	if ((*pointer & 0x1f) < 0x18) {
		m_frameLen = 0;
		const size_t naluLen = len - m_fixedRtpHeaderLen;
		H264Pack(pointer, naluLen);
		return RelayNextProc();
	}
	else if ((*pointer & 0x1f) == 0x1c) {
		const u_char fuIndicator = *pointer;
		const u_char fuHeader = *(pointer + 1);
		pointer += 2;
		if (fuHeader & 0x80) {
			m_frameLen = 0;
			const u_char naluByte = (fuIndicator & 0xe0) | (fuHeader & 0x1f);
			H264Pack((u_char *)&naluByte, 1);
		}
		const size_t naluLen = (data + len) - pointer;
		memcpy(m_frame + m_frameLen, pointer, naluLen);
		m_frameLen += naluLen;
		if (fuHeader & 0x40) {
			return RelayNextProc();
		}
		return NSPR_OK;
	}
	NsprError("unknow nalu packing type");
	return NSPR_ERROR;
}

int RtpParser::RelayNextProc()
{
	if (!m_meetIFrame && m_frame[4] == 0x67) {
		NsprInfo("get iframe");
		m_meetIFrame = true;
	}
	if (!m_meetIFrame) {
		NsprInfo("get no iframe, descard it");
		return NGX_OK;
	}
	return m_proc->RelayTx(m_frame, m_frameLen);
}

int RtpParser::H264Pack(u_char *data, size_t len)
{
	u_char prefix[4] = {0, 0, 0, 1};
	memcpy(m_frame, prefix, 4);
	memcpy(m_frame + 4, data, len);
	m_frameLen += len + 4;
	return NSPR_OK;
}

int RtpParser::ParseRtpHeader(u_char *data, size_t len)
{
	if (len < m_fixedRtpHeaderLen) {
		NsprError("len less than rtp header");
		return NSPR_ERROR;
	}
	uint16_t seq = ntohs(*(uint16_t *)(data + 2));
	if (m_lastSeq != -1 && seq != (uint16_t)(m_lastSeq + 1)) {
		NsprWarn("packet lost found, current sequence number: %d, last: %d"
				, seq, m_lastSeq);
	}
	m_lastSeq = seq;
	return NSPR_OK;
}

RtpParserProc::RtpParserProc()
	: m_rp(new RtpParser(this))
	, m_front(0)
	, m_back(0)
	, m_len(0)
{
	int i;
	for (i = 0; i < 10; i++) {
		m_pb[i].len = 0;
	}
}

RtpParserProc::~RtpParserProc()
{
}

int RtpParserProc::OnHandleTx(u_char *data, size_t len)
{
	return NSPR_OK;
}

int RtpParserProc::OnHandleRx(u_char *data, size_t len)
{
	if (!m_len) {
		memcpy(m_pb[(m_back++) % m_maxPBSize].data, data, len);
		++m_len;
		return NSPR_OK;
	}
	const uint16_t curseq = ntohs(*(uint16_t *)(data + 2));
	const uint16_t frontseq = ntohs(*(uint16_t *)(m_pb[m_front % m_maxPBSize].data + 2));
	uint64_t pointer = 0;
	if (frontseq < curseq) {
		pointer = curseq - frontseq + m_front;
	}
	else if (frontseq > curseq + m_maxSequenceNumPlusOne - m_maxPBSize - 5) {
		pointer = curseq + m_maxSequenceNumPlusOne - frontseq + m_front;
	}
	else {
		NsprError("missed packet seq: %d", curseq);
		return NSPR_OK;
	}
	const int windex = pointer % m_maxPBSize;
	memcpy(m_pb[windex].data, data, len);
	m_pb[windex].len = len;
	if (pointer > m_back) {
		m_back = pointer;
	}
	++m_len;
	
	if (m_len > m_maxPBSize / 2) {
		const int rindex = m_front % m_maxPBSize;
		m_rp->Parse(m_pb[rindex].data, m_pb[rindex].len);
		++m_front;
		--m_len;
	}
	return NSPR_OK;
}

} // namespace nspr

