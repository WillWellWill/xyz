
MODULES_UT :=
UT_CPPFLAGS :=
UT_MK_FILES = $(foreach eachdir, $(ALL_DIRS), $(wildcard $(eachdir)/OceanusUT.mk))#./Stream/UT/OceanusUT.mk
include $(UT_MK_FILES)

UT_SRC_DIRS = $(foreach filepath, $(UT_MK_FILES), $(dir $(filepath)))
UT_SRC_FILES = $(foreach module, $(MODULES_UT), $(shell python ../Build/join_with_at.py $($(module).SRC)))
UT_INC_FILES = $(foreach module, $(MODULES_UT), $(shell python ../Build/join_with_at.py $($(module).INC)))
UT_SRC_PATHS = $(shell python ../Build/join_source_file.py $(UT_SRC_DIRS) $(UT_SRC_FILES))
UT_INC_PATHS = $(shell python ../Build/join_include_file.py $(UT_SRC_DIRS) $(UT_INC_FILES))


GTEST_GMOCK_DIR := ./GTest1.7.0
UT_SRC_PATHS += $(GTEST_GMOCK_DIR)/gmock-gtest-all.cpp \
			   $(GTEST_GMOCK_DIR)/gmock_main.cpp \
			   $(GTEST_GMOCK_DIR)/ut_adapt.cpp

OBJ_DIRS += $(foreach srcdir, $(UT_SRC_DIRS), $(OBJ_TOP_DIR)/$(srcdir))
OBJ_DIRS += $(OBJ_TOP_DIR)/$(GTEST_GMOCK_DIR)

ifneq "$(MAKECMDGOALS)" "clean"
UT_OBJS = $(patsubst %.cpp, $(OBJ_TOP_DIR)/%.o, $(UT_SRC_PATHS))
endif

#NGX_OBJS = $(shell find ../../Nginx/nginx-1.8.0/objs/ -name "*.o" | grep -v "src/core/nginx.o")
NGX_OBJS = ../../Nginx/nginx-1.8.0/objs/src/os/unix/ngx_shmem.o	\
			../../Nginx/nginx-1.8.0/objs/src/os/unix/ngx_socket.o	\
			../../Nginx/nginx-1.8.0/objs/src/core/ngx_string.o	\
			#../../Nginx/nginx-1.8.0/objs/src/core/ngx_connection.o	\
			#../../Nginx/nginx-1.8.0/objs/src/core/ngx_inet.o	
			#../../Nginx/nginx-1.8.0/objs/src/event/ngx_event.o	
			#../../Nginx/nginx-1.8.0/objs/src/core/ngx_cycle.o	\
			../../Nginx/nginx-1.8.0/objs/src/os/unix/ngx_process_cycle.o	\
.PHONY: test

CPPFLAGS += -I$(GTEST_GMOCK_DIR)

TEST = ut

test: preobj $(TARGET) $(TEST)
	@echo $(UT_MK_FILES)
	./ut

$(TEST): $(UT_OBJS) $(NGX_OBJS)
	$(CC) $(LDFLAGS) -o $@ -Wl,--start-group $(UT_OBJS) $(NGX_OBJS) $(LIBS) -Wl,--end-group -L$(SRC_TOP) -loceanus -lpthread -lcrypto -lssl -lpcre -lcrypt -lrt

	

