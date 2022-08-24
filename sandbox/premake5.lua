project "sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/build/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/build/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/core/src",
	}

	links
	{
		"core"
	}

	filter "system:macosx"
		defines "LAI_PLATFORM_MACOSX"

	filter "configurations:Debug"
		defines "LAI_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "LAI_RELEASE"
		runtime "Release"
		optimize "on"
		symbols "off"