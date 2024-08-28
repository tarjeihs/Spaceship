#pragma once

#include <glm/glm.hpp>

#include "Core/Subsystem.h"

// Note: Convert radians to degrees to use an 0-360 rotation range

class PCameraSubsystem : public ISubsystem 
{
public:
    PCameraSubsystem()
    {
        Position = glm::vec3(0.0f);
        Rotation = glm::vec3(0.0f);

        LastMousePosition = glm::vec2(0.0f);
        CurrentMousePosition = glm::vec2(0.0f);

        MovementSpeed = 5.0f;
        RotationSpeed = 5.0f;
    }

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float DeltaTime) override;

protected:
    void OnWindowFocusCallback(uint32_t Focused);

protected:
    [[nodiscard]] glm::vec3 GetForwardVector() const;
    [[nodiscard]] glm::vec3 GetRightVector() const;
    [[nodiscard]] glm::vec3 GetUpVector() const;

protected:
    glm::vec3 Position;
    glm::vec3 Rotation;

    glm::vec2 LastMousePosition;
    glm::vec2 UnfocusedMousePosition;
    glm::vec2 CurrentMousePosition;

    float MovementSpeed;
    float RotationSpeed;
};

REGISTER_SUBSYSTEM(PCameraSubsystem);