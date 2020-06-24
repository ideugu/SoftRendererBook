#pragma once

namespace CK
{
namespace DDD
{

class GameEngine
{
public:
	GameEngine() = default;

public:
	bool Init(const ScreenPoint& InViewportSize);
	bool LoadResources();
	bool LoadScene(const ScreenPoint& InScreenSize);
	InputManager& GetInputManager() { return _InputManager; }
	std::vector<std::unique_ptr<GameObject>>& GetGameObjects() { return _GameObjects; }
	const Mesh* GetMesh(const std::string& InMeshKey) { return _Meshes[InMeshKey].get(); }
	GameObject* GetPlayer() { return _GameObjects[0].get(); }
	Camera* GetCamera() { return _Camera.get(); }

	std::vector<std::unique_ptr<GameObject>>::const_iterator GoBegin() const { return _GameObjects.begin(); }
	std::vector<std::unique_ptr<GameObject>>::const_iterator GoEnd() const { return _GameObjects.end(); }

	const static std::string CubeMeshKey;
	const static std::string PlayerKey;

private:
	ScreenPoint _ViewportSize;
	InputManager _InputManager;
	std::vector<std::unique_ptr<GameObject>> _GameObjects;
	std::unordered_map<std::string, std::unique_ptr<Mesh>> _Meshes;
	std::unique_ptr<Camera> _Camera;
};

}
}