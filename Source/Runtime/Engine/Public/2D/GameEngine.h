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
	bool Init(const ScreenPoint& InViewportSize);
	bool LoadResources();
	bool LoadScene();

	// �Է� ������
	InputManager& GetInputManager() { return _InputManager; }

	// ���� �ؽ���
	Texture& GetMainTexture() { return _MainTexture; }

	// ���� ������Ʈ
	const std::vector<GameObject>& GetScene() { return _Scene; }
	std::vector<GameObject>::const_iterator SceneBegin() const { return _Scene.begin(); }
	std::vector<GameObject>::const_iterator SceneEnd() const { return _Scene.end(); }
	bool InsertGameObject(GameObject& InGameObject);
	GameObject& FindGameObject(const std::string& InName);

	// �޽�
	const Mesh& GetMesh(const std::string& InMeshKey) { return _Meshes[InMeshKey]; }

	// �ֿ� Ű ��
	const static std::string QuadMeshKey;
	const static std::string PlayerKey;
	const static std::string SteveTextureKey;

private:
	ScreenPoint _ViewportSize;
	InputManager _InputManager;
	Texture _MainTexture;

	std::vector<GameObject> _Scene;
	std::unordered_map<std::string, Mesh> _Meshes;
};

}
}