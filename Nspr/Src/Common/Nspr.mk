
MODULE := Common

$(MODULE).SRC := \
				NsprLog.cpp \
				ConnMgmt.cpp \
				WordDb.cpp \
				Utility.cpp \
				PortMgmt.cpp \




$(MODULE).INC := .

include $(BUILD_DIR)/Base.mk


