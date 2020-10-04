#pragma once

namespace CK
{
namespace DD
{

class GameEngine
{
public:
	GameEngine() = default;

public:
	// 엔진 초기화 
	bool Init();
	void OnScreenResize(const ScreenPoint& InScreenSize);
	bool LoadResources();
	bool LoadScene();

	// 입력 관리자
	InputManager& GetInputManager() { return _InputManager; }
	FORCEINLINE bool CheckInputSystem();

	// 메인 텍스쳐
	Texture& GetMainTexture() { return _MainTexture; }

	// 게임 오브젝트
	const std::vector<GameObject>& GetScene() { return _Scene; }
	const std::vector<GameObject>::const_iterator SceneBegin() const { return _Scene.begin(); }
	const std::vector<GameObject>::const_iterator SceneEnd() const { return _Scene.end(); }
	bool InsertGameObject(GameObject& InGameObject);
	GameObject& FindGameObject(const std::string& InName);

	// 메시
	const Mesh& GetMesh(const std::string& InMeshKey) { return _Meshes[InMeshKey]; }

	// 카메라 
	Camera& GetMainCamera() { return _MainCamera; }

	// 주요 키 값
	const static std::string QuadMeshKey;
	const static std::string PlayerKey;
	const static std::string SteveTextureKey;

private:
	ScreenPoint _ScreenSize;
	InputManager _InputManager;
	Texture _MainTexture;
	Camera _MainCamera;

	std::vector<GameObject> _Scene;
	std::unordered_map<std::string, Mesh> _Meshes;
};

FORCEINLINE bool GameEngine::CheckInputSystem()
{
	return _InputManager.IsInputSystemReady();
}

}
}