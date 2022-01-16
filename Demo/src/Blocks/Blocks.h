#pragma once
#include "Stulu.h"
namespace Demo {
	using namespace Stulu;
	enum class BlockType {
		BedRock, Air, Stone, Grass, Dirt, DiamondBlock
	};
	struct Block {
		BlockType type;
		Ref<SubTexture2D> top;
		Ref<SubTexture2D> bottom;
		Ref<SubTexture2D> left;
		Ref<SubTexture2D> right;
		Ref<SubTexture2D> front;
		Ref<SubTexture2D> back;
		bool transparent = false;
		struct Material {
			float metallic = .0f;
			float roughness = .0f;
			float ambienOcullision = .2f;
		} mat;
		Block() {
			type = BlockType::Air;
			top = createRef<SubTexture2D>(nullptr,glm::vec2(.0f),glm::vec2(1.0f));
			bottom = createRef<SubTexture2D>(nullptr,glm::vec2(.0f),glm::vec2(1.0f));
			left = createRef<SubTexture2D>(nullptr,glm::vec2(.0f),glm::vec2(1.0f));
			right = createRef<SubTexture2D>(nullptr,glm::vec2(.0f),glm::vec2(1.0f));
			front = createRef<SubTexture2D>(nullptr,glm::vec2(.0f),glm::vec2(1.0f));
			back = createRef<SubTexture2D>(nullptr,glm::vec2(.0f),glm::vec2(1.0f));
		}
		Block(BlockType type, const Ref<SubTexture2D>& texAtalsPos, bool transparent = false, Material mat = Material())
			: type(type), top(texAtalsPos), bottom(texAtalsPos), left(texAtalsPos), right(texAtalsPos), front(texAtalsPos), back(texAtalsPos),transparent(transparent), mat(mat) {
		}
		Block(BlockType type, const Ref<SubTexture2D>& texAtalsPos, const Ref<SubTexture2D>& top, bool transparent = false, Material mat = Material())
			: type(type), top(top), bottom(texAtalsPos), left(texAtalsPos), right(texAtalsPos), front(texAtalsPos), back(texAtalsPos), transparent(transparent), mat(mat) {
		}
		Block(BlockType type, const Ref<SubTexture2D>& texAtalsPos, const Ref<SubTexture2D>& top, const Ref<SubTexture2D>& bottom, bool transparent = false, Material mat = Material())
			: type(type), top(top), bottom(bottom), left(texAtalsPos), right(texAtalsPos), front(texAtalsPos), back(texAtalsPos), transparent(transparent), mat(mat) {
		}
		Block(BlockType type, const Ref<SubTexture2D>& texAtalsPos, const Ref<SubTexture2D>& top, const Ref<SubTexture2D>& bottom, const Ref<SubTexture2D>& front, bool transparent = false, Material mat = Material())
			: type(type), top(top), bottom(bottom), left(texAtalsPos), right(texAtalsPos), front(front), back(texAtalsPos), transparent(transparent), mat(mat) {
		}
		Block(BlockType type, const Ref<SubTexture2D>& texAtalsPos, const Ref<SubTexture2D>& top, const Ref<SubTexture2D>& bottom, const Ref<SubTexture2D>& front, const Ref<SubTexture2D>& back, bool transparent = false, Material mat = Material())
			: type(type), top(top), bottom(bottom), left(texAtalsPos), right(texAtalsPos), front(front), back(back), transparent(transparent), mat(mat) {
		}
	};
}
namespace std {
	template<>
	struct hash<Demo::BlockType> {
		std::size_t operator()(const Demo::BlockType& uuid) const {
			return hash<int>()((int)uuid);
		}
	};
}