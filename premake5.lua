workspace "Cosmos"
    architecture "x86_64"
    configurations { "Debug", "Release" }
    startproject "Editor"

    dir = "%{wks.location}/_Build/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    obj = "%{wks.location}/_Build/temp/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    includelist = {}
    includelist["Engine"] = "%{wks.location}/Engine/Source"
    includelist["Editor"] = "%{wks.location}/Editor/Source"

group "Thirdparty"

group ""

include "Engine"
include "Editor"