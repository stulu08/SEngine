#pragma once
#include "Stulu/Core/Log.h"
#include "Stulu/Math/Math.h"
namespace Stulu{
	enum class BufferBinding : uint32_t {
		Camera = 0,
		Model = 1,
		Light = 2,
		PostProcessing = 3,
		Scene = 4,
		// Internal Material Data
		Material = 5,
		// This binding will be used to upload materials
		UserMaterial = 6,
		Last = UserMaterial,
		Next = Last + 1
	};
	enum class ShaderDataType { none = 0,
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		Mat3, Mat4,
		Bool, Sampler
	};
	enum class BufferElementIDType {
		Position, 
		Normal,
		Tangent,
		Color,
		TextureCoords,
		BoneWeights,
		BoneIndices,
		Other
	};

	inline constexpr uint32_t shaderDataTypeSize(ShaderDataType type) {
		switch (type) {
			case ShaderDataType::Float:		return 4;
			case ShaderDataType::Float2:	return 8;
			case ShaderDataType::Float3:	return 12;
			case ShaderDataType::Float4:	return 16;

			case ShaderDataType::Int:		return 4;
			case ShaderDataType::Int2:		return 8;
			case ShaderDataType::Int3:		return 12;
			case ShaderDataType::Int4:		return 16;

			case ShaderDataType::Mat3:		return 4 * 3 * 3;
			case ShaderDataType::Mat4:		return 4 * 4 * 4;

			case ShaderDataType::Bool:		return 1;

			case ShaderDataType::Sampler:	return 4;
		}
		CORE_ERROR("Uknown ShaderDataType: {0}", (int)type);
		return 0;
	}

	struct BufferElement {
		BufferElementIDType idType;
		ShaderDataType type;
		uint32_t offset;
		uint32_t size;
		bool normalized;

		inline constexpr BufferElement(ShaderDataType _type, BufferElementIDType _idType, bool _normalized = false)
			: idType(_idType), type(_type), size(shaderDataTypeSize(_type)), offset(0), normalized(_normalized) {}

		uint32_t getComponentCount() const{

			switch (type) {
				case ShaderDataType::Float:		return 1;
				case ShaderDataType::Float2:	return 2;
				case ShaderDataType::Float3:	return 3;
				case ShaderDataType::Float4:	return 4;

				case ShaderDataType::Int:		return 1;
				case ShaderDataType::Int2:		return 2;
				case ShaderDataType::Int3:		return 3;
				case ShaderDataType::Int4:		return 4;

				case ShaderDataType::Mat3:		return 3 * 3;
				case ShaderDataType::Mat4:		return 4 * 4;

				case ShaderDataType::Bool:		return 1;

				case ShaderDataType::Sampler:	return 1;
			}
			CORE_ERROR("Uknown ShaderDataType: {0}", (int)type);
			return 0;
		}
	};

	inline BufferElement EmptyBufferElement { ShaderDataType::Float, BufferElementIDType::Other };
	constexpr inline BufferElement PositionBufferElement { ShaderDataType::Float3, BufferElementIDType::Position };
	constexpr inline BufferElement NormalBufferElement { ShaderDataType::Float3, BufferElementIDType::Normal, true };
	constexpr inline BufferElement TextureCoordsBufferElement{ ShaderDataType::Float2, BufferElementIDType::TextureCoords };
	constexpr inline BufferElement ColorBufferElement{ ShaderDataType::Float4, BufferElementIDType::Color };
	constexpr inline BufferElement BoneWeightsBufferElement { ShaderDataType::Float4, BufferElementIDType::BoneWeights };
	constexpr inline BufferElement BoneIndicesBufferElement { ShaderDataType::Int4, BufferElementIDType::BoneIndices };

	class STULU_API BufferLayout {
	public:
		BufferLayout(){}
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_elements(elements) {
			CalculateOffsetsAndStride();
		}
		inline void addElement(const BufferElement& ele) { 
			m_elements.push_back(ele);
			CalculateOffsetsAndStride();
		}
		inline const std::vector<BufferElement>& getElements() const { return m_elements; }
		inline const uint32_t getStride() const { return m_stride; }

		std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_elements.end(); }

		std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_elements.end(); }
	private:
		void CalculateOffsetsAndStride() {
			uint32_t off = 0;
			m_stride = 0;
			for (auto& element : m_elements) {
				element.offset = off;
				off += element.size;
				m_stride += element.size;
			}
		}

		std::vector<BufferElement> m_elements;
		uint32_t m_stride = 0;
	};
	enum class BufferDrawMode {
		Static, // The data store contents will be modified once and used many times.
		Dynamic, // The data store contents will be modified repeatedly and used many times.
		Stream // The data store contents will be modified once and used at most a few times.
	};
	class STULU_API GraphicsBuffer {
	public:
		virtual ~GraphicsBuffer() = default;

		virtual void setData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		virtual BufferDrawMode getBufferDrawMode() const = 0;
		virtual uint32_t getCount() const = 0;
		virtual uint32_t getStride() const = 0;
		virtual uint32_t getSize() const = 0;
		virtual void* getNativeRendererObject() const = 0;
	};
	class STULU_API VertexBuffer : public GraphicsBuffer {
	public:
		virtual void setLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& getLayout()const = 0;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		static Ref<VertexBuffer> create(uint32_t size, BufferDrawMode mode = BufferDrawMode::Dynamic);
		static Ref<VertexBuffer> create(uint32_t size, float* vertices, BufferDrawMode mode = BufferDrawMode::Static);
		static Ref<VertexBuffer> create(uint32_t size, const void* data, BufferDrawMode mode = BufferDrawMode::Static);
	};
	class STULU_API IndexBuffer : public GraphicsBuffer {
	public:
		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		static Ref<IndexBuffer> create(uint32_t count, uint32_t* indices, BufferDrawMode mode = BufferDrawMode::Dynamic);
	};
	class STULU_API UniformBuffer : public GraphicsBuffer {
	public:
		virtual ~UniformBuffer() = default;
		virtual void bind(uint32_t binding) const = 0;
		virtual void setData(const void* data) = 0;
		virtual void setData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		static Ref<UniformBuffer> create(uint32_t size, uint32_t binding);
	};
	class STULU_API ShaderStorageBuffer : public GraphicsBuffer {
	public:
		virtual ~ShaderStorageBuffer() = default;
		virtual void bind(uint32_t binding = (uint32_t)BufferBinding::Next) const = 0;
		virtual void setData(const void* data) = 0;
		virtual void setData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		virtual void getData(void* data, uint32_t size, uint32_t offset = 0) const = 0;
		virtual void getData(void* data) const = 0;

		static Ref<ShaderStorageBuffer> create(uint32_t size, uint32_t binding = (uint32_t)BufferBinding::Next, BufferDrawMode mode = BufferDrawMode::Dynamic);
	};
}