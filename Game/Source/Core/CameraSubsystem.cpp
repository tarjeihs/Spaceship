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
    if (!GetWindow()->IsFocused())
    {
        return;
    }

    PCamera* Camera = GetScene()->ActiveCamera;

    if (PInput::GetKeyPressed(RK_KEY_W))
    {
        Position += GetForwardVector() * KeyboardMovementSpeed * DeltaTime;
    }
    if (PInput::GetKeyPressed(RK_KEY_S))
    {
        Position -= GetForwardVector() * KeyboardMovementSpeed * DeltaTime;
    }
    if (PInput::GetKeyPressed(RK_KEY_D))
    {
        Position += GetRightVector() * KeyboardMovementSpeed * DeltaTime;
    }
    if (PInput::GetKeyPressed(RK_KEY_A))
    {
        Position -= GetRightVector() * KeyboardMovementSpeed * DeltaTime;
    }
    if (PInput::GetKeyPressed(RK_KEY_SPACE))
    {
        Position += GetUpVector() * KeyboardMovementSpeed * DeltaTime;
    }
    if (PInput::GetKeyPressed(RK_KEY_LEFT_CONTROL))
    {
        Position -= GetUpVector() * KeyboardMovementSpeed * DeltaTime;
    }

    if (int32_t JoystickID = PInput::GetFirstJoystickID(); JoystickID >= 0)
    {
        if (PInput::GetJoystickButton(JoystickID, RK_GAMEPAD_BUTTON_A))
        {
            Position += GetUpVector() * JoystickMovementSpeed * DeltaTime;
        }
        if (PInput::GetJoystickButton(JoystickID, RK_GAMEPAD_BUTTON_B))
        {
            Position -= GetUpVector() * JoystickMovementSpeed * DeltaTime;
        }

        Position -= GetForwardVector() * JoystickMovementSpeed * PInput::GetJoystickAxis(JoystickID, RK_GAMEPAD_AXIS_LEFT_Y) * DeltaTime;
        Position += GetRightVector() * JoystickMovementSpeed * PInput::GetJoystickAxis(JoystickID, RK_GAMEPAD_AXIS_LEFT_X) * DeltaTime;

        Rotation.x -= PInput::GetJoystickAxis(JoystickID, RK_GAMEPAD_AXIS_RIGHT_Y) * JoystickRotationSpeed * DeltaTime;  // Pitch in radians
        Rotation.y += PInput::GetJoystickAxis(JoystickID, RK_GAMEPAD_AXIS_RIGHT_X) * JoystickRotationSpeed * DeltaTime;  // Yaw in radians
    }

    CurrentMousePosition = glm::vec2(PInput::GetMouseX(), PInput::GetMouseY());
    const glm::vec2 DeltaMousePosition = CurrentMousePosition - LastMousePosition;
    LastMousePosition = CurrentMousePosition;

    Rotation.x -= DeltaMousePosition.y * MouseRotationSpeed * DeltaTime;  // Pitch in radians
    Rotation.y += DeltaMousePosition.x * MouseRotationSpeed * DeltaTime;  // Yaw in radians

    Rotation.x = glm::clamp(Rotation.x, -glm::half_pi<float>() + 1e-4f, glm::half_pi<float>() - 1e-4f);
    Rotation.y = std::fmod(Rotation.y, glm::two_pi<float>());

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
