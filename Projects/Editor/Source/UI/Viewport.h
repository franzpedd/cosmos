#pragma once

#include <Engine.h>

namespace Cosmos
{
	class Viewport : public WidgetBase
	{
	public:

		/// @brief constructor
		Viewport(ApplicationBase* app);

		/// @brief destructor
		virtual ~Viewport();

	public:

		/// @brief updates the logic
		virtual void Update() override;

		/// @brief updates the renders
		virtual void Render() override;

	public:

		/// @brief logic/widget for the top menu
		void DrawTopMenu(const ImVec2& pivotPos);

		/// @brief logic/widget for the bottom menu
		void DrawBottomMenu(const ImVec2& pivotPos);

	private:

		ApplicationBase* mApp = nullptr;

		struct Grid
		{
		public:

			/// @brief logic/widget for the snapping value
			static void DrawSnappingValue(float* snappingValue);

			/// @brief logic/widget for enable/disable snapping
			static void DrawEnableSnapping(bool* snapping, const ImVec4& activeColor);

			/// @brief logic/widget for grid visibility
			static void DrawVisibility(bool* visible, const ImVec4& activeColor);

		public:
			float snappingValue = 1.0f;
			bool snapping = true;
			bool visible = true;
			evkGrid* grid = NULL;
		} mGrid;
	};
}