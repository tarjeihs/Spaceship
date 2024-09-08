#include "CameraSubsystem.h"

#include "Core/Camera.h"
#include "Core/Input.h"
#include "Core/KeyCode.h"
#include "Core/Window.h"
#include "Scene/Entity.h"
#include "Scene/Registry.h"
#include "Scene/Scene.h"
#include "Scene/Component.h"
#include "Renderer/Common/Material.h"
#include "Renderer/Common/Mesh.h"

IMesh* Mesh = nullptr;
IMaterial* Material = nullptr;

std::vector<SEntity> Entities;

void PCameraSubsystem::OnAttach()
{
    PCamera* Camera = GetScene()->GetCamera();

    Position = glm::vec3(-2.5f, 0.0f, 0.0f);
    Rotation = glm::vec3(0.0f, 0.0f, 0.0f);

    GetWindow()->OnWindowFocusDelegate.Bind(this, &PCameraSubsystem::OnWindowFocusCallback);

    GetScene()->GetAssetManager()->LoadAsset("/home/tarjeihs/Workspace/Spaceship/Game/Content/Monkey.glb", "Mesh", "MonkeyMesh");
    GetScene()->GetAssetManager()->LoadAsset("/home/tarjeihs/Workspace/Spaceship/Game/Content/Material.mat", "Material", "MonkeyMaterial");
    
    const SAsset* MeshAsset = GetScene()->GetAssetManager()->GetAsset("Mesh", "MonkeyMesh");
    const SAsset* MaterialAsset = GetScene()->GetAssetManager()->GetAsset("Material", "MonkeyMaterial");
    
    Mesh = MeshAsset->GetObject<IMesh>();
    Material = MaterialAsset->GetObject<IMaterial>();

    Mesh->ApplyMaterial(Material);

    for (int32_t Index = 0; Index < 10; ++Index)
    {
        STransform Transform;
        Transform.Translation.x = (Index % 10) * 2.0f;
        Transform.Translation.z = ((float)Index / 10) * 2.0f;

        SEntity Entity = GetScene()->GetRegistry()->CreateEntity();
        Entity.AddComponent<STransformComponent>(Transform);
        Entity.AddComponent<SMeshComponent>(Mesh);

        Entities.push_back(Entity);
    }
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

    //Entity.GetComponent<STransformComponent>().Transform.Rotation.y -= 0.00025f;

    PCamera* Camera = GetScene()->GetCamera();

    constexpr float DeadzoneThreshold = 0.15f;

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

        float axisLeftY = PInput::GetJoystickAxis(JoystickID, RK_GAMEPAD_AXIS_LEFT_Y);
        float axisLeftX = PInput::GetJoystickAxis(JoystickID, RK_GAMEPAD_AXIS_LEFT_X);
        float axisRightY = PInput::GetJoystickAxis(JoystickID, RK_GAMEPAD_AXIS_RIGHT_Y);
        float axisRightX = PInput::GetJoystickAxis(JoystickID, RK_GAMEPAD_AXIS_RIGHT_X);

        axisLeftY = (std::abs(axisLeftY) > DeadzoneThreshold) ? axisLeftY : 0.0f;
        axisLeftX = (std::abs(axisLeftX) > DeadzoneThreshold) ? axisLeftX : 0.0f;
        axisRightY = (std::abs(axisRightY) > DeadzoneThreshold) ? axisRightY : 0.0f;
        axisRightX = (std::abs(axisRightX) > DeadzoneThreshold) ? axisRightX : 0.0f;

        Position -= GetForwardVector() * JoystickMovementSpeed * axisLeftY * DeltaTime;
        Position += GetRightVector() * JoystickMovementSpeed * axisLeftX * DeltaTime;

        Rotation.x -= axisRightY * JoystickRotationSpeed * DeltaTime;  // Pitch in radians
        Rotation.y += axisRightX * JoystickRotationSpeed * DeltaTime;  // Yaw in radians
    }

    Rotation.x = glm::clamp(Rotation.x, -glm::half_pi<float>() + 1e-4f, glm::half_pi<float>() - 1e-4f);
    Rotation.y = std::fmod(Rotation.y, glm::two_pi<float>());

    Camera->CalculateViewMatrix(Position, Rotation);
}

void PCameraSubsystem::OnWindowFocusCallback(uint32_t Focused)
{
    if (Focused)
    {
        GetWindow()->WaitEventOrTimeout(0.1f);
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
