#include "GUI.h"

#include "Core/Application.h"
#include "Core/Logger.h"
#include "Core/Renderer.h"
#include "Core/Window.h"
#include "UI/Icons.h"
#include "UI/Widget.h"

#include <EVK.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_sdl3.cpp>
#include <imgui/backends/imgui_impl_vulkan.cpp>
#include <font/lucide/lucide.c>
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

	GUI::GUI(ApplicationBase* app, Style style)
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
		
		// SDL and Vulkan
		ImGui::SetCurrentContext(g_Context);
		ImGui_ImplSDL3_InitForVulkan(mApp->GetWindow()->GetNativeWindow());

		evkUIRenderphase* renderphase = (evkUIRenderphase*)evk_get_renderphase(evk_Renderphase_Type_UI);
		ImGui_ImplVulkan_PipelineInfo pipeline = { 0 };
		pipeline.Subpass = 0;
		//pipeline.PipelineRenderingCreateInfo; // not used
		pipeline.RenderPass = renderphase->evkRenderpass.renderpass;
		pipeline.MSAASamples = (VkSampleCountFlagBits)renderphase->evkRenderpass.msaa;
		//pipeline.SwapChainImageUsage; // not used

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
		constexpr const ImWchar iconRanges[] = { ICON_MIN_LC, ICON_MAX_LC, 0 };
		float iconSize = 13.0f * GetFontScalar();
		float fontSize = 18.0f * GetFontScalar();

		ImFontConfig iconCFG;
		iconCFG.MergeMode = true;
		iconCFG.GlyphMinAdvanceX = iconSize;
		iconCFG.PixelSnapH = true;

		g_RobotoMono = io.Fonts->AddFontFromMemoryCompressedTTF(txt_robotomono_medium_compressed_data, txt_robotomono_medium_compressed_size, fontSize);
		g_IconLC = io.Fonts->AddFontFromMemoryCompressedTTF(icon_lucide_compressed_data, icon_lucide_compressed_size, iconSize, &iconCFG, iconRanges);
		io.Fonts->Build();

		SetStyle(style);

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

	float GUI::GetFontScalar()
	{
		#if defined(__ANDROID__)
		return 3.0f;
		#else
		return 1.0f;
		#endif
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

	ImVec4* GUI::GetStyleColors()
	{
		return ImGui::GetStyle().Colors;
	}

	void GUI::SetStyle(Style style)
	{
		switch (style)
		{
			case Style::Classic: { ImGui::StyleColorsClassic(); break; }
			case Style::GoldSrc: { SetGoldSrcTheme(); break; }
			case Style::Darkish: { SetDarkishTheme(); break; }
			default: { ImGui::StyleColorsClassic(); break; }
		}
	}

	void GUI::SetGoldSrcTheme()
	{
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.29f, 0.34f, 0.26f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.29f, 0.34f, 0.26f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.24f, 0.27f, 0.20f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.54f, 0.57f, 0.51f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.14f, 0.16f, 0.11f, 0.52f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.24f, 0.27f, 0.20f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.27f, 0.30f, 0.23f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.34f, 0.26f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.24f, 0.27f, 0.20f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.34f, 0.26f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.24f, 0.27f, 0.20f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.35f, 0.42f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.28f, 0.32f, 0.24f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.30f, 0.22f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.23f, 0.27f, 0.21f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.35f, 0.42f, 0.31f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.54f, 0.57f, 0.51f, 0.50f);
		colors[ImGuiCol_Button] = ImVec4(0.29f, 0.34f, 0.26f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.42f, 0.31f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.54f, 0.57f, 0.51f, 0.50f);
		colors[ImGuiCol_Header] = ImVec4(0.35f, 0.42f, 0.31f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.42f, 0.31f, 0.6f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.54f, 0.57f, 0.51f, 0.50f);
		colors[ImGuiCol_Separator] = ImVec4(0.14f, 0.16f, 0.11f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.54f, 0.57f, 0.51f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.19f, 0.23f, 0.18f, 0.00f); // grip invis
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.54f, 0.57f, 0.51f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.35f, 0.42f, 0.31f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.54f, 0.57f, 0.51f, 0.78f);
		colors[ImGuiCol_TabActive] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.24f, 0.27f, 0.20f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.35f, 0.42f, 0.31f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.78f, 0.28f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.73f, 0.67f, 0.24f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameBorderSize = 1.0f;
		style.WindowRounding = 0.0f;
		style.ChildRounding = 0.0f;
		style.FrameRounding = 0.0f;
		style.PopupRounding = 0.0f;
		style.ScrollbarRounding = 0.0f;
		style.GrabRounding = 0.0f;
		style.TabRounding = 0.0f;
	}

	void GUI::SetDarkishTheme()
	{
		ImGui::GetStyle().FrameRounding = 4.0f;
		ImGui::GetStyle().GrabRounding = 4.0f;

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	}
}
