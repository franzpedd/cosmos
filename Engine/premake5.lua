project "Engine"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"

    targetdir(dir)
    objdir(obj)

    files
    {
        "Source/**.h",
        "Source/**.cpp"
    }

    includedirs
    {
        "%{includelist.Engine}"
    }

    links
    {

    }

    defines
    {
        "_EXPORT_SHARED_LIB"
    }

    filter "configurations:Debug"
        defines { "ENGINE_DEBUG" }
        runtime "Debug"
        symbols "On"
    
    filter "configurations:Release"
        defines { "ENGINE_RELEASE" }
        runtime "Release"
        optimize "Full"

    filter "system:windows"
        defines
        {
            "_CRT_SECURE_NO_WARNINGS"
        }

        disablewarnings 
        { 
            "4251"
        }