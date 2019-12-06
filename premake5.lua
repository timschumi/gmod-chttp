local target_suffixes_86 = {
	bsd       = "_linux",
	linux     = "_linux",
	solaris   = "_linux",
	windows   = "_win32",
	macosx    = "_osx"  ,
}

local target_suffixes_64 = {
	bsd       = "_linux64",
	linux     = "_linux64",
	solaris   = "_linux64",
	windows   = "_win64",
	macosx    = "_osx64",
}

solution "chttp"
	language		"C++"
	location		"project"

	--
	-- Statically link the C-Runtime to reduce dependencies needed to run our module
	--
	staticruntime "On"

	configurations { "Debug", "Release" }
	platforms { "x86", "x64" }

	configuration "Debug"
		defines { "DEBUG_BUILD" }
		symbols		"On"		-- Generate debugging information
		optimize	"On"		-- Optimize the build output for size and speed

	configuration "Release"
		defines { "RELEASE_BUILD" }
		optimize	"On"		-- Optimize the build output for size and speed

	filter "platforms:x86"
		architecture "x86"
		targetsuffix ( target_suffixes_86[os.target()] )

	filter "platforms:x64"
		architecture "x86_64"
		targetsuffix ( target_suffixes_64[os.target()] )

	project "chttp"
		kind	"SharedLib"
		targetprefix "gmsv_"
		targetextension ".dll"
		includedirs { "curl/include/", "gmod-module-base/include/" }
		files { "src/*.cpp", "src/*.h", "src/"..os.target().."/*.cpp", "src/"..os.target().."/*.h" }
		if os.target() == "windows" then
		    defines { "WINDOWS_BUILD" }
		    libdirs {"curl/lib"}
			filter "platforms:x64"
				links {"libcurl-x64"}
			filter "platforms:x86"
				links {"libcurl"}
		else
		    links {"curl"}
		end
