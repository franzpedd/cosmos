#pragma once

#include <Engine.h>
#include "CoreWidget.h"

namespace Cosmos
{
	class Application : public ApplicationBase
	{
	public:

		/// @brief constructor
		explicit Application(const CreateInfo& ci);

		/// @brief destructor
		~Application() override;

	private:

		CoreWidget* mCoreWidget = nullptr;
	};
}