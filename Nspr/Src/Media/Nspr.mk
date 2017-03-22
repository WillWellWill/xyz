
MODULE := Media

$(MODULE).SRC := \
			VideoCodec.cpp \
			VideoEncoder.cpp \
			VideoSource.cpp \
			MediaSponsor.cpp \
			RtpPacker.cpp \
			RtpParser.cpp \
			Mp4BoxPackerProc.cpp \
			StreamSender.cpp \
			MediaBridge.cpp \


$(MODULE).INC := .

include $(BUILD_DIR)/Base.mk


