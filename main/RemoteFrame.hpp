#pragma once


#include "InterlacedFrame.hpp"
#include <atomic>


class RemoteFrame
{
public:
	RemoteFrame();

	void swap();

	InterlacedFrame& workingFrame();

	void upload();
	
private:
	InterlacedFrame _evenFrame;
	InterlacedFrame _oddFrame;

	InterlacedFrame* _uploadingFrame;
	InterlacedFrame* _workingFrame;
	std::atomic<bool> _hasDirtyFrame;

};

