#include <Stulu.h>

using namespace Stulu;

class DefaultLayer : public Layer {
public:
	DefaultLayer()
		: Layer("Default") {
		mat = Material("Stulu/assets/Shaders/pbr.glsl", 
			MaterialData(std::vector<MaterialDataType>{
			MaterialDataType{ ShaderDataType::Float, 3.0f,"metallic" },
			MaterialDataType{ ShaderDataType::Float3, 3.0f,"albedo" },
			MaterialDataType{ ShaderDataType::Float, 3.0f,"roughness" },
			MaterialDataType{ ShaderDataType::Float, 3.0f,"ao" },
		}));
	}
	virtual void onAttach() override;
	virtual void onUpdate(Timestep ts) override;
	virtual void onImguiRender(Timestep ts) override;
	virtual void onEvent(Event& e) override;


private:
	Material mat;
};
