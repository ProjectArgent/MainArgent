#pragma once
#include "Argent/Scene/Scene.h"

class Game final:
    public Scene
{
public:
    Game(const std::string& sceneName):
		Scene(sceneName)
    {}
    ~Game() override = default;


    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Render() override;

private:
};

