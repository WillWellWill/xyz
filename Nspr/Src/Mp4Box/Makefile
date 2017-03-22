
SRC_TOP = $(shell pwd)

CC := g++

CPPFLAGS := -Wall -Werror -std=c++11 -g -O3 -fno-common -DNGINX_OFF
CPPFLAGS += -I /opt/Code/NSPR/Nspr/Src/Common

LDFLAGS += -Wall -g


SRC_PATHS = 	\
				BoxDo.cpp \
				BoxPacker.cpp \
				InfoSummary.cpp \
				BoxFactory.cpp \
				BoxCellDummy.cpp \
				BoxCellFtyp.cpp \
				BoxCellComposite.cpp \
				BoxCellMvhd.cpp \
				BoxCellTkhd.cpp \
				BoxCellMdhd.cpp \
				BoxCellHdlr.cpp \
				BoxCellVmhd.cpp \
				BoxCellDref.cpp \
				BoxCellStsd.cpp \
				BoxCellAvcC.cpp \
				BoxCellStts.cpp \
				BoxCellStsc.cpp \
				BoxCellStsz.cpp \
				BoxCellStco.cpp \
				BoxCellTrex.cpp \
				BoxCellMeta.cpp \
				BoxCellIlst.cpp \
				BoxCellMfhd.cpp \
				BoxCellTraf.cpp \
				BoxCellMdat.cpp \
				BoxCellTfra.cpp \
				BoxCellMfro.cpp \
				BoxCellTfhd.cpp \
				BoxCellTfdt.cpp \
				BoxCellTrun.cpp \



SRC_DIRS = $(SRC_TOP)

OBJ_TOP_DIR := $(SRC_TOP)/obj
OBJ_DIRS := $(foreach srcdir, $(SRC_DIRS), $(OBJ_TOP_DIR)/$(srcdir))

CPPFLAGS += -I $(OBJ_TOP_DIR)

.PHONY: clean obj_prepare

ifneq "$(MAKECMDGOALS)" "clean"
OBJS = $(patsubst %.cpp, $(OBJ_TOP_DIR)/%.o, $(SRC_PATHS))
endif

TARGET := mp4box

all: obj_prepare $(TARGET)

obj_prepare:
	mkdir -p obj

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(SOFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJ_TOP_DIR)/%.o: %.cpp
	$(CC) $(CPPFLAGS) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	$(CC) $(CPPFLAGS) -MD -o $@ -c $<

DEPS := $(OBJS:.o=.d)
-include $(DEPS)

clean:
	rm -rf $(OBJ_TOP_DIR) $(TARGET) $(TEST)

