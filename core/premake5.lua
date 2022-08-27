project "core"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/build/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/build/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"src/**.mm",
	}

	includedirs
	{
		"src",
		"%{IncludeDir.VulkanSDK}",
	}

	filter "system:macosx"
		defines "LAI_PLATFORM_MACOSX"
		libdirs
		{
			"%{LibraryDir.VulkanSDK}",
		}

		links
		{
			"Cocoa.framework",
			"Foundation.framework",
			"QuartzCore.framework",
			"%{Library.Vulkan_MacOSX}",
		}

	filter "configurations:Debug"
		defines "LAI_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "LAI_RELEASE"
		runtime "Release"
		optimize "on"
		symbols "off"