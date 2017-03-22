
MODULE := Net

$(MODULE).SRC := \
				NsprAppCore.cpp \
				MsgConnection.cpp \
				StreamConnection.cpp \
				ApiDispatcher.cpp \
				ProtocolSponsor.cpp \
				ProtocolConnection.cpp \
				WSConnection.cpp \


$(MODULE).INC := .

include $(BUILD_DIR)/Base.mk


