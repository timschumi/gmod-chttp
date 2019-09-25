solution "chttp"
	language		"C++"
	architecture	"x86"
	location		"project"
	targetdir		"bin"
	includedirs {"curl/include/"}

	--
	-- Statically link the C-Runtime to reduce dependencies needed to run our module
	--
	flags "StaticRuntime"

	configurations { "Release" }

	configuration "Release"
		flags		"symbols"	-- Generate debugging information
		optimize	"On"		-- Optimize the build output for size and speed

	project "chttp"
		kind	"SharedLib"
		include	"LuaInterface"

		files { "src/**.cpp", "src/**.h" }
		links {"curl"}
