solution "chttp"
	language		"C++"
	location		"project"

	staticruntime	"On"		-- Statically link the C-Runtime to reduce dependencies needed to run our module

	configurations { "Debug", "Release" }
	platforms { "linux64", "linux32", "win64", "win32" }

	filter "configurations:Debug"
		symbols		"On"		-- Generate debugging information
		optimize	"Off"

	filter "configurations:Release"
		symbols		"Off"
		optimize	"On"

	filter "platforms:win*"
		system "windows"
		defines { "WINDOWS_BUILD" }
		libdirs { "curl/lib" }

	filter "platforms:win32"
		architecture "x86"
		targetsuffix "_win32"
		links { "libcurl" }

	filter "platforms:win64"
		architecture "x86_64"
		targetsuffix "_win64"
		links { "libcurl-x64" }

	filter "platforms:linux*"
		system "linux"
		links { "curl" }

	filter "platforms:linux32"
		architecture "x86"
		targetsuffix "_linux"

	filter "platforms:linux64"
		architecture "x86_64"
		targetsuffix "_linux64"

	project "chttp"
		kind	"SharedLib"
		targetprefix "gmsv_"
		targetextension ".dll"
		includedirs { "curl/include/", "gmod-module-base/include/" }
		files { "src/*.cpp", "src/*.h", "src/"..os.target().."/*.cpp", "src/"..os.target().."/*.h" }
