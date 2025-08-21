#pragma once
#include "Component.h"
#include "Stulu/Renderer/PostProcessing/Effect.h"

namespace Stulu {

	class STULU_API PostProcessingComponent : public Component {
	private:
		std::vector<Ref<PostProcessingEffect>> m_effects;
		friend class SceneRenderer;
	public:
		PostProcessingComponent();
		PostProcessingComponent(const PostProcessingComponent&);

		bool HasEffect(uint32_t id) const;
		PostProcessingEffect* GetEffect(uint32_t id) const;
		PostProcessingEffect* AddEffect(Ref<PostProcessingEffect> effect);
		void RemoveEffect(uint32_t id);

		template<class T>
		inline bool HasEffect() const { return HasEffect(T::STATIC_ID()); }
		template<class T>
		inline T* GetEffect() const { return dynamic_cast<T*>(GetEffect(T::STATIC_ID())); }
		template<class T>
		inline T* AddEffect() { return dynamic_cast<T*>(AddEffect(createRef<T>())); }
		template<class T>
		inline void RemoveEffect() { return RemoveEffect(T::STATIC_ID()); }

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::Node& node) override;
	};
}