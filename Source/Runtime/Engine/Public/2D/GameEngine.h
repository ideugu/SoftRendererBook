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
	// ���� �ʱ�ȭ 
	bool Init();
	void OnScreenResize(const ScreenPoint& InScreenSize);
	bool LoadResources();
	bool LoadScene();

	// �Է� ������
	InputManager& GetInputManager() { return _InputManager; }
	FORCEINLINE bool CheckInputSystem();

	// ���� �ؽ���
	Texture& GetMainTexture() { return _MainTexture; }

	// ���� ������Ʈ
	const std::vector<GameObject>& GetScene() { return _Scene; }
	const std::vector<GameObject>::const_iterator SceneBegin() const { return _Scene.begin(); }
	const std::vector<GameObject>::const_iterator SceneEnd() const { return _Scene.end(); }
	bool InsertGameObject(GameObject& InGameObject);
	GameObject& FindGameObject(const std::string& InName);

	// �޽�
	const Mesh& GetMesh(const std::string& InMeshKey) { return _Meshes[InMeshKey]; }

	// ī�޶� 
	Camera& GetMainCamera() { return _MainCamera; }

	// �ֿ� Ű ��
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