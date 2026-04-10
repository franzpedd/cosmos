#pragma once

#include <Engine.h>

/// @brief forward declarations
namespace Cosmos::Agenda { class MainWidget; }

namespace Cosmos::Agenda
{
	class Application : public Cosmos::ApplicationBase
	{
	public:

		/// @brief constructor
		Application(const Cosmos::ApplicationBase::CreateInfo& ci);

		/// @brief destructor
		virtual ~Application();

	public:
		
	private:
		MainWidget* mMainWidget = nullptr;
	};
}