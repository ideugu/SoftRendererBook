
#include "Precompiled.h"
#include <random>
using namespace CK::DDD;

const std::string GameEngine::CubeMeshKey("SM_Cube");
const std::string GameEngine::GizmoArrowMeshKey("SM_Arrow");
const std::string GameEngine::PlayerKey("Player");
const std::string GameEngine::SteveTextureKey("Steve.png");

void GameEngine::OnScreenResize(const ScreenPoint& InScreenSize)
{
	// ȭ�� ũ���� ����
	_ScreenSize = InScreenSize;
	_MainCamera.SetViewportSize(_ScreenSize);
}

bool GameEngine::Init()
{
	// ȭ�� ũ�Ⱑ �ùٷ� �����Ǿ� �ִ��� Ȯ��
	if (_ScreenSize.HasZero())
	{
		return false;
	}

	if (!_InputManager.GetXAxis || !_InputManager.GetYAxis || !_InputManager.GetZAxis || !_InputManager.GetWAxis || !_InputManager.SpacePressed)
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
	// ť�� �޽�
	Mesh cubeMesh;
	static const float halfSize = 0.5f;

	cubeMesh._Vertices = {
		// Right 
		Vector3(-1.f, -1.f, -1.f)* halfSize, Vector3(-1.f, -1.f, 1.f)* halfSize, Vector3(-1.f, 1.f, 1.f)* halfSize, Vector3(-1.f, 1.f, -1.f)* halfSize,
		// Front
		Vector3(-1.f, -1.f, 1.f)* halfSize, Vector3(-1.f, 1.f, 1.f)* halfSize, Vector3(1.f, 1.f, 1.f)* halfSize, Vector3(1.f, -1.f, 1.f)* halfSize,
		// Back
		Vector3(-1.f, -1.f, -1.f)* halfSize, Vector3(-1.f, 1.f, -1.f)* halfSize, Vector3(1.f, 1.f, -1.f)* halfSize, Vector3(1.f, -1.f, -1.f)* halfSize,
		// Left
		Vector3(1.f, -1.f, -1.f)* halfSize, Vector3(1.f, -1.f, 1.f)* halfSize, Vector3(1.f, 1.f, 1.f)* halfSize, Vector3(1.f, 1.f, -1.f)* halfSize,
		// Top
		Vector3(-1.f, 1.f, -1.f)* halfSize, Vector3(1.f, 1.f, -1.f)* halfSize, Vector3(1.f, 1.f, 1.f)* halfSize, Vector3(-1.f, 1.f, 1.f)* halfSize,
		// Bottom
		Vector3(-1.f, -1.f, -1.f)* halfSize, Vector3(1.f, -1.f, -1.f)* halfSize, Vector3(1.f, -1.f, 1.f)* halfSize, Vector3(-1.f, -1.f, 1.f)* halfSize
	};

	cubeMesh._Indices = {
			0, 1, 2, 0, 2, 3, // Right
			4, 6, 5, 4, 7, 6, // Front
			8, 9, 10, 8, 10, 11, // Back
			12, 14, 13, 12, 15, 14, // Left
			16, 18, 17, 16, 19, 18, // Top
			20, 21, 22, 20, 22, 23  // Bottom
	};

	cubeMesh._UVs = {
		// Right
		Vector2(0.f, 48.f) / 64.f, Vector2(8.f, 48.f) / 64.f, Vector2(8.f, 56.f) / 64.f, Vector2(0.f, 56.f) / 64.f,
		// Front
		Vector2(8.f, 48.f) / 64.f, Vector2(8.f, 56.f) / 64.f, Vector2(16.f, 56.f) / 64.f, Vector2(16.f, 48.f) / 64.f,
		// Back
		Vector2(32.f, 48.f) / 64.f, Vector2(32.f, 56.f) / 64.f, Vector2(24.f, 56.f) / 64.f, Vector2(24.f, 48.f) / 64.f,
		// Left
		Vector2(24.f, 48.f) / 64.f, Vector2(16.f, 48.f) / 64.f, Vector2(16.f, 56.f) / 64.f, Vector2(24.f, 56.f) / 64.f,
		// Top
		Vector2(8.f, 64.f) / 64.f, Vector2(16.f, 64.f) / 64.f, Vector2(16.f, 56.f) / 64.f, Vector2(8.f, 56.f) / 64.f,
		// Bottom
		Vector2(16.f, 64.f) / 64.f, Vector2(24.f, 64.f) / 64.f, Vector2(24.f, 56.f) / 64.f, Vector2(16.f, 56.f) / 64.f
	};

	_Meshes.insert({ GameEngine::CubeMeshKey , std::move(cubeMesh) });

	// ����� ȭ��ǥ
	Mesh gizmoArrow;
	gizmoArrow._Vertices = {
		// Shaft
		Vector3(-1.f, -1.f, 0.f) * halfSize, Vector3(-1.f, -1.f, 5.f) * halfSize, Vector3(-1.f, 1.f, 5.f) * halfSize, Vector3(-1.f, 1.f, 0.f) * halfSize,
		Vector3(-1.f, -1.f, 5.f) * halfSize, Vector3(-1.f, 1.f, 5.f) * halfSize, Vector3(1.f, 1.f, 5.f) * halfSize, Vector3(1.f, -1.f, 5.f) * halfSize,
		Vector3(-1.f, -1.f, 0.f) * halfSize, Vector3(-1.f, 1.f, 0.f) * halfSize, Vector3(1.f, 1.f, 0.f) * halfSize, Vector3(1.f, -1.f, 0.f) * halfSize,
		Vector3(1.f, -1.f, 0.f) * halfSize, Vector3(1.f, -1.f, 5.f) * halfSize, Vector3(1.f, 1.f, 5.f) * halfSize, Vector3(1.f, 1.f, 0.f) * halfSize,
		Vector3(-1.f, 1.f, 0.f) * halfSize, Vector3(1.f, 1.f, 0.f) * halfSize, Vector3(1.f, 1.f, 5.f) * halfSize, Vector3(-1.f, 1.f, 5.f) * halfSize,
		Vector3(-1.f, -1.f, 0.f) * halfSize, Vector3(1.f, -1.f, 0.f) * halfSize, Vector3(1.f, -1.f, 5.f) * halfSize, Vector3(-1.f, -1.f, 5.f) * halfSize,

		// Head
		Vector3(-1.f, -1.f, 2.5f), Vector3(-1.f, 1.f, 2.5f), Vector3(1.f, 1.f, 2.5f), Vector3(1.f, -1.f, 2.5f),
		Vector3(0.f, 0.f, 4.f)
	};

	gizmoArrow._Indices = {
		// Shaft
		0, 1, 2, 0, 2, 3,
		4, 6, 5, 4, 7, 6,
		8, 9, 10, 8, 10, 11,
		12, 14, 13, 12, 15, 14,
		16, 18, 17, 16, 19, 18,
		20, 21, 22, 20, 22, 23,
		// Head
		24, 26, 25, 24, 27, 26,
		24, 28, 27, 24, 25, 28, 25, 26, 28, 26, 27, 28
	};

	_Meshes.insert({ GameEngine::GizmoArrowMeshKey , std::move(gizmoArrow) });

	// �ؽ��� �ε�
	_MainTexture = Texture(SteveTextureKey);
	if (!GetMainTexture().IsIntialized())
	{
		return false;
	}

	return true;
}

bool GameEngine::LoadScene()
{
	// �÷��̾� ����
	static float cubeScale = 100.f;
	GameObject player(GameEngine::PlayerKey);
	player.SetMesh(GameEngine::CubeMeshKey);
	player.GetTransform().SetPosition(Vector3::Zero);
	player.GetTransform().SetScale(Vector3::One * cubeScale);
	player.GetTransform().SetRotation(Rotator(180.f, 0.f, 0.f));
	player.SetColor(LinearColor::Blue);
	InsertGameObject(std::move(player));

	// ����� ȭ��ǥ ����
	static float arrowScale = 5.f;
	GameObject gizmoX("GizmoX");
	gizmoX.SetMesh(GameEngine::GizmoArrowMeshKey);
	gizmoX.GetTransform().SetScale(Vector3::One * arrowScale);
	gizmoX.GetTransform().SetRotation(Rotator(90.f, 0.f, 0.f));
	gizmoX.SetColor(LinearColor::Red);

	GameObject gizmoY("GizmoY");
	gizmoY.SetMesh(GameEngine::GizmoArrowMeshKey);
	gizmoY.GetTransform().SetScale(Vector3::One * arrowScale);
	gizmoY.GetTransform().SetRotation(Rotator(0.f, 0.f, -90.f));
	gizmoY.SetColor(LinearColor::Green);

	GameObject gizmoZ("GizmoZ");
	gizmoZ.SetMesh(GameEngine::GizmoArrowMeshKey);
	gizmoZ.GetTransform().SetScale(Vector3::One * arrowScale);
	gizmoZ.SetColor(LinearColor::Blue);

	InsertGameObject(std::move(gizmoX));
	InsertGameObject(std::move(gizmoY));
	InsertGameObject(std::move(gizmoZ));

	// ī�޶� ����
	_MainCamera.GetTransform().SetPosition(Vector3(300.f, 300.f, -300.f));

	return true;
}

// �����ϸ鼭 �����ϱ�
bool GameEngine::InsertGameObject(GameObject& InGameObject)
{
	auto it = std::lower_bound(_Scene.begin(), _Scene.end(), InGameObject);
	if (it == _Scene.end())
	{
		_Scene.push_back(std::move(InGameObject));
		return true;
	}

	std::size_t inHash = InGameObject.GetHash();
	std::size_t targetHash = (*it).GetHash();

	if (targetHash == inHash)
	{
		// �ߺ��� Ű �߻�. ����.
		return false;
	}
	else if (targetHash < inHash)
	{
		_Scene.insert(it + 1, std::move(InGameObject));
	}
	else
	{
		_Scene.insert(it, std::move(InGameObject));
	}

	return true;
}

GameObject& GameEngine::FindGameObject(const std::string& InName)
{
	auto it = std::lower_bound(_Scene.begin(), _Scene.end(), InName);
	if (it != _Scene.end())
	{
		return (*it);
	}

	return const_cast<GameObject&>(GameObject::NotFound);
}
