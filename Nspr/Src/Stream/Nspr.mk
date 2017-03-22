
MODULE := Stream

$(MODULE).SRC := StreamProcFactory.cpp \
				 InteractStreamProc.cpp \
				 LogStreamProc.cpp \
				 StreamProc.cpp \
				 LiveSession.cpp \


$(MODULE).INC := .

include $(BUILD_DIR)/Base.mk


