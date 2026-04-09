#include "Application.h"

#include "MainWidget.h"

namespace Cosmos
{
	Application::Application(const ApplicationBase::CreateInfo& ci)
		: ApplicationBase(ci)
	{
		mMainWidget = new MainWidget(this);
		GetGUI()->AddWidget(mMainWidget);
	}

	Application::~Application()
	{
		delete mMainWidget;
	}
}