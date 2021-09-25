#pragma once
#include "Stulu/Core/Log.h"
namespace Stulu{
	enum class ShaderDataType { none = 0,
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		Mat3, Mat4,
		Bool
	};

	static uint32_t shaderDataTypeSize(ShaderDataType type) {
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
		}
		CORE_ASSERT(false, "Uknown ShaderDataType");
		return 0;
	}

	struct BufferElement {
		std::string name;
		ShaderDataType type;
		uint32_t offset;
		uint32_t size;
		bool normalized;

		BufferElement(){}

		BufferElement(ShaderDataType _type, const std::string& _name, bool _normalized = false)
			: name(_name), type(_type), size(shaderDataTypeSize(_type)), offset(0), normalized(_normalized) {}

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
			}
			CORE_ASSERT(false, "Uknown ShaderDataType");
			return 0;
		}
	};

	class BufferLayout {
	public:
		BufferLayout(){}
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_elements(elements){
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

	class VertexBuffer {
	public:
		virtual ~VertexBuffer() = default;

		virtual void setLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& getLayout()const = 0;
		virtual void setData(const void* data, uint32_t size) = 0;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;


		static Ref<VertexBuffer> create(uint32_t size);
		static Ref<VertexBuffer> create(uint32_t size, float* vertices);
		static Ref<VertexBuffer> create(uint32_t size, const void* data);
	};
	class IndexBuffer {
	public:
		virtual ~IndexBuffer() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual uint32_t getCount() const = 0;

		static Ref<IndexBuffer> create(uint32_t count, uint32_t* vertices);
	};
}