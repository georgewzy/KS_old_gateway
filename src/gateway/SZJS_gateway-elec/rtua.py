
def GetCPPPATH(BSP_ROOT, RTT_ROOT):
	CPPPATH=[
		BSP_ROOT + "/.",
		BSP_ROOT + "/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F2xx",
		BSP_ROOT + "/Libraries/STM32F2xx_StdPeriph_Driver/inc",
		BSP_ROOT + "/applications",
		BSP_ROOT + "/config",
		BSP_ROOT + "/drivers",
		BSP_ROOT + "/misc",
		BSP_ROOT + "/services",
		RTT_ROOT + "/components/dfs/filesystems/elmfat",
		RTT_ROOT + "/components/dfs/filesystems/romfs",
		RTT_ROOT + "/components/dfs/include",
		RTT_ROOT + "/components/drivers/include",
		RTT_ROOT + "/components/drivers/spi",
		RTT_ROOT + "/components/finsh",
		RTT_ROOT + "/components/libdl",
		RTT_ROOT + "/components/utilities/logtrace",
		RTT_ROOT + "/components/utilities/ymodem",
		RTT_ROOT + "/include",
		RTT_ROOT + "/libcpu/arm/common",
		RTT_ROOT + "/libcpu/arm/cortex-m3",
	]

	return CPPPATH

def GetCPPDEFINES():
	CPPDEFINES=['USE_STDPERIPH_DRIVER']
	return CPPDEFINES

