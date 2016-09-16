function setTargetObjDir(outDir)
	targetdir(outDir)
	objdir(string.lower("../intermediate/%{cfg.shortname}/" .. _ACTION))
	targetsuffix(string.lower("_%{cfg.shortname}_" .. _ACTION))
end

solution "benchmark"
	configurations { "release" }
	platforms { "x32", "x64" }

	location ("./" .. (_ACTION or ""))
	language "C++"
	flags { "ExtraWarnings" }
	defines { "__STDC_FORMAT_MACROS=1" }

    if (os.findlib("folly")) then
        defines { "HAS_FOLLY=1" }
        links { "folly" }
    end
	
	configuration "release"
		defines { "NDEBUG" }
		flags { "Optimize" }

	configuration "vs*"
		defines { "_CRT_SECURE_NO_WARNINGS" }
		
	configuration "gmake"
		buildoptions "-msse4.2 -Wall -Wextra -std=c++11"

	project "itoa"
		kind "ConsoleApp"
		
		files { 
			"../src/**.h",
			"../src/**.cpp",
		}

		setTargetObjDir("../bin")
