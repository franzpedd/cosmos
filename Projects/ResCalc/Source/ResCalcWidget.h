#pragma once

#include <Engine.h>
#include <string>

namespace Cosmos
{
    class ResistorCalcWidget : public WidgetBase
    {
    public:

        typedef enum BandColor
        {
            Black = 0, Brown, Red, Orange, Yellow, Green, Blue, Violet, Grey, White, Gold, Silver
        } BandColor;

        typedef enum Scale
        {
            MilliOhm = 0, Ohm = 1, KiloOhm = 1000, MegaOhm = 100000, GigaOhm = 1000000,
        } Scale;

        typedef int BandFlags;
        enum BandFlags_
        {
            BandFlags_None = 0,
            BandFlags_Band1 = 1 << 0,
            BandFlags_Band2 = 1 << 1,
            BandFlags_Band3 = 1 << 2,
            BandFlags_Multiplier = 1 << 3,
            BandFlags_Tolerance = 1 << 4,
            BandFlags_Temperature = 1 << 5
        };

    public:

        /// @brief constructor
        ResistorCalcWidget(ApplicationBase* app);

        /// @brief destructor (fuck off clang, override destructor, really?)
        ~ResistorCalcWidget() override = default;

    public:

        /// @brief user interface updating
        void Update() override;

    private:

        /// @brief draws the bands according with flags
        void ModeBand(BandFlags flags);

        /// @brief draws a band color given the options
        void DrawBand(BandColor* option, bool drawBlack = true, bool drawWhite = true, bool extraOptions = false);

    public:

        /// @brief calculates the resistor resistance based on witch bandMode is currently active
        void CalculateResistance(int bandMode);

        /// @brief returns the formated re-scaled resistance
        static void Resistance_Format(double ohms, char *output);

        /// @brief returns the multiplier value based on band color
        static double GetMultiplierValue(BandColor color);

        /// @brief returns the string for the tolerance
        static const char* GetToleranceValue(BandColor color);

        /// @brief returns the string for the temperature
        static const char* GetTemperatureCoefficient(BandColor color);

        /// @brief utility to convert prue RGBA into it's floating version
        static float4 ConvertRGB_Vec4(int r, int g, int b);

    private:

        ApplicationBase* mApp = nullptr;
        float4 mBlack, mBrown, mRed, mOrange, mYellow, mGreen, mBlue, mViolet, mGrey, mWhite, mGold, mSilver;
        BandColor mBand1, mBand2, mBand3, mMultiplier, mTolerance, mTemperature;

        struct ResistorInfo
        {
            float resistance;
            Scale scale;
            std::ostringstream msg;
        } mResistorInfo;
    };
}