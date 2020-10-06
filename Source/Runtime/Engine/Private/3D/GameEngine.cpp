
#include "Precompiled.h"
#include <random>
using namespace CK::DDD;

// 본 명칭
const std::string GameEngine::RootBone("RootBone");
const std::string GameEngine::PelvisBone("PelvisBone");
const std::string GameEngine::SpineBone("SpineBone");
const std::string GameEngine::LeftArmBone("LeftArmBone");
const std::string GameEngine::RightArmBone("RightArmBone");
const std::string GameEngine::NeckBone("NeckBone");
const std::string GameEngine::LeftLegBone("LeftLegBone");
const std::string GameEngine::RightLegBone("RightLegBone");

// 메시
const std::size_t GameEngine::CharacterMesh = std::hash<std::string>()("SK_Steve");;

// 게임 오브젝트
const std::string GameEngine::PlayerGo("Player");
const std::string GameEngine::CameraRigGo("CameraRig");

// 텍스쳐
const std::size_t GameEngine::DiffuseTexture = std::hash<std::string>()("Diffuse");
const std::string GameEngine::SteveTexturePath("Steve.png");

struct GameObjectCompare
{
	bool operator()(const std::unique_ptr<GameObject>& lhs, std::size_t rhs)
	{
		return lhs->GetHash() < rhs;
	}
};


void GameEngine::OnScreenResize(const ScreenPoint& InScreenSize)
{
	// 화면 크기의 설정
	_ScreenSize = InScreenSize;
	_MainCamera.SetViewportSize(_ScreenSize);
}

bool GameEngine::Init()
{
	// 화면 크기가 올바로 설정되어 있는지 확인
	if (_ScreenSize.HasZero())
	{
		return false;
	}

	if (!_InputManager.IsInputSystemReady())
	{
		return false;
	}

	if (!LoadResources())
	{
		return false;
	}

	if (!LoadScene())
	{
		return false;
	}

	return true;
}

bool GameEngine::LoadResources()
{
	static const Vector3 headSize(0.5f, 0.5f, 0.5f);
	static const Vector3 bodySize(0.5f, 0.75f, 0.25f);
	static const Vector3 armLegSize(0.25f, 0.75f, 0.25f);

	constexpr size_t totalParts = 6;
	Mesh& characterMesh = CreateMesh(GameEngine::CharacterMesh);
	auto& v = characterMesh._Vertices;
	auto& i = characterMesh._Indices;
	auto& uv = characterMesh._UVs;

	// 6개의 파트로 구성되어 있음.
	static std::array<Vector3, totalParts * 4> cubeMeshPositions = {
		Vector3(-1.f, -1.f, -1.f), Vector3(-1.f, -1.f, 1.f), Vector3(-1.f, 1.f, 1.f), Vector3(-1.f, 1.f, -1.f),
		Vector3(-1.f, -1.f, 1.f), Vector3(-1.f, 1.f, 1.f), Vector3(1.f, 1.f, 1.f), Vector3(1.f, -1.f, 1.f),
		Vector3(-1.f, -1.f, -1.f), Vector3(-1.f, 1.f, -1.f), Vector3(1.f, 1.f, -1.f), Vector3(1.f, -1.f, -1.f),
		Vector3(1.f, -1.f, -1.f), Vector3(1.f, -1.f, 1.f), Vector3(1.f, 1.f, 1.f), Vector3(1.f, 1.f, -1.f),
		Vector3(-1.f, 1.f, -1.f), Vector3(1.f, 1.f, -1.f), Vector3(1.f, 1.f, 1.f), Vector3(-1.f, 1.f, 1.f),
		Vector3(-1.f, -1.f, -1.f), Vector3(1.f, -1.f, -1.f), Vector3(1.f, -1.f, 1.f), Vector3(-1.f, -1.f, 1.f)
	};

	static std::array<size_t, totalParts * 6> cubeMeshIndice = {
		0, 1, 2, 0, 2, 3, // Right
		4, 6, 5, 4, 7, 6, // Front
		8, 9, 10, 8, 10, 11, // Back
		12, 14, 13, 12, 15, 14, // Left
		16, 18, 17, 16, 19, 18, // Top
		20, 21, 22, 20, 22, 23  // Bottom
	};

	static std::array<Vector3, totalParts> cubeMeshSize = {
		headSize, bodySize, armLegSize, armLegSize, armLegSize, armLegSize
	};

	static std::array<Vector3, totalParts> cubeMeshOffset = {
		Vector3(0.f, 3.5f, 0.f), Vector3(0.f, 2.25f, 0.f), Vector3(-0.75f, 2.25f, 0.f), Vector3(0.75f, 2.25f, 0.f), Vector3(-0.25f, 0.75f, 0.f), Vector3(0.25f, 0.75f, 0.f)
	};

	for (size_t part = 0; part < totalParts; part++)
	{
		std::transform(cubeMeshPositions.begin(), cubeMeshPositions.end(), std::back_inserter(v), [&](auto& p) { return p * cubeMeshSize[part] + cubeMeshOffset[part]; });
		std::transform(cubeMeshIndice.begin(), cubeMeshIndice.end(), std::back_inserter(i), [&](auto& p) { return p + 24 * part; });
	}

	uv = {
		// HeadRight
		Vector2(0.f, 48.f) / 64.f, Vector2(8.f, 48.f) / 64.f, Vector2(8.f, 56.f) / 64.f, Vector2(0.f, 56.f) / 64.f,
		// HeadFront
		Vector2(8.f, 48.f) / 64.f, Vector2(8.f, 56.f) / 64.f, Vector2(16.f, 56.f) / 64.f, Vector2(16.f, 48.f) / 64.f,
		// HeadBack
		Vector2(32.f, 48.f) / 64.f, Vector2(32.f, 56.f) / 64.f, Vector2(24.f, 56.f) / 64.f, Vector2(24.f, 48.f) / 64.f,
		// HeadLeft
		Vector2(24.f, 48.f) / 64.f, Vector2(16.f, 48.f) / 64.f, Vector2(16.f, 56.f) / 64.f, Vector2(24.f, 56.f) / 64.f,
		// HeadTop
		Vector2(8.f, 64.f) / 64.f, Vector2(16.f, 64.f) / 64.f, Vector2(16.f, 56.f) / 64.f, Vector2(8.f, 56.f) / 64.f,
		// HeadBottom
		Vector2(16.f, 64.f) / 64.f, Vector2(24.f, 64.f) / 64.f, Vector2(24.f, 56.f) / 64.f, Vector2(16.f, 56.f) / 64.f,
		// BodyRight
		Vector2(16.f, 32.f) / 64.f, Vector2(20.f, 32.f) / 64.f, Vector2(20.f, 44.f) / 64.f, Vector2(16.f, 44.f) / 64.f,
		// BodyFront
		Vector2(20.f, 32.f) / 64.f, Vector2(20.f, 44.f) / 64.f, Vector2(28.f, 44.f) / 64.f, Vector2(28.f, 32.f) / 64.f,
		// BodyBack
		Vector2(36.f, 32.f) / 64.f, Vector2(36.f, 44.f) / 64.f, Vector2(28.f, 44.f) / 64.f, Vector2(28.f, 32.f) / 64.f,
		// BodyLeft
		Vector2(40.f, 32.f) / 64.f, Vector2(36.f, 32.f) / 64.f, Vector2(36.f, 44.f) / 64.f, Vector2(40.f, 44.f) / 64.f,
		// BodyTop
		Vector2(20.f, 48.f) / 64.f, Vector2(28.f, 48.f) / 64.f, Vector2(28.f, 44.f) / 64.f, Vector2(20.f, 44.f) / 64.f,
		// BodyBottom
		Vector2(28.f, 48.f) / 64.f, Vector2(36.f, 48.f) / 64.f, Vector2(36.f, 44.f) / 64.f, Vector2(28.f, 44.f) / 64.f,
		// LeftArmRight
		Vector2(32.f, 0.f) / 64.f, Vector2(36.f, 0.f) / 64.f, Vector2(36.f, 12.f) / 64.f, Vector2(32.f, 12.f) / 64.f,
		// LeftArmFront
		Vector2(36.f, 0.f) / 64.f, Vector2(36.f, 12.f) / 64.f, Vector2(40.f, 12.f) / 64.f, Vector2(40.f, 0.f) / 64.f,
		// LeftArmBack
		Vector2(44.f, 0.f) / 64.f, Vector2(44.f, 12.f) / 64.f, Vector2(40.f, 12.f) / 64.f, Vector2(40.f, 0.f) / 64.f,
		// LeftArmLeft
		Vector2(48.f, 0.f) / 64.f, Vector2(44.f, 0.f) / 64.f, Vector2(44.f, 12.f) / 64.f, Vector2(48.f, 12.f) / 64.f,
		// LeftArmTop
		Vector2(36.f, 16.f) / 64.f, Vector2(40.f, 16.f) / 64.f, Vector2(40.f, 12.f) / 64.f, Vector2(36.f, 12.f) / 64.f,
		// LeftArmBottom
		Vector2(40.f, 16.f) / 64.f, Vector2(44.f, 16.f) / 64.f, Vector2(44.f, 12.f) / 64.f, Vector2(40.f, 12.f) / 64.f,
		// RightArmRight
		Vector2(40.f, 32.f) / 64.f, Vector2(44.f, 32.f) / 64.f, Vector2(44.f, 44.f) / 64.f, Vector2(40.f, 44.f) / 64.f,
		// RightArmFront
		Vector2(44.f, 32.f) / 64.f, Vector2(44.f, 44.f) / 64.f, Vector2(48.f, 44.f) / 64.f, Vector2(48.f, 32.f) / 64.f,
		// RightArmBack
		Vector2(52.f, 32.f) / 64.f, Vector2(52.f, 44.f) / 64.f, Vector2(48.f, 44.f) / 64.f, Vector2(48.f, 32.f) / 64.f,
		// RightArmLeft
		Vector2(56.f, 32.f) / 64.f, Vector2(52.f, 32.f) / 64.f, Vector2(52.f, 44.f) / 64.f, Vector2(56.f, 44.f) / 64.f,
		// RightArmTop
		Vector2(44.f, 48.f) / 64.f, Vector2(48.f, 48.f) / 64.f, Vector2(48.f, 44.f) / 64.f, Vector2(44.f, 44.f) / 64.f,
		// RightArmBottom
		Vector2(48.f, 48.f) / 64.f, Vector2(52.f, 48.f) / 64.f, Vector2(52.f, 44.f) / 64.f, Vector2(48.f, 44.f) / 64.f,
		// LeftLegRight
		Vector2(16.f, 0.f) / 64.f, Vector2(20.f, 0.f) / 64.f, Vector2(20.f, 12.f) / 64.f, Vector2(16.f, 12.f) / 64.f,
		// LeftLegFront
		Vector2(20.f, 0.f) / 64.f, Vector2(20.f, 12.f) / 64.f, Vector2(24.f, 12.f) / 64.f, Vector2(24.f, 0.f) / 64.f,
		// LeftLegBack
		Vector2(28.f, 0.f) / 64.f, Vector2(28.f, 12.f) / 64.f, Vector2(24.f, 12.f) / 64.f, Vector2(24.f, 0.f) / 64.f,
		// LeftLegLeft
		Vector2(32.f, 0.f) / 64.f, Vector2(28.f, 0.f) / 64.f, Vector2(28.f, 12.f) / 64.f, Vector2(32.f, 12.f) / 64.f,
		// LeftLegTop
		Vector2(20.f, 16.f) / 64.f, Vector2(24.f, 16.f) / 64.f, Vector2(24.f, 12.f) / 64.f, Vector2(20.f, 12.f) / 64.f,
		// LeftLegBottom
		Vector2(24.f, 16.f) / 64.f, Vector2(28.f, 16.f) / 64.f, Vector2(28.f, 12.f) / 64.f, Vector2(24.f, 12.f) / 64.f,
		// RightLegRight
		Vector2(0.f, 32.f) / 64.f, Vector2(4.f, 32.f) / 64.f, Vector2(4.f, 44.f) / 64.f, Vector2(0.f, 44.f) / 64.f,
		// RightLegFront
		Vector2(4.f, 32.f) / 64.f, Vector2(4.f, 44.f) / 64.f, Vector2(8.f, 44.f) / 64.f, Vector2(8.f, 32.f) / 64.f,
		// RightLegBack
		Vector2(12.f, 32.f) / 64.f, Vector2(12.f, 44.f) / 64.f, Vector2(8.f, 44.f) / 64.f, Vector2(8.f, 32.f) / 64.f,
		// RightLegLeft
		Vector2(16.f, 32.f) / 64.f, Vector2(12.f, 32.f) / 64.f, Vector2(12.f, 44.f) / 64.f, Vector2(16.f, 44.f) / 64.f,
		// RightLegTop
		Vector2(4.f, 48.f) / 64.f, Vector2(8.f, 48.f) / 64.f, Vector2(8.f, 44.f) / 64.f, Vector2(4.f, 44.f) / 64.f,
		// RightLegBottom
		Vector2(8.f, 48.f) / 64.f, Vector2(12.f, 48.f) / 64.f, Vector2(12.f, 44.f) / 64.f, Vector2(8.f, 44.f) / 64.f
	};

	// 캐릭터 Rig 설정
	characterMesh.SetMeshType(MeshType::Skinned);
	auto& b = characterMesh._ConnectedBones;
	auto& w = characterMesh._Weights;

	// 본 생성
	characterMesh._Bones = {
		{ GameEngine::RootBone, Bone(GameEngine::RootBone, TransformData()) },
		{ GameEngine::PelvisBone, Bone(GameEngine::PelvisBone, TransformData(Vector3(0.f, 1.5f, 0.f))) },
		{ GameEngine::SpineBone, Bone(GameEngine::SpineBone, TransformData(Vector3(0.f, 2.25f, 0.f))) },
		{ GameEngine::LeftArmBone, Bone(GameEngine::LeftArmBone, TransformData(Vector3(-0.75f, 3.f, 0.f))) },
		{ GameEngine::RightArmBone, Bone(GameEngine::RightArmBone, TransformData(Vector3(0.75f, 3.f, 0.f))) },
		{ GameEngine::LeftLegBone, Bone(GameEngine::LeftLegBone, TransformData(Vector3(0.25f, 1.5f, 0.f))) },
		{ GameEngine::RightLegBone, Bone(GameEngine::RightLegBone, TransformData(Vector3(-0.25f, 1.5f, 0.f))) },
		{ GameEngine::NeckBone, Bone(GameEngine::NeckBone, TransformData(Vector3(0.f, 3.f, 0.f))) }
	};

	// 계층 구조 생성
	Bone& root = characterMesh.GetBone(GameEngine::RootBone);
	Bone& pelvis = characterMesh.GetBone(GameEngine::PelvisBone);
	pelvis.SetParent(root);
	Bone& spine = characterMesh.GetBone(GameEngine::SpineBone);
	spine.SetParent(pelvis);
	Bone& leftArm = characterMesh.GetBone(GameEngine::LeftArmBone);
	leftArm.SetParent(spine);
	Bone& rightArm = characterMesh.GetBone(GameEngine::RightArmBone);
	rightArm.SetParent(spine);
	Bone& leftLeg = characterMesh.GetBone(GameEngine::LeftLegBone);
	leftLeg.SetParent(pelvis);
	Bone& rightLeg = characterMesh.GetBone(GameEngine::RightLegBone);
	rightLeg.SetParent(pelvis);
	Bone& neck = characterMesh.GetBone(GameEngine::NeckBone);
	neck.SetParent(spine);

	// 메시에 리깅 
	static std::array<std::string, 6> boneOrder = {
		GameEngine::NeckBone, GameEngine::SpineBone, GameEngine::LeftArmBone, GameEngine::RightArmBone, GameEngine::LeftLegBone, GameEngine::RightLegBone
	};

	b.resize(v.size());
	w.resize(v.size());
	std::fill(b.begin(), b.end(), 1);

	for (size_t part = 0; part < 6; part++)
	{
		Weight weight;
		weight.Bones = { boneOrder[part] };
		weight.Values = { 1.f };
		auto startIt = w.begin() + part * 24;
		std::fill(startIt, startIt + 24, weight);
	}

	characterMesh.CalculateBounds();

	// 텍스쳐 로딩
	Texture& diffuseTexture = CreateTexture(GameEngine::DiffuseTexture, GameEngine::SteveTexturePath);
	if(!diffuseTexture.IsIntialized())
	{
		return false;
	}

	return true;
}

bool GameEngine::LoadScene()
{
	// 플레이어
	static const float playerScale = 100.f;

	GameObject& goPlayer = CreateNewGameObject(GameEngine::PlayerGo);
	goPlayer.SetMesh(GameEngine::CharacterMesh);
	goPlayer.GetTransform().SetWorldScale(Vector3::One * playerScale);
	goPlayer.GetTransform().SetWorldRotation(Rotator(180.f, 0.f, 0.f));

	// 카메라 릭
	GameObject& goCameraRig = CreateNewGameObject(GameEngine::CameraRigGo);
	goCameraRig.GetTransform().SetWorldPosition(Vector3(0.f, 150.f, 0.f));

	// 카메라 설정
	_MainCamera.GetTransform().SetWorldPosition(Vector3(500.f, 800.f, -1000.f));
	_MainCamera.SetParent(goCameraRig);
	_MainCamera.SetLookAtRotation(goCameraRig);
	auto q1 = _MainCamera.GetTransform().GetWorldRotation();

	return true;
}

Mesh& GameEngine::CreateMesh(const std::size_t& InKey)
{
	auto meshPtr = std::make_unique<Mesh>();
	_Meshes.insert({ InKey, std::move(meshPtr) });
	return *_Meshes.at(InKey).get();
}

Texture& GameEngine::CreateTexture(const std::size_t& InKey, const std::string& InTexturePath)
{
	auto texturePtr = std::make_unique<Texture>(InTexturePath);
	_Textures.insert({ InKey, std::move(texturePtr) });
	return *_Textures.at(InKey).get();
}

GameObject& GameEngine::CreateNewGameObject(const std::string& InName)
{
	std::size_t inHash = std::hash<std::string>()(InName);
	const auto it = std::lower_bound(SceneBegin(), SceneEnd(), inHash, GameObjectCompare());

	auto newGameObject = std::make_unique<GameObject>(InName);
	if (it != _Scene.end())
	{
		std::size_t targetHash = (*it)->GetHash();
		if (targetHash == inHash)
		{
			// 중복된 키 발생. 무시.
			return GameObject::Invalid;
		}
		else if (targetHash < inHash)
		{
			_Scene.insert(it + 1, std::move(newGameObject));
		}
		else
		{
			_Scene.insert(it, std::move(newGameObject));
		}
	}
	else
	{
		_Scene.push_back(std::move(newGameObject));
	}

	return GetGameObject(InName);
}

GameObject& GameEngine::GetGameObject(const std::string& InName)
{
	std::size_t targetHash = std::hash<std::string>()(InName);
	const auto it = std::lower_bound(SceneBegin(), SceneEnd(), targetHash, GameObjectCompare());

	if (it != _Scene.end())
	{
		return *(*it).get();
	}

	return GameObject::Invalid;
}
