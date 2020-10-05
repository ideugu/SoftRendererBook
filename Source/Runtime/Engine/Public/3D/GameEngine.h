#pragma once

namespace CK
{
namespace DDD
{

class GameEngine
{
public:
	GameEngine() = default;

public: // 공용 로직

	// 엔진 초기화 
	bool Init();
	void OnScreenResize(const ScreenPoint& InScreenSize);
	bool LoadResources();
	bool LoadScene();

	// 입력 관리자
	InputManager& GetInputManager() { return _InputManager; }  // 입력 설정에 사용

	// 리소스 관리
	bool AddMesh(const std::size_t& InKey, const Mesh& InMesh);
	bool AddTexture(const std::size_t& InKey, const Texture& InTexture);

	// 게임 오브젝트
	const std::vector<std::unique_ptr<GameObject>>& GetScene() const { return _Scene; }
	std::vector< std::unique_ptr<GameObject>>::const_iterator SceneBegin() const { return _Scene.begin(); }
	std::vector< std::unique_ptr<GameObject>>::const_iterator SceneEnd() const { return _Scene.end(); }
	GameObject& CreateNewGameObject(const std::string& InName);
	GameObject& GetGameObject(const std::string& InName);

	// 메시
	Mesh& GetMesh(const std::size_t& InMeshKey) { return *_Meshes.at(InMeshKey).get(); }
	const Mesh& GetMesh(const std::size_t& InMeshKey) const { return *_Meshes.at(InMeshKey).get(); }

	// 카메라 
	FORCEINLINE Camera& GetMainCamera() { return _MainCamera; }
	FORCEINLINE const Camera& GetMainCamera() const { return _MainCamera; }

	// 메인 텍스쳐
	FORCEINLINE const Texture& GetTexture(const std::size_t& InTextureKey) const { return *_Textures.at(InTextureKey).get(); }

public: // 주요 키 값
	// 메시
	static const std::size_t CubeMeshKey;

	// 게임 오브젝트
	static const std::string Sun;
	static const std::string Earth;
	static const std::string Moon;
	static const std::string CameraRig;

	// 텍스쳐
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