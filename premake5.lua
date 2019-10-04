local target_suffixes = {
	bsd       = "_linux",
	linux     = "_linux",
	solaris   = "_linux",
	windows   = "_win32",
	macosx    = "_osx"  ,
}

solution "chttp"
	language		"C++"
	architecture	"x86"
	location		"project"
	targetdir		"bin"

	--
	-- Statically link the C-Runtime to reduce dependencies needed to run our module
	--
	staticruntime "On"

	configurations { "Debug", "Release" }

	configuration "Debug"
		defines { "DEBUG_BUILD" }
		symbols		"On"		-- Generate debugging information
		optimize	"On"		-- Optimize the build output for size and speed

	configuration "Release"
		defines { "RELEASE_BUILD" }
		optimize	"On"		-- Optimize the build output for size and speed

	project "chttp"
		kind	"SharedLib"
		targetprefix "gmsv_"
		targetextension ".dll"
		targetsuffix ( target_suffixes[os.target()] )
		includedirs { "curl/include/", "gmod-module-base/include/" }
		files { "src/**.cpp", "src/**.h" }
		if os.target() == "windows" then
		    libdirs {"curl/lib"}
		    links {"libcurl"}
		else
		    links {"curl"}
		end
