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
    "src/**.h"
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