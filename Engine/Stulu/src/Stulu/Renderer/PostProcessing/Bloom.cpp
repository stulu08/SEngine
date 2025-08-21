#include "st_pch.h"
#include "Bloom.h"

namespace Stulu {
	BloomEffect::BloomEffect()
	{
		m_shader = Renderer::getShaderSystem()->GetShader("Renderer/PostProcessing/Bloom");
		m_filterShader = Renderer::getShaderSystem()->GetShader("Renderer/PostProcessing/Bloom/Filter");
		m_downSampleShader = Renderer::getShaderSystem()->GetShader("Renderer/PostProcessing/Bloom/Down");
		m_upSampleShader = Renderer::getShaderSystem()->GetShader("Renderer/PostProcessing/Bloom/Up");
	}

	void BloomEffect::Apply(Ref<FrameBuffer> destination, const Texture2D* source) {

		Texture2D* bloomResult = CreateBloom(source);

		Renderer::uploadBufferData(BufferBinding::PostProcessing, &intensity, sizeof(float));

		source->bind(0);
		bloomResult->bind(1);
		Renderer::ScreenQuad(destination, m_shader);
	}

	Texture2D* BloomEffect::CreateBloom(const Texture2D* source) {
		ST_PROFILING_SCOPE("Renderer - Bloom");
		uint32_t width = (uint32_t)((float)source->getWidth() * this->resolutionScale);
		uint32_t height = (uint32_t)((float)source->getHeight() * this->resolutionScale);

		if (width <= 1 || height <= 1 || !IsEnabled())
			return Resources::BlackTexture();

		float logs = glm::log2((float)glm::max(width, height)) + glm::min((float)this->diffusion, (float)BLOOM_MAX_SAMPLES) - BLOOM_MAX_SAMPLES;
		int32_t logs_i = (int)glm::floor(logs);
		uint32_t targetSamples = glm::clamp(logs_i, 1, (int32_t)maxSamples);

		if (this->downSample == nullptr || this->downSample->getWidth() != width || this->downSample->getHeight() != height || this->samples != targetSamples) {
			this->downSample.reset();
			this->upSample.reset();
			this->samples = targetSamples;
			this->sampleScale = 0.5f + logs - logs_i;

			TextureSettings settings = TextureFormat::RGBA32F;
			settings.filtering = TextureFiltering::Trilinear;
			settings.wrap = TextureWrap::ClampToEdge;
			settings.levels = this->samples;

			this->downSample = Texture2D::create(width, height, settings);
			this->upSample = Texture2D::create(width, height, settings);

		}
		struct Data {
			glm::vec4 filter;
			glm::vec2 desRes;
			float sampleScale;
			float clamp;
			uint32_t iteration;
		}updata = {};
		
		static Ref<ShaderStorageBuffer> storageBuffer = ShaderStorageBuffer::create(sizeof(Data), 6);


		float soft = this->threshold * this->knee;
		updata.filter.x = this->threshold;
		updata.filter.y = this->threshold - soft;
		updata.filter.z = 2.0f * soft;
		updata.filter.w = 0.25f / (soft + 0.00001f);
		updata.sampleScale = this->sampleScale;
		updata.clamp = this->clamp;
		updata.iteration = 0;

		for (int i = 0; i < (int)this->samples; i++) {
			Ref<Shader> shader = (i == 0) ? m_filterShader : m_downSampleShader;
			const Texture2D* sourceTex = (i == 0) ? source : this->downSample.get();

			updata.desRes = glm::vec2(this->downSample->getMipWidth(i), this->downSample->getMipHeight(i)) - glm::vec2(1, 1);
			storageBuffer->setData(&updata, sizeof(Data));

			sourceTex->bind(0);
			shader->setTexture("destination", 2, this->downSample, i, AccesMode::WriteOnly);
			shader->Dispatch({ this->downSample->getMipWidth(i), this->downSample->getMipHeight(i), 1 }, ComputeUsage::ShaderImageAcces | ComputeUsage::TextureFetch);
			updata.iteration = i;
		}

		for (int i = (int)this->samples - 2; i >= 0; i--) {
			const Texture2D* sourceTex = (i == (int)this->samples - 2) ? this->downSample.get() : this->upSample.get();

			updata.desRes = glm::vec2(this->upSample->getMipWidth(i), this->upSample->getMipHeight(i)) - glm::vec2(1, 1);
			storageBuffer->setData(&updata, sizeof(Data));

			sourceTex->bind(0);
			this->downSample->bind(1);
			m_upSampleShader->setTexture("destination", 2, this->upSample, i, AccesMode::WriteOnly);
			m_upSampleShader->Dispatch({ this->upSample->getMipWidth(i), this->upSample->getMipHeight(i), 1 }, ComputeUsage::ShaderImageAcces | ComputeUsage::TextureFetch);

			updata.iteration = i;
		}

		return this->upSample.get();
	}
}
