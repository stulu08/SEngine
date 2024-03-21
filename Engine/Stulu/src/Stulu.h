#pragma once

////////////////////Core/////////////////////
#include "Stulu/Core/Application.h"
#include "Stulu/Core/Core.h"
#include "Stulu/Core/CoreConfig.h"
#include "Stulu/Core/Input.h"
#include "Stulu/Core/Log.h"
#include "Stulu/Core/Platform.h"
#include "Stulu/Core/Window.h"
#include "Stulu/Core/Time.h"
#include "Stulu/Core/Layer.h"
#include "Stulu/Core/LayerStack.h"
#include "Stulu/Core/Resources.h"
#include "Stulu/Core/Timestep.h"
#include "Stulu/Core/Utils.h"
#include "Stulu/Core/Version.h"

////////////////////Debug////////////////////
#include "Stulu/Debug/RenderDataProfiler.h"
#include "Stulu/Debug/Instrumentor.h"

////////////////////Events///////////////////
#include "Stulu/Events/KeyEvent.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Events/MouseEvent.h"
#include "Stulu/Events/Event.h"

////////////////////ImGui////////////////////
#include "Stulu/ImGui/Gizmo.h"
#include "Stulu/ImGui/ImGuiLayer.h"

////////////////////Math/////////////////////
#include "Stulu/Math/Math.h"
#include "Stulu/Math/Random.h"

//////////////////Rendering//////////////////
#include "Stulu/Renderer/FrameBuffer.h"
#include "Stulu/Renderer/Renderer.h"
#include "Stulu/Renderer/Renderer2D.h"
#include "Stulu/Renderer/SubTexture2D.h"
#include "Stulu/Renderer/OrthographicCamera.h"
#include "Stulu/Renderer/PerspectiveCamera.h"
#include "Stulu/Renderer/Buffer.h"
#include "Stulu/Renderer/Camera.h"
#include "Stulu/Renderer/GraphicsContext.h"
#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Renderer/OrthographicCamera.h"
#include "Stulu/Renderer/PerspectiveCamera.h"
#include "Stulu/Renderer/RenderAPI.h"
#include "Stulu/Renderer/RenderCommand.h"
#include "Stulu/Renderer/Shader.h"
#include "Stulu/Renderer/Texture.h"
#include "Stulu/Renderer/VertexArray.h"

////////////////////Scene////////////////////
#include "Stulu/Scene/AssetsManager.h"
#include "Stulu/Scene/Behavior.h"
#include "Stulu/Scene/Components/Components.h"
#include "Stulu/Scene/Material.h"
#include "Stulu/Scene/SceneCamera.h"
#include "Stulu/Scene/SceneRenderer.h"
#include "Stulu/Scene/SceneSerializer.h"
#include "Stulu/Scene/Model.h"
#include "Stulu/Scene/GameObject.h"
#include "Stulu/Scene/Components/Components.h"
#include "Stulu/Scene/Scene.h"

//Scripting
#include "Stulu/Scripting/EventCaller.h"
