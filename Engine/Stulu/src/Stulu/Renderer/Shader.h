#pragma once
#include "Stulu/Renderer/Texture.h"

#include <vector>

namespace Stulu {
	enum class ShaderType : uint32_t {
		None, Vertex, Fragment, Compute
	};
	enum class AccesMode {
		ReadOnly = 0x88B8,
		WriteOnly = 0x88B9,
		ReadWrite = 0x88BA
	};
	struct ComputeUsage {
		static inline const uint32_t None = 0x00000000;

		// Ensures that vertex attribute arrays are finished before subsequent commands are processed
		static inline const uint32_t VertexAttribArray = 0x00000001;

		// Guarantees that the element array buffer is complete before subsequent rendering commands are executed
		static inline const uint32_t ElementArray = 0x00000002;

		// Synchronizes uniform buffer modifications, ensuring they are completed before subsequent commands are processed
		static inline const uint32_t Uniform = 0x00000004;

		// Ensures that texture fetches are completed before subsequent commands, useful when reading from textures in shaders
		static inline const uint32_t TextureFetch = 0x00000008;

		// Synchronizes access to image load/store functionality in shaders. Ensures that image accesses are complete before subsequent commands are processed
		static inline const uint32_t ShaderImageAcces = 0x00000020;

		// A general-purpose barrier bit that can be used for any kind of command
		static inline const uint32_t Command = 0x00000040;

		// Synchronizes access to pixel buffer objects (PBOs). Ensures that pixel buffer modifications are complete before subsequent commands are processed
		static inline const uint32_t PixelBuffer = 0x00000080;

		// Ensures that texture updates are finished before subsequent commands
		static inline const uint32_t TextureUpdate = 0x00000100;

		// Synchronizes updates to buffer objects, ensuring they are complete before subsequent commands
		static inline const uint32_t BufferUpdate = 0x00000200;

		// Guarantees that framebuffer modifications are completed before subsequent rendering commands
		static inline const uint32_t Framebuffer = 0x00000400;

		// Ensures that transform feedback operations are complete before subsequent commands are processed
		static inline const uint32_t TransformFeedback = 0x00000800;

		// Synchronizes atomic counter modifications, ensuring they are complete before subsequent commands
		static inline const uint32_t AtomicCounter = 0x00001000;

		// Synchronizes access to shader storage blocks, ensuring modifications are complete before subsequent commands
		static inline const uint32_t ShaderStorage = 0x00002000;

		// All supported barriers for the corresponding command will be inserted
		static inline const uint32_t All = 0xFFFFFFFF;
	};
	struct ShaderSource {
	public:
		ShaderSource() {

		}
		ShaderSource(const std::string& vertex, const std::string& fragment) {
			Add(ShaderType::Vertex, vertex);
			Add(ShaderType::Fragment, fragment);
		}
		ShaderSource(const std::string& compute) {
			Add(ShaderType::Compute, compute);
		}

		inline void Add(ShaderType type, const std::string& source) {
			m_sources.push_back({ type, source });
		}

		inline size_t Size() const { return m_sources.size(); }

		inline const std::pair<ShaderType, std::string>& Get(size_t index) const { return m_sources.at(index); }
		inline std::pair<ShaderType, std::string>& Get(size_t index) { return m_sources[index]; }
		inline std::pair<ShaderType, std::string>& operator[](size_t index) { return m_sources[index]; }
	private:
		std::vector<std::pair<ShaderType, std::string>> m_sources;
	};
	class STULU_API Shader {
	public:
		virtual ~Shader() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual const std::string& getName() const = 0;

		static Ref<Shader> create(const std::string& name, const ShaderSource& sources);
		static Ref<Shader> create(const std::string& name, const std::string& vertex, const std::string& fragment) { return create(name, ShaderSource{ vertex, fragment }); }
		static Ref<Shader> create(const std::string& name, const std::string& compute) { return create(name, ShaderSource{ compute }); }

		virtual void Dispatch(const glm::uvec3& numWorkGroups = { 1,1,1 }, uint32_t usage = ComputeUsage::All) = 0;
		virtual void Dispatch(const uint32_t numWorkGroupsX = 1, const uint32_t numWorkGroupsY = 1, const uint32_t numWorkGroupsZ = 1, uint32_t usage = ComputeUsage::All) = 0;

		virtual void reload(const ShaderSource& sources) = 0;
		virtual void reload(const std::string& vertex, const std::string& fragment) = 0;
		virtual void reload(const std::string& compute) = 0;

		virtual void setMat(const std::string& name, const glm::mat4& mat) = 0;
		virtual void setVec(const std::string& name, const glm::vec4& value) = 0;
		virtual void setFloat(const std::string& name, const float Float) = 0;
		virtual void setInt(const std::string& name, const int32_t Int) = 0;
		virtual void setIntArray(const std::string& name, const int* values, uint32_t count) = 0;
		virtual void setFloatArray(const std::string& name, const float* floats, uint32_t count) = 0;
		virtual void setVecArray(const std::string& name, const glm::vec4* vecs, uint32_t count) = 0;
		virtual void setMatArray(const std::string& name, const glm::mat4* mats, uint32_t count) = 0;

		virtual void setTexture(const std::string& name, uint32_t binding, const Ref<Texture>& texture, uint32_t mipLevel = 0, AccesMode mode = AccesMode::ReadWrite) = 0;
		virtual void setTextureInternal(const std::string& name, uint32_t binding, void* texture, uint32_t mipLevel = 0, AccesMode mode = AccesMode::ReadWrite, TextureFormat format = TextureFormat::Auto) = 0;
		
		virtual const void* getNativeRendererObject() const = 0;
	};
}