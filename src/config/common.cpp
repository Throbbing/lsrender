#include<config\common.h>

bool ls::lsEmbree::isInit = false;
ls::lsEmbree::_hw ls::lsEmbree::hw;
void ls::lsEmbree::initEmbree()
{
	if (isInit)
		return;

	hw.rtcDevice = rtcNewDevice(nullptr);
	hw.rtcScene = rtcNewScene(hw.rtcDevice);

	isInit = true;
}


void ls::lsEmbree::releaseEmbree()
{
	if (hw.rtcScene)
		rtcReleaseScene(hw.rtcScene);

	if (hw.rtcDevice)
		rtcReleaseDevice(hw.rtcDevice);
}