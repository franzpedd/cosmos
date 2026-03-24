#pragma once

#include <Engine.h>

namespace Cosmos
{
	class EntityView : public WidgetBase
	{
	public:

		/// @brief constructor
		EntityView(ApplicationBase* app);

		/// @brief destructor
		virtual ~EntityView();

	public:

		/// @brief updates the logic
		virtual void Update() override;

		/// @brief updates the renders
		virtual void Render() override;

	public:

		/// @brief displays the entity propertie
		void ShowEntitySettingsPopup(Entity* entity);

	private:

		ApplicationBase* mApp = nullptr;
		bool mShowSettingsWindow = false;
		Entity* mEntitySettings = nullptr;
	};
}