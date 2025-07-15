#include "st_pch.h"
#include "Effect.h"

namespace Stulu {
	void GammaCorrectionEffect::Apply(Ref<FrameBuffer> destination, const Texture2D* source) {
		struct UploadData {
			float enableGammaCorrection;
			float toneMappingMode;
			float exposure;
			float maxWhite;
			float gamma;
		}data = {};
		data.enableGammaCorrection = this->enableGammaCorrection ? 1.0f : 0.0f;
		data.toneMappingMode = (float)this->toneMappingMode;
		data.exposure = this->exposure;
		data.maxWhite = this->maxWhite;
		data.gamma = this->gamma;

		Renderer::uploadBufferData(BufferBinding::PostProcessing, &data, sizeof(UploadData));

		source->bind(0);
		Renderer::ScreenQuad(destination, m_shader);
	}
}
