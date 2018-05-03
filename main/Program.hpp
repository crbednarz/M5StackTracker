#pragma once


#include "RemoteFrame.hpp"


class Program
{
public:
	Program() = delete;

	static void launch();

private:
	static RemoteFrame _remoteFrame;
	
	static void renderThreadEntryPoint();	
};
