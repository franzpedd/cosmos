#include "Util.h"

#include <string>
#include <sstream>
#include <iomanip>

namespace Cosmos::Agenda
{
    static const char* const g_month_names_en[] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
    static const char* const g_month_names_ptbr[] = { "Janeiro", "Fevereiro", "Março", "Abril", "Maio", "Junho", "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro" };
    //
    static const char* const day_names_en[] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
    static const char* const day_names_ptbr[] = { "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado", "Domingo" };

    const char* month_c_str(Month month, bool ptbr)
    {
        int idx = (int)(month) - 1;
        if (idx < 0 || idx >= 12) return "Undefined";
        return ptbr ? g_month_names_ptbr[idx] : g_month_names_en[idx];
    }

    Month string_to_month(const std::string& monthStr)
    {
        for (int i = 0; i < 12; i++) {
            if (monthStr == g_month_names_en[i]) {
                return static_cast<Month>(i + 1);
            }
        }
        return January;
    }

    int month_name_to_number(const std::string& monthName)
    {
        static const std::unordered_map<std::string, int> monthMap = {
            {"January", 1}, {"February", 2}, {"March", 3}, {"April", 4},
            {"May", 5}, {"June", 6}, {"July", 7}, {"August", 8},
            {"September", 9}, {"October", 10}, {"November", 11}, {"December", 12}
        };

        auto it = monthMap.find(monthName);
        return (it != monthMap.end()) ? it->second : 0;
    }

    const char* day_c_str(Day day, bool ptbr)
    {
        int idx = (int)(day);
        if (idx < 0 || idx >= 7) return "Undefined";
        return ptbr ? day_names_ptbr[idx] : day_names_en[idx];
    }

    Day calculate_day_of_week(uint32_t year, Month month, uint8_t day)
    {
        if (month == January || month == February) {
            month = static_cast<Month>(static_cast<int>(month) + 12);
            year--;
        }

        int m = static_cast<int>(month);
        int y = year % 100;
        int c = year / 100;

        int h = (day + (13 * (m + 1)) / 5 + y + y / 4 + c / 4 + 5 * c) % 7;

        static const Day zellerToDay[] = { Saturday, Sunday, Monday, Tuesday, Wednesday, Thursday, Friday };
        return zellerToDay[h];
    }

    std::string make_date_key(int year, Month month, int dayNum)
    {
        char year_str[5];
        char day_str[3];
        snprintf(year_str, sizeof(year_str), "%d", year);
        snprintf(day_str, sizeof(day_str), "%d", dayNum);

        std::ostringstream oss;
        oss << year_str << "::" << month_c_str(month, false) << "::" << day_str;
        return oss.str();
    }
}