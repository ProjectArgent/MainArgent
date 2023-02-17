#pragma once
#include "Argent/Scene/Scene.h"

class Title final :
    public Scene
{
public:
    Title(std::string sceneName):
		Scene(sceneName)
	{}
	~Title() override = default;
		
	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Render() override;
	void DrawDebug() override;

private:
};

