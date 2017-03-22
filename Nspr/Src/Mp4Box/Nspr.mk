
MODULE := Mp4Box

$(MODULE).SRC := 	\
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


$(MODULE).INC := .

include $(BUILD_DIR)/Base.mk


