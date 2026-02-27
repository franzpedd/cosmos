#pragma once

#include <string>

namespace Cosmos
{
	class Effect
	{
	public:

		/// @brief constructor
		Effect(const char* name, bool enabled = false) : mName(name), mEnabled(enabled) {}

		/// @brief destructor
		~Effect() = default;

		/// @brief returns a reference to the effect's name
		inline const std::string& GetName() { return mName; }

		/// @brief returns if the effect is currently enabled
		inline const bool IsEnabled() const { return mEnabled; }

		/// @brief enables the effect (will be on fx list)
		inline void Enable() { mEnabled = true; }

		/// @brief disabled the effect (won't be on fx list)
		inline void Disable() { mEnabled = false; }

	public:

		/// @brief called when settings button is clicked
		virtual void OnSettings() = 0;

		/// @brief called when left button is clicked
		virtual void OnLeftButton() = 0;

	protected:

		std::string mName;
		bool mEnabled = false;
	};

	class Tremolo : public Effect
	{
	public:

		/// @brief constructor
		Tremolo(const char* name) : Effect(name) {}

		/// @brief destructor
		virtual ~Tremolo() = default;

	public:

		/// @brief called when settings button is clicked
		inline virtual void OnSettings() override
		{
		}

		/// @brief called when left button is clicked
		inline virtual void OnLeftButton() override
		{
			mEnabled = !mEnabled;
		}
	};

	class Overdrive : public Effect
	{
	public:

		/// @brief constructor
		Overdrive(const char* name) : Effect(name) {}

		/// @brief destructor
		virtual ~Overdrive() = default;

	public:

		/// @brief called when settings button is clicked
		inline virtual void OnSettings() override
		{
		}

		/// @brief called when left button is clicked
		inline virtual void OnLeftButton() override
		{
			mEnabled = !mEnabled;
		}
	};

	class Distortion : public Effect
	{
	public:

		/// @brief constructor
		Distortion(const char* name) : Effect(name) {}

		/// @brief destructor
		virtual ~Distortion() = default;

	public:

		/// @brief called when settings button is clicked
		inline virtual void OnSettings() override
		{
		}

		/// @brief called when left button is clicked
		inline virtual void OnLeftButton() override
		{
			mEnabled = !mEnabled;
		}
	};

	class Delay : public Effect
	{
	public:

		/// @brief constructor
		Delay(const char* name) : Effect(name) {}

		/// @brief destructor
		virtual ~Delay() = default;

	public:

		/// @brief called when settings button is clicked
		inline virtual void OnSettings() override
		{
		}

		/// @brief called when left button is clicked
		inline virtual void OnLeftButton() override
		{
			mEnabled = !mEnabled;
		}
	};

	class Reverb : public Effect
	{
	public:

		/// @brief constructor
		Reverb(const char* name) : Effect(name) {}

		/// @brief destructor
		virtual ~Reverb() = default;

	public:

		/// @brief called when settings button is clicked
		inline virtual void OnSettings() override
		{
		}

		/// @brief called when left button is clicked
		inline virtual void OnLeftButton() override
		{
			mEnabled = !mEnabled;
		}
	};

	class Compression : public Effect
	{
	public:

		/// @brief constructor
		Compression(const char* name) : Effect(name) {}

		/// @brief destructor
		virtual ~Compression() = default;

	public:

		/// @brief called when settings button is clicked
		inline virtual void OnSettings() override
		{
		}

		/// @brief called when left button is clicked
		inline virtual void OnLeftButton() override
		{
			mEnabled = !mEnabled;
		}
	};
}