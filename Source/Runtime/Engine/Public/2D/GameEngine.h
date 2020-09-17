#pragma once

namespace CK
{
namespace DD
{

struct OrderByHash
{
	inline bool operator () (const std::unique_ptr<GameObject>& _left, const std::string& _right) const
	{
		return _left->GetHash() < std::hash<std::string>()(_right);
	}
};

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
	Texture& GetMainTexture() { return *_MainTexture.get(); }

	// ���� ������Ʈ
	std::vector<std::unique_ptr<DD::GameObject>>& GetScene() { return _Scene; }
	std::vector<std::unique_ptr<DD::GameObject>>::const_iterator SceneBegin() const { return _Scene.begin(); }
	std::vector<std::unique_ptr<DD::GameObject>>::const_iterator SceneEnd() const { return _Scene.end(); }
	bool InsertGameObject(std::unique_ptr<GameObject> InGameObject);
	DD::GameObject& FindGameObject(const std::string& InName);

	// �޽�
	const DD::Mesh& GetMesh(const std::string& InMeshKey) { return *_Meshes[InMeshKey].get(); }

	const static std::string QuadMeshKey;
	const static std::string PlayerKey;
	const static std::string SteveTextureKey;

private:
	ScreenPoint _ViewportSize;
	InputManager _InputManager;

	std::vector<std::unique_ptr<DD::GameObject>> _Scene;
	std::unordered_map<std::string, std::unique_ptr<DD::Mesh>> _Meshes;
	std::unique_ptr<Texture> _MainTexture;
};

}
}