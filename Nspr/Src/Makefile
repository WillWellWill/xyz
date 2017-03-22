
SRC_TOP = $(shell pwd)

include $(SRC_TOP)/../Build/Define.mk
include $(BUILD_DIR)/NgxDefine.mk
include $(BUILD_DIR)/Objects.mk
include $(BUILD_DIR)/UnitTest.mk

STATIC_TARGET := libnspr.a

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(SOFLAGS) -o $@ $(OBJS) $(LIBS)
	ar rcs $(STATIC_TARGET) $(OBJS)


$(OBJ_TOP_DIR)/%.o: %.cpp
	$(CC) $(CPPFLAGS) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	$(CC) $(CPPFLAGS) -MD -o $@ -c $<

DEPS := $(OBJS:.o=.d)
-include $(DEPS)

clean:
	rm -rf $(OBJ_TOP_DIR) $(TARGET) $(TEST)

