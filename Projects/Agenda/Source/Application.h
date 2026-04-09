#pragma once

#include <Engine.h>

/// @brief forward declarations
namespace Cosmos { class MainWidget; }

namespace Cosmos
{
	class Application : public ApplicationBase
	{
	public:

		/// @brief constructor
		Application(const ApplicationBase::CreateInfo& ci);

		/// @brief destructor
		virtual ~Application();

	public:
		
	private:
		MainWidget* mMainWidget = nullptr;
	};
}