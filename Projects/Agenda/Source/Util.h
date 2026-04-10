#pragma once

#include <Engine.h>
#include <string>

namespace Cosmos::Agenda
{
	enum Month
	{
		January = 1,
		February,
		March,
		April,
		May,
		June,
		July,
		August,
		September,
		October,
		November,
		December
	};

	enum Day
	{
		Monday = 0,
		Tuesday,
		Wednesday,
		Thursday,
		Friday,
		Saturday,
		Sunday
	};

	/// @brief holds info about a date
	struct DateComponents
	{
		int year;
		int month;
		int day;
		std::string originalKey;
	};

	/// @brief holds info about a note
	struct Note
	{
		Day day;
		uint8_t dayNum;
		Month month;
		uint32_t year;
		std::string text;
		float4 color;

		/// @brief constructor
		Note(Day d, uint8_t dn, Month m, uint32_t y, std::string str, float4 c) : day(d), dayNum(dn), month(m), year(y), text(str), color(c) {}
	};

	/// @brief stringfies the month enum
	const char* month_c_str(Month month, bool ptbr = true);

	/// @brief convert month string back to Month enum
	Month string_to_month(const std::string& monthStr);

	/// @brief returns the integer from the month's name
	int month_name_to_number(const std::string& monthName);

	/// @brief stringfies the day enum
	const char* day_c_str(Day day, bool ptbr = true);

	/// @brief calculates what day of the week it is
	Day calculate_day_of_week(uint32_t year, Month month, uint8_t day);

	/// @brief concatenates stuff into one string
	std::string make_date_key(int year, Month month, int dayNum);
}