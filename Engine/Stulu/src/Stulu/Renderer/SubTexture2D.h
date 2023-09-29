#pragma once
#include "Texture.h"
namespace Stulu {
	class STULU_API SubTexture2D {
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);

		const Ref<Texture2D> getTexture() const { return m_texture; };
		const glm::vec2* getTexCoords() const { return m_texCoords; };

		static Ref<SubTexture2D> fromCoords(const glm::vec2& spriteCoords, const glm::vec2& spriteCellSize, const Ref<Texture2D>& texture, const glm::vec2& spriteSize = { 1.0f,1.0f });
		/// <summary>
		/// Get's the texture in pixel Coords
		/// </summary>
		static Ref<SubTexture2D> fromPosition(const glm::vec2& spritePos, const glm::vec2& spriteSize, const Ref<Texture2D>& texture);
	private:
		Ref<Texture2D> m_texture;
		glm::vec2 m_texCoords[4];
	};
}