#include "GUI.h"

#include "Core/Application.h"
#include "Core/Logger.h"
#include "Core/Renderer.h"
#include "Core/Window.h"
#include "UI/Icons.h"
#include "UI/Widget.h"

#include <evk.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_sdl3.cpp>
#include <imgui/backends/imgui_impl_vulkan.cpp>
#include <font/lucide/lucide.c>
#include <font/awesome/awesome.c>
#include <font/roboto/robotomono.c>

IMGUI_API ImGuiContext* GImGui = nullptr; // this is because of imgui
static ImGuiContext* g_Context = nullptr;
static ImFont* g_IconFA = nullptr;
static ImFont* g_IconLC = nullptr;
static ImFont* g_RobotoMono = nullptr;

namespace Cosmos
{
	static void internal_vkerror_callback(VkResult err)
	{
		if (err != VK_SUCCESS) {
			LOG_TO_TERMINAL(Logger::Error, "ImGui internal vulkan error: Result %d", err);
		}
	}

	GUI::GUI(ApplicationBase* app)
		: mApp(app)
	{
		// initial config
		IMGUI_CHECKVERSION();
		g_Context = ImGui::CreateContext();

		// io config
		ImGuiIO& io = ImGui::GetIO();
		(void)io;

		io.IniFilename = "UI.ini";
		io.WantCaptureMouse = true;

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		#if defined(__ANDROID__)
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
		#endif

		if (io.BackendFlags | ImGuiBackendFlags_PlatformHasViewports && io.BackendFlags | ImGuiBackendFlags_RendererHasViewports)
		{
			#if defined(_WIN32)
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			#endif
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		}

		ImGui::StyleColorsClassic();
		
		// SDL and Vulkan
		ImGui::SetCurrentContext(g_Context);
		ImGui_ImplSDL3_InitForVulkan(mApp->GetWindow()->GetNativeWindow());

		evkUIRenderphase* renderphase = (evkUIRenderphase*)evk_get_renderphase(evk_Renderphase_Type_UI);
		ImGui_ImplVulkan_PipelineInfo pipeline = { 0 };
		pipeline.Subpass = 0;
		pipeline.PipelineRenderingCreateInfo; // not used
		pipeline.RenderPass = renderphase->evkRenderpass.renderpass;
		pipeline.MSAASamples = VK_SAMPLE_COUNT_1_BIT; // ui layer has no msaa
		pipeline.SwapChainImageUsage; // not used

		ImGui_ImplVulkan_InitInfo info = { 0 };
		info.ApiVersion = mApp->GetRenderer()->GetAPIVersion();
		info.Instance = evk_get_instance();
		info.PhysicalDevice = evk_get_physical_device();
		info.Device = evk_get_device();
		info.QueueFamily = evk_get_graphics_queue_family();
		info.Queue = evk_get_graphics_queue();
		info.DescriptorPool = VK_NULL_HANDLE;
		info.DescriptorPoolSize = 128; // arbitrary value, must be > 0
		info.MinImageCount = evk_get_swapchain_image_count();
		info.ImageCount = evk_get_swapchain_image_count();
		info.PipelineCache = VK_NULL_HANDLE;
		info.PipelineInfoMain = pipeline;
		info.PipelineInfoForViewports = pipeline;
		info.UseDynamicRendering = false;
		info.Allocator = VK_NULL_HANDLE;
		info.CheckVkResultFn = internal_vkerror_callback;
		info.MinAllocationSize = 1024 * 1024;
		//info.CustomShaderVertCreateInfo = { }; // optional customize fragment shader
		//info.CustomShaderFragCreateInfo = { }; // optional customize vertex shader
		ImGui_ImplVulkan_Init(&info);

		// fonts
		constexpr const ImWchar iconRanges1[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		constexpr const ImWchar iconRanges2[] = { ICON_MIN_LC, ICON_MAX_LC, 0 };
		float iconSize = 13.0f;
		float fontSize = 18.0f;

		#if defined(__ANDROID__)
		LOG_TO_TERMINAL(Logger::Todo, "Figure out a better way for icon size");
		iconSize *= 3.0f;
		fontSize *= 3.0f;
		#endif

		ImFontConfig iconCFG;
		iconCFG.MergeMode = true;
		iconCFG.GlyphMinAdvanceX = iconSize;
		iconCFG.PixelSnapH = true;

		g_RobotoMono = io.Fonts->AddFontFromMemoryCompressedTTF(txt_robotomono_medium_compressed_data, txt_robotomono_medium_compressed_size, fontSize);
		g_IconFA = io.Fonts->AddFontFromMemoryCompressedTTF(icon_awesome_compressed_data, icon_awesome_compressed_size, iconSize, &iconCFG, iconRanges1);
		g_IconLC = io.Fonts->AddFontFromMemoryCompressedTTF(icon_lucide_compressed_data, icon_lucide_compressed_size, iconSize, &iconCFG, iconRanges2);
		io.Fonts->Build();

		LOG_TO_TERMINAL(Logger::Todo, "Implement Gizmos");
	}

	GUI::~GUI()
	{
		evk_device_wait_idle();
		mWidgets.Clear();
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext(g_Context);
	}

	void* GUI::GetImGuiContext()
	{
		return g_Context;
	}

	void GUI::Update()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		mWidgets.ForEach([](WidgetBase* widget)
			{
				if (widget->GetVisibility()) widget->Update();
			});

		ImGui::Render();

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void GUI::Render()
	{
		mWidgets.ForEach([](WidgetBase* widget)
			{
				if (widget->GetVisibility()) widget->Render();
			});
	}

	void GUI::DrawRawData(void* commandbuffer)
	{
		ImDrawData* data = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(data, (VkCommandBuffer)commandbuffer);
	}

	bool GUI::AddWidget(WidgetBase* widget)
	{
		if (!widget) return false;

		if (!FindWidget(widget->GetName())) {
			mWidgets.PushToTop(widget);
			return true;
		}

		return false;
	}

	bool GUI::RemoveWidget(const char* name)
	{
		auto result = mWidgets.FindIf([&name](WidgetBase* widget)
			{
				return widget && strcmp(widget->GetName(), name) == 0;
			});

		if (result.has_value()) {
			mWidgets.Remove(result.value());
			return true;
		}

		return false;
	}

	WidgetBase* GUI::FindWidget(const char* name)
	{
		auto result = mWidgets.FindIf([&name](WidgetBase* widget)
			{
				return widget && strcmp(widget->GetName(), name) == 0;
			});
		return result ? *result : nullptr;
	}

	void GUI::ToggleCursor()
	{
		mCursorVisible = !mCursorVisible;

		ImGuiIO& io = ImGui::GetIO();
		if (mCursorVisible) {
			io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
			io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
			return;
		}

		io.ConfigFlags ^= ImGuiConfigFlags_NoMouse;
	}

	void GUI::OnMinimize()
	{
		mWidgets.ForEach([](WidgetBase* widget)
			{
				if (widget->GetVisibility()) widget->OnMinimize();
			});
	}

	void GUI::OnMaximize()
	{
		mWidgets.ForEach([](WidgetBase* widget)
			{
				if (widget->GetVisibility()) widget->OnMaximize();
			});
	}

	void GUI::OnRestore()
	{
		mWidgets.ForEach([](WidgetBase* widget)
			{
				if (widget->GetVisibility()) widget->OnRestore();
			});
	}

	void GUI::OnResize(int32_t width, int32_t height)
	{
		mWidgets.ForEach([width, height](WidgetBase* widget)
			{
				if (widget->GetVisibility()) widget->OnResize(width, height);
			});
	}

	void GUI::OnKeyPress(Input::Keycode keycode, Input::Keymod mod, bool held)
	{
		mWidgets.ForEach([keycode, mod, held](WidgetBase* widget)
			{
				if (widget->GetVisibility()) widget->OnKeyPress(keycode, mod, held);
			});
	}

	void GUI::OnKeyRelease(Input::Keycode keycode)
	{
		mWidgets.ForEach([keycode](WidgetBase* widget)
			{
				if (widget->GetVisibility()) widget->OnKeyRelease(keycode);
			});
	}

	void GUI::OnButtonPress(Input::Buttoncode buttoncode, Input::Keymod mod)
	{
		mWidgets.ForEach([buttoncode, mod](WidgetBase* widget)
			{
				if (widget->GetVisibility()) widget->OnButtonPress(buttoncode, mod);
			});
	}

	void GUI::OnButtonRelease(Input::Buttoncode buttoncode)
	{
		mWidgets.ForEach([buttoncode](WidgetBase* widget)
			{
				if (widget->GetVisibility()) widget->OnButtonRelease(buttoncode);
			});
	}

	void GUI::OnMouseScroll(double xoffset, double yoffset)
	{
		mWidgets.ForEach([xoffset, yoffset](WidgetBase* widget)
			{
				if (widget->GetVisibility()) widget->OnMouseScroll(xoffset, yoffset);
			});
	}

	void GUI::OnMouseMove(double xoffset, double yoffset)
	{
		mWidgets.ForEach([xoffset, yoffset](WidgetBase* widget)
			{
				if (widget->GetVisibility()) widget->OnMouseMove(xoffset, yoffset);
			});
	}

	void GUI::OnDPIChange(float scale)
	{
		mWidgets.ForEach([scale](WidgetBase* widget)
			{
				if (widget->GetVisibility()) widget->OnDPIChange(scale);
			});
	}
}
