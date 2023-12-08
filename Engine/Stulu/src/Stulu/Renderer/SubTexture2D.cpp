#include "st_pch.h"
#include "SubTexture2D.h"


namespace Stulu {

	SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max)
		:m_texture(texture) {
		ST_PROFILING_FUNCTION();
		m_texCoords[0] = { min.x, min.y };
		m_texCoords[1] = { max.x, min.y };
		m_texCoords[2] = { max.x, max.y };
		m_texCoords[3] = { min.x, max.y };
	}

	Ref<SubTexture2D> SubTexture2D::fromCoords(const glm::vec2& spriteCoords, const glm::vec2& spriteCellSize, const Ref<Texture2D>& texture, const glm::vec2& spriteSize) {

		float textureWidth = (float)texture->getWidth(), textureHeight = (float)texture->getHeight();

		glm::vec2 texCoords[] = { 
			glm::vec2(spriteCoords.x * spriteCellSize.x / textureWidth, spriteCoords.y * spriteCellSize.y / textureHeight),
			glm::vec2((spriteCoords.x + spriteSize.x)* spriteCellSize.x / textureWidth,(spriteCoords.y + spriteSize.y) * spriteCellSize.y / textureHeight -( .1f / textureHeight)),
		};



		return createRef<SubTexture2D>(texture, texCoords[0], texCoords[1]);
	}

	Ref<SubTexture2D> SubTexture2D::fromPosition(const glm::vec2& spritePos, const glm::vec2& spriteSize, const Ref<Texture2D>& texture)
	{

		float textureWidth = (float)texture->getWidth(), textureHeight = (float)texture->getHeight();

		glm::vec2 texCoords[] = {
			glm::vec2(spritePos.x / textureWidth, spritePos.y / textureHeight),
			glm::vec2((spritePos.x +  spriteSize.x) / textureWidth, (spritePos.y + spriteSize.y) / textureHeight),
		};



		return createRef<SubTexture2D>(texture, texCoords[0], texCoords[1]);
	}

}