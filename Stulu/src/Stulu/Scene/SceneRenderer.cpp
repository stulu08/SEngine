#include "st_pch.h"
#include "SceneRenderer.h"

#include "Stulu/Core/Resources.h"
#include <imgui.h>
#include <glm/gtx/euler_angles.hpp>

#include "Stulu/Core/Time.h"

namespace Stulu {
	SceneRenderer::RuntimeData SceneRenderer::s_data;

	void SceneRenderer::init() {
		ST_PROFILING_FUNCTION();
		if (!s_data.quadPostProcShader) {
			s_data.quadPostProcShader = Shader::create("quadFullScreenPP", R"(
		#version 460
		layout (location = 0) in vec3 a_pos;
		layout (location = 1) in vec2 a_texCoords;
		out vec2 v_tex;
		layout(std140, binding = 1) uniform model
		{
			float z;
		};
		void main()
		{
			v_tex=a_texCoords;
			vec3 pos = a_pos;
			pos.z = z;
			gl_Position=vec4(pos, 1.0);
		}
		)", R"(
		#version 460
		in vec2 v_tex;
		##include "Stulu/Functions"
		##include "Stulu/Bindings"
		layout (binding = 0) uniform sampler2D texSampler;
		layout (binding = 1) uniform sampler2D bloom;
		out vec4 a_color;

		void apply_bloom(inout vec4 original)
		{
			vec3 bc = texture(bloom, v_tex).rgb;
			//original = vec4(mix(original.rgb, bc, bloomStrength), original.a);
			original = vec4((original.rgb + (bc * bloomStrength)), original.a);
		}
		void main()
		{
			vec4 color = texture2D(texSampler, v_tex);
			
			apply_bloom(color);
			
			a_color = gammaCorrect(color, gamma, toneMappingExposure);

			if(color.a == 0.0f)
				discard;
		}
		)");
		}
		if (!s_data.shadowMap) {
			FrameBufferSpecs specs;
			specs.width = s_data.shadowSize;
			specs.height = s_data.shadowSize;
			specs.samples = 1;

			specs.colorTexture.format = TextureFormat::None;
			specs.colorTexture.filtering = TextureFiltering::Linear;
			specs.colorTexture.wrap = TextureWrap::ClampToBorder;
			specs.colorTexture.forceGenMips = true;

			specs.depthTexture.format = TextureFormat::Depth32F;
			specs.depthTexture.filtering = TextureFiltering::Linear;
			specs.depthTexture.wrap = TextureWrap::ClampToBorder;
			specs.depthTexture.forceGenMips = false;

			s_data.shadowMap = FrameBuffer::create(specs);
		}
		if (!s_data.shadowShader) {
			s_data.shadowShader = Shader::create("shadowShader", R"(
		#version 460 core
		##include "Stulu/MeshLayout"
		##include "Stulu/Bindings"
		
		layout(std140, binding = 1) uniform modelData
		{
			mat4 normalMatrix;
			mat4 transform;
		};		
		void main(){
			gl_Position = lightSpaceMatrix * transform * vec4(a_pos, 1.0);
		}
		)", R"(
		#version 460
		layout(location = 0) out float fragmentdepth;
		#define BIAS 0.01
		void main(){
			gl_FragDepth = gl_FragCoord.z;
			gl_FragDepth += gl_FrontFacing ? BIAS : 0.0; //  float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

			fragmentdepth = gl_FragCoord.z;
		}
		)");
		}

	}

	void SceneRenderer::Begin() {
		ST_PROFILING_FUNCTION();
		for (int i = 0; i < 32; i++) {
			Resources::getWhiteTexture()->bind(i);
		}
	}
	void SceneRenderer::End() {
		ST_PROFILING_FUNCTION();
		s_data.drawList.clear();
		s_data.transparentDrawList.clear();
	}

	void SceneRenderer::LoadLights(Scene* scene) {
		ST_PROFILING_FUNCTION();
		for (auto& goID : scene->getAllGameObjectsWith<LightComponent, TransformComponent>()) {
			GameObject go(goID, scene);
			if (go == scene->getMainLight()) {
				s_data.shadowCaster = s_data.lightBufferData.lightCount;
			}
			AddLight(go.getComponent<TransformComponent>(), go.getComponent<LightComponent>());
		}
		UploadLightBuffer();
		s_data.lightBufferData = LightBufferData();
		s_data.shadowCaster = 0;
	}
	void SceneRenderer::AddLight(const TransformComponent& transform, const LightComponent& light) {
		if (s_data.lightBufferData.lightCount < ST_MAXLIGHTS) {
			
			s_data.lightBufferData.lights[s_data.lightBufferData.lightCount].colorAndStrength = glm::vec4(light.color, light.strength);
			s_data.lightBufferData.lights[s_data.lightBufferData.lightCount].positionAndType = glm::vec4(transform.worldPosition, light.lightType);
			s_data.lightBufferData.lights[s_data.lightBufferData.lightCount].rotation = glm::vec4(transform.forward, 1.0f);
			s_data.lightBufferData.lights[s_data.lightBufferData.lightCount].spotLightData = glm::vec4(glm::cos(glm::radians(light.spotLight_cutOff)), glm::cos(glm::radians(light.spotLight_outerCutOff)), 1.0f, light.areaRadius);
			s_data.lightBufferData.lightCount++;
		}
	}
	void SceneRenderer::UploadLightBuffer() {
		Renderer::getBuffer(BufferBinding::Light)->setData(&s_data.lightBufferData, sizeof(LightBufferData));
	}
	const glm::quat getViewRotation(uint32_t face) {
		switch (face) {
		case 0://+x
			return glm::quat(glm::radians(glm::vec3(180.0f, 90.0f, .0f)));
			break;
		case 1://-x
			return glm::quat(glm::radians(glm::vec3(180.0f, -90.0f, .0f)));
			break;
		case 2://+y
			return glm::quat(glm::radians(glm::vec3(90.0f, .0f, .0f)));
			break;
		case 3://-y
			return glm::quat(glm::radians(glm::vec3(-90.0f, .0f, .0f)));
			break;
		case 4://+z
			return glm::quat(glm::radians(glm::vec3(180.0f, .0f, .0f)));
			break;
		case 5://-y
			return glm::quat(glm::radians(glm::vec3(180.0f, 180.0f, .0f)));
			break;
		}
		return glm::quat();
	}
	glm::mat4 calcView(uint32_t face, TransformComponent& transform) {
		glm::mat4 local = glm::mat4(1.0f);
		switch (face) {
		case 0://+x
			local = Math::createMat4(transform.worldPosition, getViewRotation(face), glm::vec3(1.0f));
			break;
		case 1://-x
			local = Math::createMat4(transform.worldPosition, getViewRotation(face), glm::vec3(1.0f));
			break;
		case 2://+y
			local = Math::createMat4(transform.worldPosition, getViewRotation(face), glm::vec3(1.0f));
			break;
		case 3://-y
			local = Math::createMat4(transform.worldPosition, getViewRotation(face), glm::vec3(1.0f));
			break;
		case 4://+z
			local = Math::createMat4(transform.worldPosition, getViewRotation(face), glm::vec3(1.0f));
			break;
		case 5://-y
			local = Math::createMat4(transform.worldPosition, getViewRotation(face), glm::vec3(1.0f));
			break;
		}
		return glm::inverse(local);
	}

	void SceneRenderer::DrawSceneToCamera(SceneCamera& sceneCam, CameraComponent& mainCam) {
		CameraComponent camComp = mainCam.gameObject.getComponent<CameraComponent>();
		camComp.cam = sceneCam.getCamera();
		camComp.reflectionFrameBuffer = sceneCam.reflectionFrameBuffer;
		camComp.reflectionMap = sceneCam.reflectionMap;
		camComp.settings.zFar = sceneCam.m_zFar;
		camComp.settings.zNear = sceneCam.m_zNear;
		camComp.settings.fov = sceneCam.m_fov;
		camComp.settings.aspectRatio = sceneCam.m_aspectRatio;
		camComp.frustum = sceneCam.getFrustum();
		camComp.postProcessing = &sceneCam.getPostProcessingData();
		DrawSceneToCamera(sceneCam.m_transform, camComp);
	}
	void SceneRenderer::DrawSceneToCamera(SceneCamera& sceneCam, Scene* scene) {
		CameraComponent camComp = CameraComponent(CameraMode::Perspective, false);
		camComp.gameObject = GameObject(entt::null, scene);
		camComp.cam = sceneCam.getCamera();
		camComp.reflectionFrameBuffer = sceneCam.reflectionFrameBuffer;
		camComp.reflectionMap = sceneCam.reflectionMap;
		camComp.settings.zFar = sceneCam.m_zFar;
		camComp.settings.zNear = sceneCam.m_zNear;
		camComp.settings.fov = sceneCam.m_fov;
		camComp.settings.aspectRatio = sceneCam.m_aspectRatio;
		camComp.frustum = sceneCam.getFrustum();
		camComp.postProcessing = &sceneCam.getPostProcessingData();
		DrawSceneToCamera(sceneCam.m_transform, camComp);
	}
	void SceneRenderer::DrawSceneToCamera(TransformComponent& transform, CameraComponent& cameraComp) {
		ST_PROFILING_FUNCTION();
		const Ref<Camera>& cam = cameraComp.getNativeCamera();
		const glm::mat4& view = glm::inverse(transform.transform);
		const glm::mat4& proj = cameraComp.getProjection();

		const Ref<FrameBuffer>& reflectionFrameBuffer = cameraComp.reflectionFrameBuffer;
		Ref<CubeMap> reflectionMap = cameraComp.reflectionMap;
		bool useReflectionMap = cameraComp.gameObject.getScene()->m_data.graphicsData.useReflectionMapReflections;

		Ref<SkyBox> camSkyBoxTexture = nullptr;
		uint32_t camSkyBoxMapType = 0;
		glm::mat4 skyboxRotation = glm::mat4(1.0f);
		if ((entt::entity)cameraComp.gameObject != entt::null) {
			if (cameraComp.settings.clearType == CameraComponent::ClearType::Skybox && cameraComp.gameObject.hasComponent<SkyBoxComponent>()) {
				auto& s = cameraComp.gameObject.getComponent<SkyBoxComponent>();
				camSkyBoxTexture = s.texture;
				camSkyBoxMapType = (uint32_t)s.mapType;
				skyboxRotation = Math::createMat4(glm::vec3(.0f), glm::quat(glm::radians(s.rotation)), glm::vec3(1.0f));
			}
			if (cameraComp.gameObject.hasComponent<PostProcessingComponent>()) {
				cameraComp.postProcessing = &cameraComp.gameObject.getComponent<PostProcessingComponent>().data;
			}
		}
		
		//upload shader scene data
		s_data.sceneBufferData.env_lod = cameraComp.gameObject.getScene()->m_data.graphicsData.env_lod;
		s_data.sceneBufferData.skyboxMapType = camSkyBoxMapType;
		s_data.sceneBufferData.useSkybox = camSkyBoxTexture != nullptr;
		s_data.sceneBufferData.shaderFlags = cameraComp.gameObject.getScene()->m_data.shaderFlags;
		s_data.sceneBufferData.skyBoxRotation = skyboxRotation;
		s_data.sceneBufferData.lightSpaceMatrix = glm::mat4(1.0f);
		s_data.sceneBufferData.shadowCasterPos = glm::vec3(0.0f);
		s_data.sceneBufferData.shadowCaster = -1;
		Renderer::getBuffer(BufferBinding::Scene)->setData(&s_data.sceneBufferData, sizeof(SceneBufferData));

		//transparent sorting
		std::sort(s_data.transparentDrawList.begin(), s_data.transparentDrawList.end(), [&](auto& a, auto& b) {
			return glm::distance(transform.worldPosition, glm::vec3(a.transform[3])) > glm::distance(transform.worldPosition, glm::vec3(b.transform[3]));//sort the vector backwards
			});
		
		//shadow pass
		GameObject lightObject = cameraComp.gameObject.getScene()->getMainLight();
		if(lightObject && s_data.shadowCaster > -1) {
			TransformComponent lTC = lightObject.getComponent<TransformComponent>();
			LightComponent& light = lightObject.getComponent<LightComponent>();

			float halfDistance = s_data.shadowDistance / 2.0f;
			glm::vec3 lightPos = transform.position + (halfDistance * (-lTC.forward));
			glm::mat4 lightView = glm::inverse(Math::createMat4(lightPos, lTC.worldRotation, glm::vec3(1.0f)));
			glm::mat4 lightProj = glm::ortho(-halfDistance, halfDistance, halfDistance, -halfDistance, 0.01f, s_data.shadowDistance * 4.0f);

			s_data.sceneBufferData.shadowCaster = s_data.shadowCaster;
			s_data.sceneBufferData.lightSpaceMatrix = lightProj * lightView;
			s_data.sceneBufferData.shadowCasterPos = lightPos;
			Renderer::getBuffer(BufferBinding::Scene)->setData(&s_data.sceneBufferData, sizeof(SceneBufferData));

			VFC::setEnabled(true);
			VFC::setCamera(VFC::createFrustum_ortho(-halfDistance, halfDistance, -halfDistance, halfDistance, 0.0f, s_data.shadowDistance, lightPos, lTC.worldRotation));
			Renderer::uploadCameraBufferData(lightProj, lightView, lTC.worldPosition, lTC.eulerAnglesWorldDegrees, 0.01f, s_data.shadowDistance * 4.0f);
			
			s_data.shadowMap->bind();
			RenderCommand::clear();
			drawSceneShadow();
			s_data.shadowMap->unbind();
			VFC::setEnabled(true);

			
			s_data.shadowMap->getTexture()->getDepthAttachment()->bind(4);
		}

		bool renderedReflection = false;
		//reflection map pass
		//https://www.adriancourreges.com/blog/2015/11/02/gta-v-graphics-study/
		if (useReflectionMap && reflectionFrameBuffer && reflectionMap) {
			//						  fb,    index
			static std::unordered_map<void*, uint32_t> mapIndexMap;
			uint32_t& reflectionRenderIndex = mapIndexMap[cam.get()];
			reflectionFrameBuffer->bind();
			reflectionFrameBuffer->attachCubeMapFace(reflectionMap, reflectionRenderIndex);
			VFC::setCamera(1.0f, 0.1f, 200.0f, 90.0f, transform.worldPosition, getViewRotation(reflectionRenderIndex));
			Renderer::uploadCameraBufferData(glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 200.0f), calcView(reflectionRenderIndex, transform), transform.worldPosition, transform.eulerAnglesWorldDegrees, cameraComp.settings.zNear, cameraComp.settings.zFar);
			RenderCommand::clear();
			drawSkyBox(camSkyBoxTexture);
			drawScene(nullptr);
			reflectionFrameBuffer->unbind();
			if (reflectionRenderIndex >= 5)
				reflectionRenderIndex = 0;
			else
				reflectionRenderIndex++;

			renderedReflection = true;

		}else
			reflectionMap = nullptr;

		//default render pass
		VFC::setCamera(cameraComp.getFrustum());
		cam->bindFrameBuffer();
		Renderer::uploadCameraBufferData(proj, view, transform.worldPosition, transform.eulerAnglesWorldDegrees, cameraComp.settings.zNear, cameraComp.settings.zFar);
		drawSkyBox(camSkyBoxTexture);
		if (renderedReflection) {
			//rotation already applied
			s_data.sceneBufferData.skyBoxRotation = glm::mat4(1.0f);
			Renderer::getBuffer(BufferBinding::Scene)->setData(&s_data.sceneBufferData, sizeof(SceneBufferData));
		}
		drawScene(reflectionMap);
		cam->unbindFrameBuffer();
	}

	void SceneRenderer::Clear() {
		s_data.sceneBufferData.clearColor = glm::vec4(.0f, .0f, .0f, 1.0f);
		RenderCommand::setClearColor(glm::vec4(.0f, .0f, .0f, 1.0f));
		RenderCommand::clear();
	}
	void SceneRenderer::Clear(CameraComponent& cam) {
		RenderCommand::setDefault();
		switch (cam.settings.clearType)
		{
		case CameraComponent::ClearType::Color:
			RenderCommand::setClearColor(cam.settings.clearColor);
			s_data.sceneBufferData.clearColor = cam.settings.clearColor;
			break;
		case CameraComponent::ClearType::Skybox:
			RenderCommand::setClearColor(glm::vec4(.0f, .0f, .0f, 1.0f));
			s_data.sceneBufferData.clearColor = glm::vec4(.0f, .0f, .0f, 1.0f);
			break;
		}
		RenderCommand::clear();
	}

	void SceneRenderer::GenSceneTexture(const Ref<FrameBuffer>& sceneFbo, const Ref<Scene>& scene) {
		ST_PROFILING_FUNCTION();
		auto& cams = scene->getAllGameObjectsAsGroupWith<TransformComponent, CameraComponent>();

		cams.sort([&](const entt::entity a, const entt::entity b) {
			int32_t dA = cams.get<CameraComponent>(a).depth;
			int32_t dB = cams.get<CameraComponent>(b).depth;
			return dA < dB;
		});


		sceneFbo->bind();
		RenderCommand::setClearColor(glm::vec4(.0f, .0f, .0f, .0f));
		RenderCommand::clear();
		sceneFbo->unbind();
		for (auto& goID : cams) {
			CameraComponent& cam = cams.get<CameraComponent>(goID);
			if (!cam.settings.isRenderTarget) {
				if (cam.postProcessing)
					ApplyPostProcessing(sceneFbo, cam.getFrameBuffer()->getTexture(), *cam.postProcessing);
				else
					ApplyPostProcessing(sceneFbo, cam.getFrameBuffer()->getTexture());
			}
		}
	}

	void SceneRenderer::ApplyPostProcessing(SceneCamera& sceneCam) {
		if (sceneCam.getPostProcessingUsingMain() && Scene::activeScene()) {
			GameObject mainCam = Scene::activeScene()->getMainCamera();
			if (mainCam && mainCam.hasComponent<PostProcessingComponent>()) {
				PostProcessingComponent data = mainCam.getComponent<PostProcessingComponent>();
				for (uint32_t i = 0; i < s_data.maxSamples; i++) {
					data.data.bloomData.downSample[i] = sceneCam.getPostProcessingData().bloomData.downSample[i];
					data.data.bloomData.upSample[i] = sceneCam.getPostProcessingData().bloomData.upSample[i];
				}
				data.data.bloomData.samples = sceneCam.getPostProcessingData().bloomData.samples;

				ApplyPostProcessing(sceneCam.getCamera()->getFrameBuffer(), data.data);

				for (uint32_t i = 0; i < s_data.maxSamples; i++) {
					sceneCam.getPostProcessingData().bloomData.downSample[i] = data.data.bloomData.downSample[i];
					sceneCam.getPostProcessingData().bloomData.upSample[i] = data.data.bloomData.upSample[i];
				}
				sceneCam.getPostProcessingData().bloomData.samples = data.data.bloomData.samples;

				return;
			}
		}

		ApplyPostProcessing(sceneCam.getCamera()->getFrameBuffer(), sceneCam.getPostProcessingData());
	}
	void SceneRenderer::ApplyPostProcessing(CameraComponent& camera) {
		if(camera.postProcessing)
			ApplyPostProcessing(camera.getFrameBuffer(), *camera.postProcessing);
		else
			ApplyPostProcessing(camera.getFrameBuffer());
	}
	void SceneRenderer::ApplyPostProcessing(const Ref<FrameBuffer>& frameBuffer, PostProcessingData& data) {
		ApplyPostProcessing(frameBuffer, frameBuffer->getTexture(), data);
	}
	void SceneRenderer::ApplyPostProcessing(const Ref<FrameBuffer>& destination, const Ref<Texture>& source, PostProcessingData& data) {
		ST_PROFILING_FUNCTION();
		s_data.postProcessingBufferData.time = Time::time;
		s_data.postProcessingBufferData.delta = Time::deltaTime;
		s_data.postProcessingBufferData.toneMappingExposure = data.exposure;
		s_data.postProcessingBufferData.gamma = data.gamma;
		s_data.postProcessingBufferData.bloomStrength = data.bloomData.bloomIntensity;
		Renderer::getBuffer(BufferBinding::PostProcessing)->setData(&s_data.postProcessingBufferData, sizeof(PostProcessingBufferData));

		Ref<Texture> bloomResult = Resources::getBlackTexture();
		//bloom
		if (data.bloomData.enabled)
			bloomResult = DoBloom(destination, source, data);

		//gamma correction, tonemapping and apply bloom
		{
			source->bind(0);
			bloomResult->bind(1);
			s_data.quadPostProcShader->bind();
			destination->bind();
			float z = -1.0f;
			Renderer::getBuffer(BufferBinding::Model)->setData(&z, sizeof(float));
			RenderCommand::drawIndexed(Resources::getFullscreenVA(), 0);
			destination->unbind();
		}
	}

	Ref<Texture> SceneRenderer::DoBloom(const Ref<FrameBuffer>& destination, const Ref<Texture>& source, PostProcessingData& postProcData) {
		ST_PROFILING_SCOPE("Bloom Pass");
		static Ref<ComputeShader> filterShader;
		static Ref<ComputeShader> downSampleShader;
		static Ref<ComputeShader> upSampleShader;
		;
		if (!filterShader) {
			filterShader = ComputeShader::create("BloomFilterShader", R"(
#version 460 core
layout(local_size_x = 4, local_size_y = 4, local_size_z = 1) in;

layout(rgba16f, binding = 0) uniform readonly image2D source;
layout(rgba32f, binding = 1) uniform writeonly image2D destination;

layout(std430, binding = 6) buffer data {
    float u_treshold;
    float u_sampleScale;
	vec2 srcRes;
	vec2 desRes;
	vec2 resFactor;
};

float when_gt(float x, float y) { return max(sign(x-y), float(0.0));}
vec3 textureLoad(vec2 pos1) {
	vec2 pos = vec2(resFactor * pos1);
	return imageLoad(source, ivec2(uint(pos.x),uint(pos.y))).xyz;
}
vec3 blur(ivec2 iPos) {
	vec2 pos = vec2(float(iPos.x), float(iPos.y));

	vec3 A = textureLoad(pos + vec2(-1.0, -1.0));
    vec3 B = textureLoad(pos + vec2( 0.0, -1.0));
    vec3 C = textureLoad(pos + vec2( 1.0, -1.0));
    vec3 D = textureLoad(pos + vec2(-0.5, -0.5));
    vec3 E = textureLoad(pos + vec2( 0.5, -0.5));
    vec3 F = textureLoad(pos + vec2(-1.0,  0.0));
    vec3 G = textureLoad(pos                   );
    vec3 H = textureLoad(pos + vec2( 1.0,  0.0));
    vec3 I = textureLoad(pos + vec2(-0.5,  0.5));
    vec3 J = textureLoad(pos + vec2( 0.5,  0.5));
    vec3 K = textureLoad(pos + vec2(-1.0,  1.0));
    vec3 L = textureLoad(pos + vec2( 0.0,  1.0));
    vec3 M = textureLoad(pos + vec2( 1.0,  1.0));

    //vec2 div = (1.0 / 4.0) * vec2(0.5, 0.125);
    const vec2 div = (0.25) * vec2(0.5, 0.125);

    vec3 o = (D + E + I + J) * div.x;
    o += (A + B + G + F) * div.y;
    o += (B + C + H + G) * div.y;
    o += (F + G + L + K) * div.y;
    o += (G + H + M + L) * div.y;

    return o;
}
void main() {
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
	vec3 color = blur(pixelCoord);

	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	color *= when_gt(brightness, u_treshold);

	imageStore(destination, pixelCoord, vec4(color, 1.0));
}
)");
		}
		if (!downSampleShader) {
			downSampleShader = ComputeShader::create("BloomDownSampleShader", R"(
#version 460 core
layout(local_size_x = 4, local_size_y = 4, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform readonly image2D source;
layout(rgba32f, binding = 1) uniform writeonly image2D destination;
layout(std430, binding = 6) buffer data {
    float u_treshold;
    float u_sampleScale;
	vec2 srcRes;
	vec2 desRes;
	vec2 resFactor;
};

vec3 textureLoad(vec2 pos1) {
	vec2 pos = vec2(resFactor * pos1);
	return imageLoad(source, ivec2(int(pos.x),int(pos.y))).xyz;
}
vec3 blur(ivec2 iPos) {
	vec2 pos = vec2(float(iPos.x), float(iPos.y));

	vec3 A = textureLoad(pos + vec2(-1.0, -1.0));
    vec3 B = textureLoad(pos + vec2( 0.0, -1.0));
    vec3 C = textureLoad(pos + vec2( 1.0, -1.0));
    vec3 D = textureLoad(pos + vec2(-0.5, -0.5));
    vec3 E = textureLoad(pos + vec2( 0.5, -0.5));
    vec3 F = textureLoad(pos + vec2(-1.0,  0.0));
    vec3 G = textureLoad(pos                   );
    vec3 H = textureLoad(pos + vec2( 1.0,  0.0));
    vec3 I = textureLoad(pos + vec2(-0.5,  0.5));
    vec3 J = textureLoad(pos + vec2( 0.5,  0.5));
    vec3 K = textureLoad(pos + vec2(-1.0,  1.0));
    vec3 L = textureLoad(pos + vec2( 0.0,  1.0));
    vec3 M = textureLoad(pos + vec2( 1.0,  1.0));

    //vec2 div = (1.0 / 4.0) * vec2(0.5, 0.125);
    const vec2 div = (0.25) * vec2(0.5, 0.125);

    vec3 o = (D + E + I + J) * div.x;
    o += (A + B + G + F) * div.y;
    o += (B + C + H + G) * div.y;
    o += (F + G + L + K) * div.y;
    o += (G + H + M + L) * div.y;

    return o;
}

void main() {
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
	imageStore(destination, pixelCoord, vec4(blur(pixelCoord), 1.0));
}
)");
		}
		if (!upSampleShader) {
			upSampleShader = ComputeShader::create("BloomUpSampleShader", R"(
#version 460 core
layout(local_size_x = 4, local_size_y = 4, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform readonly image2D source;
layout(rgba32f, binding = 1) uniform readonly image2D downSampledSource;//same res as destination
layout(rgba32f, binding = 2) uniform writeonly image2D destination;
layout(std430, binding = 6) buffer data {
    float u_treshold;
    float u_sampleScale;
	vec2 srcRes;
	vec2 desRes;
	vec2 resFactor;
};
vec3 textureLoad(vec2 pos1) {
	vec2 pos = vec2(resFactor * pos1);
	return imageLoad(source, ivec2(uint(pos.x),uint(pos.y))).xyz;
}
vec3 upsample(ivec2 iPos) {
	vec2 pos = vec2(float(iPos.x), float(iPos.y));
	vec4 d = vec4(1.0, 1.0, -1.0, 0.0) * (u_sampleScale);
    vec3 s;
	s =  textureLoad(pos - d.xy);
    s += textureLoad(pos - d.wy) * 2.0;
    s += textureLoad(pos - d.zy);
		 			 
    s += textureLoad(pos + d.zw) * 2.0;
    s += textureLoad(pos       ) * 4.0;
    s += textureLoad(pos + d.xw) * 2.0;
		 			 
    s += textureLoad(pos + d.zy);
    s += textureLoad(pos + d.wy) * 2.0;
    s += textureLoad(pos + d.xy);
	
	//return s * (1.0 / 16.0);
    return s * (0.0625);
}


void main() {
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

	vec3 color = vec3(0);
	color += imageLoad(downSampledSource, pixelCoord).rgb;//downsampled version
	color += upsample(pixelCoord);
	
	imageStore(destination, pixelCoord, vec4(color, 1.0));
}
)");
		}
		uint32_t width = (uint32_t)(source->getWidth() / 2);
		uint32_t height = (uint32_t)(source->getHeight() / 2);
		auto& data = postProcData.bloomData;


		if (width == 1 || height == 1 || !data.enabled)
			return Resources::getBlackTexture();
		struct Data {
			float treshold = 1.0f;
			float sampleScale = .5f;
			glm::vec2 srcRes = glm::vec2(0.0f);
			glm::vec2 desRes = glm::vec2(0.0f);
			glm::vec2 resFactor = glm::vec2(0.0f);
		}updata;
		static Ref<ShaderStorageBuffer> storageBuffer = ShaderStorageBuffer::create(sizeof(Data), 6);
		
		if (data.downSample[0] == nullptr || data.downSample[0]->getWidth() != width || data.downSample[0]->getHeight() != height) {
			data.samples = 0;
			TextureSettings settings;
			settings.format = TextureFormat::RGBA32F;
			settings.filtering = TextureFiltering::Linear;
			settings.wrap = TextureWrap::ClampToEdge;
			for (uint32_t i = 0; i < s_data.maxSamples; i++) {
				uint32_t texWidth = (uint32_t)(width / glm::pow(2, i));
				uint32_t texHeight = (uint32_t)(height / glm::pow(2, i));
				

				if (texWidth <= s_data.minWidth || texHeight <= s_data.minHeight)
					break;

				data.downSample[i].reset();
				data.downSample[i] = Texture2D::create(texWidth, texHeight, settings);

				data.upSample[i].reset();
				data.upSample[i] = Texture2D::create(texWidth, texHeight, settings);

				data.samples++;
			}
			data.samples = glm::clamp(data.samples, 0u, s_data.maxSamples);
			data.upSample[data.samples - 1].reset();
		}
		{
			updata.treshold = data.bloomTreshold;
			updata.sampleScale = s_sampleScale;
			updata.srcRes = glm::vec2(width, height);
			updata.desRes = glm::vec2(data.downSample[0]->getWidth(), data.downSample[0]->getHeight());
			updata.resFactor = updata.desRes / updata.srcRes * 2;
			storageBuffer->setData(&updata, sizeof(Data));
		}
		filterShader->setTexture("source", 0, source, 0, AccesMode::ReadOnly);
		filterShader->setTexture("destination", 1, data.downSample[0], 0, AccesMode::WriteOnly);
		filterShader->Dispatch({ data.downSample[0]->getWidth(), data.downSample[0]->getHeight(), 1}, ComputeShader::Usage::ShaderImageAcces);

		if (data.downSample[(int)data.samples - 1] != nullptr && data.upSample[(int)data.samples - 2] != nullptr) {
			for (int i = 0; i < ((int)data.samples-1); i++) {
				Ref<Texture2D>& tex1 = data.downSample[i];
				Ref<Texture2D>& tex2 = data.downSample[i+1];

				{
					updata.srcRes = glm::vec2(tex1->getWidth(), tex1->getHeight());
					updata.desRes = glm::vec2(tex2->getWidth(), tex2->getHeight());
					updata.resFactor = updata.srcRes / updata.desRes;
					storageBuffer->setData(&updata, sizeof(Data));
				}

				downSampleShader->setTexture("source", 0, tex1, 0, AccesMode::ReadOnly);
				downSampleShader->setTexture("destination", 1, tex2, 0, AccesMode::WriteOnly);
				downSampleShader->Dispatch({ tex2->getWidth(), tex2->getHeight(), 1 }, ComputeShader::Usage::ShaderImageAcces);
			}
			for (int i = (int)data.samples - 2; i > -1; i--) {
				Ref<Texture2D>& tex1 = (i == (int)data.samples - 2) ? data.downSample[data.samples - 1] : data.upSample[i + 1];
				Ref<Texture2D>& tex2 = data.upSample[i];
				Ref<Texture2D>& tex3 = data.downSample[i];

				{
					updata.srcRes = glm::vec2(tex1->getWidth(), tex1->getHeight());
					updata.desRes = glm::vec2(tex2->getWidth(), tex2->getHeight());
					updata.resFactor = updata.srcRes / updata.desRes;
					storageBuffer->setData(&updata, sizeof(Data));
				}

				upSampleShader->setTexture("source", 0, tex1, 0, AccesMode::ReadOnly);
				upSampleShader->setTexture("destination", 2, tex2, 0, AccesMode::WriteOnly);
				upSampleShader->setTexture("downSampledSource", 1, tex3, 0, AccesMode::ReadOnly);
				upSampleShader->Dispatch({ tex2->getWidth(), tex2->getHeight(), 1 }, ComputeShader::Usage::ShaderImageAcces);
			}

			return data.upSample[0];
		}
		return Resources::getBlackTexture();
	}

	void SceneRenderer::drawSceneShadow() {
		ST_PROFILING_FUNCTION();
		//opaque
		RenderCommand::setCullMode(CullMode::BackAndFront);
		s_data.shadowShader->bind();
		for (const RenderObject& object : s_data.drawList) {
			if (object.boundingBox) {
				if (!VFC::isInView(object.boundingBox)) {
					continue;
				}
			}
			if (object.vertexArray) {
				Renderer::submit(object.vertexArray, nullptr, object.transform, object.normalMatrix);
			}
		}
	}
	void SceneRenderer::drawScene(const Ref<CubeMap>& reflectionMap) {
		ST_PROFILING_FUNCTION();
		UUID last = UUID();
		
		if (reflectionMap) {
			reflectionMap->bind(2);
			SkyBox::genrateBRDFLUT(ST_REFLECTION_MAP_SIZE)->bind(3);
		}

		//opaque
		for (const RenderObject& object : s_data.drawList) {
			if (object.boundingBox) {
				if (!VFC::isInView(object.boundingBox)) {
					continue;
				}
			}

			if (last != object.material->getUUID()) {
				object.material->getShader()->bind();
				object.material->uploadData();
				last = object.material->getUUID();
			}
			RenderCommand::setCullMode(object.cullmode);
			if (object.vertexArray) {
				Renderer::submit(object.vertexArray, nullptr, object.transform, object.normalMatrix);
			}
		}

		//transparent
		for (const auto& object : s_data.transparentDrawList) {
			if (object.boundingBox) {
				if (!VFC::isInView(object.boundingBox)) {
					continue;
				}
			}
			if (last != object.material->getUUID()) {
				object.material->uploadData();
				object.material->getShader()->bind();
				last = object.material->getUUID();
			}
			RenderCommand::setCullMode(object.cullmode);
			if (object.vertexArray) {
				Renderer::submit(object.vertexArray, nullptr, object.transform, object.normalMatrix);
			}
		}
	}
	void SceneRenderer::drawSkyBox(const Ref<CubeMap>& skybox) {
		ST_PROFILING_FUNCTION();  
		if (skybox && !(s_data.sceneBufferData.shaderFlags & ST_ShaderViewFlags_DisplayDepth)) {
			RenderCommand::setWireFrame(false);
			auto& shader = Resources::getSkyBoxShader();
			shader->bind();
			skybox->bind();
			skybox->draw();
			if (s_data.sceneBufferData.shaderFlags & ST_ShaderViewFlags_DisplayVertices)
				RenderCommand::setWireFrame(true);
		}
		
	}

	void SceneRenderer::RegisterObject(MeshRendererComponent& mesh, MeshFilterComponent& filter, TransformComponent& transform) {
		if (!filter.mesh.hasMesh)
			return;
		Ref<Material> material;
		if (mesh.material) {
			material = mesh.material;
		}
		else {
			material = Resources::getDefaultMaterial();
		}


		glm::mat4 normalMatrix = glm::transpose(glm::inverse(Math::createMat4(transform.worldPosition, transform.worldRotation, { 1,1,1 })));

		Ref<BoundingBox> boundingBox;
		if (filter.mesh.mesh->getBoundingBox()) {
			boundingBox = filter.mesh.mesh->getBoundingBox()->copy();
			boundingBox->applyTransform(transform);
		}

		if (material->isTransparent())
			s_data.transparentDrawList.push_back(RenderObject{
			material,filter.mesh.mesh->getVertexArray(),transform.transform, normalMatrix, mesh.cullmode, boundingBox });
		else
			s_data.drawList.push_back(RenderObject{
			material,filter.mesh.mesh->getVertexArray(),transform.transform, normalMatrix, mesh.cullmode, boundingBox });
	}
	void SceneRenderer::RegisterObject(const Ref<VertexArray>& vertexArray, const Ref<Material>& mat, const glm::mat4& transform, const Ref<BoundingBox>& transformedBoundingBox) {
		if (!vertexArray)
			return;
		Ref<Material> material;
		if (mat) {
			material = mat;
		}
		else {
			material = Resources::getDefaultMaterial();
		}

		if (material->isTransparent())
			s_data.transparentDrawList.push_back(RenderObject{material,vertexArray,transform, glm::transpose(glm::inverse(transform)),CullMode::Back,transformedBoundingBox });
		else
			s_data.drawList.push_back(RenderObject{ material,vertexArray,transform, glm::transpose(glm::inverse(transform)), CullMode::Back,transformedBoundingBox });
	}
	void SceneRenderer::RegisterObject(const RenderObject& object) {
		s_data.drawList.push_back(object);
	}
}