#include "CameraSubsystem.h"

#include "Core/Scene.h"
#include "Core/Camera.h"
#include "Core/Window.h"
#include "Core/Input.h"
#include "Core/KeyCode.h"

void PCameraSubsystem::OnAttach()
{
    PCamera* Camera = GetScene()->ActiveCamera;
    Camera->SetPerspectiveProjection(glm::radians(45.0f), GetWindow()->GetAspectRatio(), 0.1f, 1000.0f);

    Position = glm::vec3(-2.5f, 0.0f, 0.0f);
    Rotation = glm::vec3(0.0f, 0.0f, 0.0f);

    GetWindow()->OnWindowFocusDelegate.Bind(this, &PCameraSubsystem::OnWindowFocusCallback);
}

void PCameraSubsystem::OnDetach()
{
    GetWindow()->OnWindowFocusDelegate.Unbind(this, &PCameraSubsystem::OnWindowFocusCallback);
}

void PCameraSubsystem::OnUpdate(float DeltaTime)
{
    PCamera* Camera = GetScene()->ActiveCamera;

    if (PInput::IsKeyPressed(RK_KEY_W))
    {
        Position += GetForwardVector() * MovementSpeed * DeltaTime;
    }
    if (PInput::IsKeyPressed(RK_KEY_S))
    {
        Position -= GetForwardVector() * MovementSpeed * DeltaTime;
    }
    if (PInput::IsKeyPressed(RK_KEY_D))
    {
        Position += GetRightVector() * MovementSpeed * DeltaTime;
    }
    if (PInput::IsKeyPressed(RK_KEY_A))
    {
        Position -= GetRightVector() * MovementSpeed * DeltaTime;
    }
    if (PInput::IsKeyPressed(RK_KEY_SPACE))
    {
        Position += GetUpVector() * MovementSpeed * DeltaTime;
    }
    if (PInput::IsKeyPressed(RK_KEY_LEFT_CONTROL))
    {
        Position -= GetUpVector() * MovementSpeed * DeltaTime;
    }

    if (GetWindow()->IsFocused())
    {
        CurrentMousePosition = glm::vec2(PInput::GetMouseX(), PInput::GetMouseY());

        const glm::vec2 DeltaMousePosition = CurrentMousePosition - LastMousePosition;

        Rotation.x -= DeltaMousePosition.y * RotationSpeed * DeltaTime;  // Pitch in radians
        Rotation.y += DeltaMousePosition.x * RotationSpeed * DeltaTime;  // Yaw in radians

        Rotation.x = glm::clamp(Rotation.x, -glm::half_pi<float>() + 1e-4f, glm::half_pi<float>() - 1e-4f);
        Rotation.y = std::fmod(Rotation.y, glm::two_pi<float>());

        LastMousePosition = CurrentMousePosition;
    }

    Camera->CalculateViewMatrix(Position, Rotation);
}

void PCameraSubsystem::OnWindowFocusCallback(uint32_t Focused)
{
    if (Focused)
    {
        GetWindow()->WaitEventOrTimeout(0.1f);

        LastMousePosition = glm::vec2(PInput::GetMouseX(), PInput::GetMouseY());
    }
}

glm::vec3 PCameraSubsystem::GetForwardVector() const
{
    glm::vec3 ForwardVector;
    ForwardVector.x = glm::cos(Rotation.y) * glm::cos(Rotation.x);
    ForwardVector.y = glm::sin(Rotation.x);
    ForwardVector.z = glm::sin(Rotation.y) * glm::cos(Rotation.x);
    ForwardVector = glm::normalize(ForwardVector);
    return ForwardVector;
}

glm::vec3 PCameraSubsystem::GetRightVector() const
{
    return glm::normalize(glm::cross(GetForwardVector(), GetUpVector()));
}

glm::vec3 PCameraSubsystem::GetUpVector() const
{
    return { 0.0f, 1.0f, 0.0f };
}
