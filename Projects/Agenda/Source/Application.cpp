#include "Application.h"

#include "MainWidget.h"

namespace Cosmos::Agenda
{
	Application::Application(const Cosmos::ApplicationBase::CreateInfo& ci)
		: ApplicationBase(ci)
	{
		GetGUI()->SetStyle(Cosmos::GUI::Style::Dark);

		mMainWidget = new MainWidget(this);
		GetGUI()->AddWidget(mMainWidget);
	}

	Application::~Application()
	{
		delete mMainWidget;
	}
}