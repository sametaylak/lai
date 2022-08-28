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

	filter { "action:gmake2" }
  	buildoptions { "-Werror=vla" }

	filter "system:macosx"
		postbuildcommands {
			"%{wks.location}/bin/glslc/glslc_macosx %{wks.location}/assets/shaders/Builtin.ObjectShader.vert -o %{wks.location}/assets/shaders/Builtin.ObjectShader.vert.spv",
			"%{wks.location}/bin/glslc/glslc_macosx %{wks.location}/assets/shaders/Builtin.ObjectShader.frag -o %{wks.location}/assets/shaders/Builtin.ObjectShader.frag.spv",
		}

		defines
		{
			"LAI_PLATFORM_MACOSX",
			"VK_ENABLE_BETA_EXTENSIONS",
		}

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