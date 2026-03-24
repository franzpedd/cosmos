package com.ResCalc;
import android.content.res.AssetManager;
import android.os.Bundle;

public class Main extends org.libsdl.app.SDLActivity
{
    /// @brief register libraries needed for the project
    @Override
    protected String[] getLibraries()
    {
        return new String[]
        {
                "SDL3",
                "vulkan",
                "Engine",
                "ResCalc"
        };
    }

    /// @brief if need for external assets we must initialize the assets manager
    public static native void initAssetsManager(AssetManager assetManager);

    /// @brief almost like the int main of the project
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        initAssetsManager(getAssets());
    }
}