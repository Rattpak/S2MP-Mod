-- thank you iw7-mod / auroramod / momo5502
dependencies = {
	basePath = "./deps"
}

function dependencies.load()
	dir = path.join(dependencies.basePath, "premake/*.lua")
	deps = os.matchfiles(dir)

	for i, dep in pairs(deps) do
		dep = dep:gsub(".lua", "")
		require(dep)
	end
end

function dependencies.imports()
	for i, proj in pairs(dependencies) do
		if type(i) == 'number' then
			proj.import()
		end
	end
end

function dependencies.projects()
	for i, proj in pairs(dependencies) do
		if type(i) == 'number' then
			proj.project()
		end
	end
end

dependencies.load()

workspace "s2mp-mod"
architecture "x86_64"
configurations { "Debug", "Release" }
startproject "s2mp-mod"

-- === Main Mod Project ===
project "s2mp-mod"
kind "SharedLib"

runtime "Release"

location "./build"
objdir "%{wks.location}/obj"
targetdir "%{wks.location}/bin/%{cfg.platform}/%{cfg.buildcfg}"

language "C++"
cppdialect "C++20"

configurations {"Debug", "Release"}

files {
	"src/**.cpp",
    "src/**.h",
    "src/**.hpp",
    "src/**.rc",
    "src/**.bmp"
}

vpaths {
    ["src/Arxan"] = {
        "src/Arxan.hpp",
        "src/ArxanPatches.cpp",
        "src/DebugPatches.cpp",
    },
    ["src/Code"] = {
        "src/Noclip.cpp",
        "src/Noclip.hpp",
        "src/PrintPatches.cpp",
        "src/PrintPatches.hpp",
    },
    ["src/Common"] = {
        "src/FuncPointers.cpp",
        "src/FuncPointers.h",
        "src/memory.cpp",
        "src/memory.h",
        "src/structs.h",
    },
    ["src/Console"] = {
        "src/Console.cpp",
        "src/Console.hpp",
        "src/CustomCommands.cpp",
        "src/ExtConsole.cpp",
        "src/ExtConsoleGui.cpp",
        "src/InternalConsole.cpp",
    },
    ["src/Loaders"] = {
        "src/FontLoader.cpp",
        "src/Loaders.cpp",
        "src/Loaders.hpp",
        "src/LuiLoader.cpp",
    },
    ["src/Util"] = {
        "src/ConfigManager.cpp",
        "src/ConfigManager.h",
        "src/DvarInterface.cpp",
        "src/DvarInterface.hpp",
        "src/GameUtil.cpp",
        "src/GameUtil.hpp",
    },
    ["src/Util/Hook"] = {
        "src/Hook.cpp",
        "src/Hook.hpp",
        "src/Hooking.Patterns.cpp",
        "src/Hooking.Patterns.h",
    },
    ["src/dev"] = {
        "src/DevDef.h",
        "src/DevDraw.cpp",
        "src/DevPatches.cpp",
        "src/DevPatches.hpp",
    },
    ["src/dll"] = {
        "src/dllmain.cpp",
        "src/framework.h",
        "src/pch.cpp",
        "src/pch.h",
        "src/resource.h",
        "src/s2mp-mod.rc",
        "src/s2mp_con.bmp",
    },
}

includedirs {
    "src"
}

libdirs {
    "bin/%{cfg.buildcfg}"
}	

dependencies.imports()

filter "system:windows"
    systemversion "latest"

filter "configurations:Debug"
    runtime "Debug"
    symbols "On"

filter "configurations:Release"
    runtime "Release"
    optimize "On"

group "Dependencies"
dependencies.projects()