#include "st_pch.h"
#include "TextureAssets.h"
#include "Stulu/Resources/AssetsManager.h"

namespace Stulu {
	TextureSettings Stulu::SharedTexture2DAssetData::ReadSettings() {
		const auto& manager = AssetsManager::GlobalInstance();
		MetaInfo info;
		if (manager.ReadFileMeta(&info, GetPath())) {
			TextureSettings settings = TextureFormat::Auto;
			settings.border = manager.GetMetaValue(info, "Border", settings.border);
			settings.tiling = manager.GetMetaValue(info, "Tiling", settings.tiling);
			settings.levels = manager.GetMetaValue(info, "Levels", settings.levels);
			settings.format = manager.GetMetaValue(info, "Format", settings.format);
			settings.wrap = manager.GetMetaValue(info, "Wrap", settings.wrap);
			settings.filtering = manager.GetMetaValue(info, "Filtering", settings.filtering);
			return settings;
		}
		return TextureFormat::Auto;
	}

	bool Stulu::SharedTexture2DAssetData::SaveSetting(TextureSettings settings) const {
		const auto& manager = AssetsManager::GlobalInstance();
		MetaInfo info;
		if (manager.ReadFileMeta(&info, GetPath())) {
			manager.SetMetaValue(info, "Border", settings.border);
			manager.SetMetaValue(info, "Tiling", settings.tiling);
			manager.SetMetaValue(info, "Levels", settings.levels);
			manager.SetMetaValue(info, "Format", settings.format);
			manager.SetMetaValue(info, "Wrap", settings.wrap);
			manager.SetMetaValue(info, "Filtering", settings.filtering);
			return manager.WriteFileMeta(info, GetPath());
		}
		return false;
	}
	bool SharedSkyBoxAssetData::Load() {
		const auto& manager = AssetsManager::GlobalInstance();
		if (!IsMemoryLoaded()) {
			MetaInfo info;
			if (manager.ReadFileMeta(&info, GetPath())) {
				m_texture = SkyBox::create(GetPath(), manager.GetMetaValue(info, "Resolution", 2048));
			}
			else {
				m_texture = SkyBox::create(GetPath());
			}
		}
		return m_texture != nullptr;
	}
	bool SharedSkyBoxAssetData::Save() const {
		const auto& manager = AssetsManager::GlobalInstance();
		if (!IsMemoryLoaded() && m_texture) {
			MetaInfo info;
			if (manager.ReadFileMeta(&info, GetPath())) {
				manager.SetMetaValue(info, "Resolution", m_texture->getWidth());
				return manager.WriteFileMeta(info, GetPath());
			}
		}
		return false;
	}
}