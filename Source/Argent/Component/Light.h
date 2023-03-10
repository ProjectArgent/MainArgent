#pragma once
#include "ArComponent.h"

class Light :
    public Argent::Component::ArComponent
{
public:
    Light(std::string name = "Light", DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1)):
		ArComponent(name)
    ,   color(color)
    {}
    virtual ~Light() = default;


    void Reset() override;
#ifdef _DEBUG
    void DrawDebug() override;
#endif
    [[nodiscard]] Color GetColor() const { return color; }


protected:
    Color color;
};

