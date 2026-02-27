#pragma once

#include <Engine.h>
#include <vector>

#include "Effect.h"

namespace Cosmos
{
	class CoreWidget : public WidgetBase
	{
	public:

		/// @brief constructor
		CoreWidget(ApplicationBase* app);

		/// @brief destructor
		virtual ~CoreWidget();

	public:

		/// @brief updates logic for current frame
		virtual void Update() override;

		/// @brief updates drawables for current frame
		virtual void Render() override;

	private:

		/// @brief ui code for drawing of the top menu
		void DrawTopMenu();

		/// @brief ui code for drawing view canva
		void DrawViewMenu();

		/// @brief ui code for drawing create canva
		void DrawCreateMenu();

		/// @brief ui code for drawing settings canva
		void DrawSettingsMenu();

	private:

		struct MoveCommand
		{
			Effect* fx;
			bool addToEnabled;  // true: disabled->enabled, false: enabled->disabled
		};

		/// @brief ui code for drawing and handling the fx
		void DrawFXBox(const char* effectName, float boxWidth, float boxHeight, float leftButtonWidth, float titleYOffset, float innerXOffset, float innerYOffset, float smallButtonSize, std::function<void()> onLeftButtonClick, std::function<void()> onSettingsClick);

		/// @brief if an fx was moved to enabled/disabled must process the pending move command after the drawing of the current frame
		void ProcessPendingMoves();

	private:

		ApplicationBase* mApp = nullptr;

		Tremolo* mTremolo = nullptr;
		Overdrive* mOverdrive = nullptr;
		Distortion* mDistortion = nullptr;
		Delay* mDelay = nullptr;
		Reverb* mReverb = nullptr;
		Compression* mCompression = nullptr;

		std::vector<Effect*> mAllFX = {};
		std::vector<Effect*> mEnabledFX = {};
		std::vector<MoveCommand> mPendingMoves;
	};
}