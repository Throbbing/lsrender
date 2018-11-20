#include<config\common.h>

bool ls::lsEmbree::isInit = false;

void ls::lsEmbree::initEmbree()
{
	if (isInit)
		return;

	hw.rtcDevice = rtcNewDevice(nullptr);
	hw.rtcScene = rtcNewScene(hw.rtcDevice);
}


void ls::lsEmbree::releaseEmbree()
{
	if (hw.rtcScene)
		rtcReleaseScene(hw.rtcScene);

	if (hw.rtcDevice)
		rtcReleaseDevice(hw.rtcDevice);
}