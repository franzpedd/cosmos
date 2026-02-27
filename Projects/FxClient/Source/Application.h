#pragma once

#include <Engine.h>
#include "CoreWidget.h"

namespace Cosmos
{
	class Application : public ApplicationBase
	{
	public:
		
		/// @brief constructor
		Application(const CreateInfo& ci);

		/// @brief destructor
		virtual ~Application();

	private:

		CoreWidget* mCoreWidget = nullptr;
	};
}