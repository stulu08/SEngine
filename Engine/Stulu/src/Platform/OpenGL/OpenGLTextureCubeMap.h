#pragma once
#include "Stulu/Renderer/Texture.h"
#include <Stulu/Renderer/Shader.h>
#include "OpenGLVertexArray.h"
namespace Stulu {
	class STULU_API OpenGLCubeMap : public virtual CubeMap {
	public:
		OpenGLCubeMap(uint32_t resolution, TextureSettings settings);
		OpenGLCubeMap(uint32_t resolution, const std::vector<std::string>& faces, TextureSettings settings);

		virtual ~OpenGLCubeMap();

		virtual void bind(uint32_t slot) const override;
		virtual void GenerateMips() const override;

		virtual void* getNativeRendererObject() const override { return (void*)(&m_map); }
		virtual uint32_t getWidth() const override { return m_resolution; }
		virtual uint32_t getHeight() const override { return m_resolution; }
		virtual TextureSettings& getSettings() override { return m_settings; }

		virtual bool operator == (const Texture& other) const override;
		virtual operator int() override { return m_map; }
	private:
		uint32_t m_resolution;
		uint32_t m_map = 0;
		TextureSettings m_settings;
	};
}