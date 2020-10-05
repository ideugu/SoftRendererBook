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
	GameObject& CreateNewGameObject(const std::string& InName);
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
	static const std::size_t CubeMeshKey;

	// ���� ������Ʈ
	static const std::string Sun;
	static const std::string Earth;
	static const std::string Moon;
	static const std::string CameraRig;

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