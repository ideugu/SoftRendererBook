#pragma once

namespace CK
{
namespace DDD
{

class GameEngine
{
public:
	GameEngine() = default;

public: // ���� ����

	// ���� �ʱ�ȭ 
	bool Init();
	void OnScreenResize(const ScreenPoint& InScreenSize);
	bool LoadResources();
	bool LoadScene();

	// �Է� ������
	InputManager& GetInputManager() { return _InputManager; }  // �Է� ������ ���

	// ���ҽ� ����
	bool AddMesh(const std::size_t& InKey, const Mesh& InMesh);
	bool AddTexture(const std::size_t& InKey, const Texture& InTexture);

	// ���� ������Ʈ
	const std::vector<std::unique_ptr<GameObject>>& GetScene() const { return _Scene; }
	std::vector< std::unique_ptr<GameObject>>::const_iterator SceneBegin() const { return _Scene.begin(); }
	std::vector< std::unique_ptr<GameObject>>::const_iterator SceneEnd() const { return _Scene.end(); }
	bool AddGameObject(const GameObject& InGameObject);
	GameObject& GetGameObject(const std::string& InName);

	// �޽�
	Mesh& GetMesh(const std::size_t& InMeshKey) { return *_Meshes.at(InMeshKey).get(); }
	const Mesh& GetMesh(const std::size_t& InMeshKey) const { return *_Meshes.at(InMeshKey).get(); }

	// ī�޶� 
	FORCEINLINE Camera& GetMainCamera() { return _MainCamera; }
	FORCEINLINE const Camera& GetMainCamera() const { return _MainCamera; }

	// ���� �ؽ���
	FORCEINLINE const Texture& GetTexture(const std::size_t& InTextureKey) const { return *_Textures.at(InTextureKey).get(); }

public: // �ֿ� Ű ��
	// �޽�
	static const std::size_t HeadMeshKey;
	static const std::size_t BodyMeshKey;
	static const std::size_t LArmMeshKey;
	static const std::size_t RArmMeshKey;
	static const std::size_t LLegMeshKey;
	static const std::size_t RLegMeshKey;

	// ���� ������Ʈ
	static const std::string BodyKey;
	static const std::string HeadKey;
	static const std::string LArmKey;
	static const std::string RArmKey;
	static const std::string LLegKey;
	static const std::string RLegKey;
	static const std::string PlayerKey;
	static const std::string HeadPivotKey;
	static const std::string LArmPivotKey;
	static const std::string RArmPivotKey;
	static const std::string LLegPivotKey;
	static const std::string RLegPivotKey;

	// �ؽ���
	const static std::size_t DiffuseTexture;
	const static std::string SteveTexturePath;

private:
	ScreenPoint _ScreenSize;
	InputManager _InputManager;
	Camera _MainCamera;

	std::vector<std::unique_ptr<GameObject>> _Scene;
	std::unordered_map<std::size_t, std::unique_ptr<Mesh>> _Meshes;
	std::unordered_map<std::size_t, std::unique_ptr<Texture>> _Textures;
};

}
}