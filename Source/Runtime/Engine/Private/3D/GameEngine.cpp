
#include "Precompiled.h"
#include <random>
using namespace CK::DDD;

const std::size_t GameEngine::HeadMeshKey = std::hash<std::string>()("SM_Head");
const std::size_t GameEngine::BodyMeshKey = std::hash<std::string>()("SM_Body");
const std::size_t GameEngine::LArmMeshKey = std::hash<std::string>()("SM_LeftHand");
const std::size_t GameEngine::RArmMeshKey = std::hash<std::string>()("SM_RightHand");
const std::size_t GameEngine::LLegMeshKey = std::hash<std::string>()("SM_LeftLeg");
const std::size_t GameEngine::RLegMeshKey = std::hash<std::string>()("SM_RightLeg");

const std::string GameEngine::PlayerKey("Player");
const std::string GameEngine::BodyKey("Body");
const std::string GameEngine::HeadKey("Head");
const std::string GameEngine::LArmKey("LHand");
const std::string GameEngine::RArmKey("RHand");
const std::string GameEngine::LLegKey("LLeg");
const std::string GameEngine::RLegKey("RLeg");
const std::string GameEngine::HeadPivotKey("HeadPivot");
const std::string GameEngine::LArmPivotKey("LHandPivot");
const std::string GameEngine::RArmPivotKey("RHandPivot");
const std::string GameEngine::LLegPivotKey("LLegPivot");
const std::string GameEngine::RLegPivotKey("RLegPivot");

const std::size_t GameEngine::DiffuseTexture = std::hash<std::string>()("Diffuse");
const std::string GameEngine::SteveTexturePath("Steve.png");

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
	static const float cubeHalfSize = 0.5f;
	static const Vector3 bodySize(0.5f, 0.75f, 0.25f);
	static const Vector3 armSize(0.25f, 0.75f, 0.25f);

	// 각 메시 파트의 정보
	static const int vertexCount = 24;
	static const int triangleCount = 12;
	static const int indexCount = triangleCount * 3;

	// 메시 원칙 1.  -X, -Y, -Z에서부터 +X, +Y, +Z로
	// 메시 원칙 2.  X와 Y중 X우선,  Y와 Z중 Y우선, X와 Z 중 Z우선
	// 예시) Left, Right는 X고정 따라서 (-Y, -Z) (-Y, Z) (Y, Z) (Y, -Z) 의 순서로 진행
	// 예시) Front, Back은 Z고정 따라서 (-X, -Y) (-X, Y) (X, Y) (X, -Y) 의 순서로 진행
	// 예시) Top, Bottom은 Y고정 따라서 (-X, -Z) (X, -Z) (X, Z) (-X, Z) 의 순서로 진행

	Mesh headMesh, bodyMesh, leftArmMesh, rightArmMesh, leftLegMesh, rightLegMesh;
	headMesh._Vertices = {
		// Right 
		Vector3(-1.f, -1.f, -1.f) * cubeHalfSize, Vector3(-1.f, -1.f, 1.f) * cubeHalfSize, Vector3(-1.f, 1.f, 1.f) * cubeHalfSize, Vector3(-1.f, 1.f, -1.f) * cubeHalfSize,
		// Front
		Vector3(-1.f, -1.f, 1.f) * cubeHalfSize, Vector3(-1.f, 1.f, 1.f) * cubeHalfSize, Vector3(1.f, 1.f, 1.f) * cubeHalfSize, Vector3(1.f, -1.f, 1.f) * cubeHalfSize,
		// Back
		Vector3(-1.f, -1.f, -1.f) * cubeHalfSize, Vector3(-1.f, 1.f, -1.f) * cubeHalfSize, Vector3(1.f, 1.f, -1.f) * cubeHalfSize, Vector3(1.f, -1.f, -1.f) * cubeHalfSize,
		// Left
		Vector3(1.f, -1.f, -1.f) * cubeHalfSize, Vector3(1.f, -1.f, 1.f) * cubeHalfSize, Vector3(1.f, 1.f, 1.f) * cubeHalfSize, Vector3(1.f, 1.f, -1.f) * cubeHalfSize,
		// Top
		Vector3(-1.f, 1.f, -1.f) * cubeHalfSize, Vector3(1.f, 1.f, -1.f) * cubeHalfSize, Vector3(1.f, 1.f, 1.f) * cubeHalfSize, Vector3(-1.f, 1.f, 1.f) * cubeHalfSize,
		// Bottom
		Vector3(-1.f, -1.f, -1.f) * cubeHalfSize, Vector3(1.f, -1.f, -1.f) * cubeHalfSize, Vector3(1.f, -1.f, 1.f) * cubeHalfSize, Vector3(-1.f, -1.f, 1.f) * cubeHalfSize
	};
	headMesh._Indices = {
		0, 1, 2, 0, 2, 3, // Right
		4, 6, 5, 4, 7, 6, // Front
		8, 9, 10, 8, 10, 11, // Back
		12, 14, 13, 12, 15, 14, // Left
		16, 18, 17, 16, 19, 18, // Top
		20, 21, 22, 20, 22, 23  // Bottom
	};
	headMesh._UVs = {
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

	bodyMesh._Vertices = {
		// Right 
		Vector3(-1.f, -1.f, -1.f) * bodySize, Vector3(-1.f, -1.f, 1.f) * bodySize, Vector3(-1.f, 1.f, 1.f) * bodySize, Vector3(-1.f, 1.f, -1.f) * bodySize,
		// Front
		Vector3(-1.f, -1.f, 1.f) * bodySize, Vector3(-1.f, 1.f, 1.f) * bodySize, Vector3(1.f, 1.f, 1.f) * bodySize, Vector3(1.f, -1.f, 1.f) * bodySize,
		// Back
		Vector3(-1.f, -1.f, -1.f) * bodySize, Vector3(-1.f, 1.f, -1.f) * bodySize, Vector3(1.f, 1.f, -1.f) * bodySize, Vector3(1.f, -1.f, -1.f) * bodySize,
		// Left
		Vector3(1.f, -1.f, -1.f) * bodySize, Vector3(1.f, -1.f, 1.f) * bodySize, Vector3(1.f, 1.f, 1.f) * bodySize, Vector3(1.f, 1.f, -1.f) * bodySize,
		// Top
		Vector3(-1.f, 1.f, -1.f) * bodySize, Vector3(1.f, 1.f, -1.f) * bodySize, Vector3(1.f, 1.f, 1.f) * bodySize, Vector3(-1.f, 1.f, 1.f) * bodySize,
		// Bottom
		Vector3(-1.f, -1.f, -1.f) * bodySize, Vector3(1.f, -1.f, -1.f) * bodySize, Vector3(1.f, -1.f, 1.f) * bodySize, Vector3(-1.f, -1.f, 1.f) * bodySize
	};
	bodyMesh._Indices = {
		0, 1, 2, 0, 2, 3, // Right
		4, 6, 5, 4, 7, 6, // Front
		8, 9, 10, 8, 10, 11, // Back
		12, 14, 13, 12, 15, 14, // Left
		16, 18, 17, 16, 19, 18, // Top
		20, 21, 22, 20, 22, 23  // Bottom
	};
	bodyMesh._UVs = {
		// Right
		Vector2(16.f, 32.f) / 64.f, Vector2(20.f, 32.f) / 64.f, Vector2(20.f, 44.f) / 64.f, Vector2(16.f, 44.f) / 64.f,
		// Front
		Vector2(20.f, 32.f) / 64.f, Vector2(20.f, 44.f) / 64.f, Vector2(28.f, 44.f) / 64.f, Vector2(28.f, 32.f) / 64.f,
		// Back
		Vector2(36.f, 32.f) / 64.f, Vector2(36.f, 44.f) / 64.f, Vector2(28.f, 44.f) / 64.f, Vector2(28.f, 32.f) / 64.f,
		// Left
		Vector2(40.f, 32.f) / 64.f, Vector2(36.f, 32.f) / 64.f, Vector2(36.f, 44.f) / 64.f, Vector2(40.f, 44.f) / 64.f,
		// Top
		Vector2(20.f, 48.f) / 64.f, Vector2(28.f, 48.f) / 64.f, Vector2(28.f, 44.f) / 64.f, Vector2(20.f, 44.f) / 64.f,
		// Bottom
		Vector2(28.f, 48.f) / 64.f, Vector2(36.f, 48.f) / 64.f, Vector2(36.f, 44.f) / 64.f, Vector2(28.f, 44.f) / 64.f
	};

	leftArmMesh._Vertices = {
		// Right 
		Vector3(-1.f, -1.f, -1.f) * armSize, Vector3(-1.f, -1.f, 1.f) * armSize, Vector3(-1.f, 1.f, 1.f) * armSize, Vector3(-1.f, 1.f, -1.f) * armSize,
		// Front
		Vector3(-1.f, -1.f, 1.f) * armSize, Vector3(-1.f, 1.f, 1.f) * armSize, Vector3(1.f, 1.f, 1.f) * armSize, Vector3(1.f, -1.f, 1.f) * armSize,
		// Back
		Vector3(-1.f, -1.f, -1.f) * armSize, Vector3(-1.f, 1.f, -1.f) * armSize, Vector3(1.f, 1.f, -1.f) * armSize, Vector3(1.f, -1.f, -1.f) * armSize,
		// Left
		Vector3(1.f, -1.f, -1.f) * armSize, Vector3(1.f, -1.f, 1.f) * armSize, Vector3(1.f, 1.f, 1.f) * armSize, Vector3(1.f, 1.f, -1.f) * armSize,
		// Top
		Vector3(-1.f, 1.f, -1.f) * armSize, Vector3(1.f, 1.f, -1.f) * armSize, Vector3(1.f, 1.f, 1.f) * armSize, Vector3(-1.f, 1.f, 1.f) * armSize,
		// Bottom
		Vector3(-1.f, -1.f, -1.f) * armSize, Vector3(1.f, -1.f, -1.f) * armSize, Vector3(1.f, -1.f, 1.f) * armSize, Vector3(-1.f, -1.f, 1.f) * armSize
	};
	leftArmMesh._Indices = {
		0, 1, 2, 0, 2, 3, // Right
		4, 6, 5, 4, 7, 6, // Front
		8, 9, 10, 8, 10, 11, // Back
		12, 14, 13, 12, 15, 14, // Left
		16, 18, 17, 16, 19, 18, // Top
		20, 21, 22, 20, 22, 23  // Bottom
	};
	leftArmMesh._UVs = {
		// Right
		Vector2(32.f, 0.f) / 64.f, Vector2(36.f, 0.f) / 64.f, Vector2(36.f, 12.f) / 64.f, Vector2(32.f, 12.f) / 64.f,
		// Front
		Vector2(36.f, 0.f) / 64.f, Vector2(36.f, 12.f) / 64.f, Vector2(40.f, 12.f) / 64.f, Vector2(40.f, 0.f) / 64.f,
		// Back
		Vector2(44.f, 0.f) / 64.f, Vector2(44.f, 12.f) / 64.f, Vector2(40.f, 12.f) / 64.f, Vector2(40.f, 0.f) / 64.f,
		// Left
		Vector2(48.f, 0.f) / 64.f, Vector2(44.f, 0.f) / 64.f, Vector2(44.f, 12.f) / 64.f, Vector2(48.f, 12.f) / 64.f,
		// Top
		Vector2(36.f, 16.f) / 64.f, Vector2(40.f, 16.f) / 64.f, Vector2(40.f, 12.f) / 64.f, Vector2(36.f, 12.f) / 64.f,
		// Bottom
		Vector2(40.f, 16.f) / 64.f, Vector2(44.f, 16.f) / 64.f, Vector2(44.f, 12.f) / 64.f, Vector2(40.f, 12.f) / 64.f
	};

	rightArmMesh._Vertices = {
		// Right 
		Vector3(-1.f, -1.f, -1.f) * armSize, Vector3(-1.f, -1.f, 1.f) * armSize, Vector3(-1.f, 1.f, 1.f) * armSize, Vector3(-1.f, 1.f, -1.f) * armSize,
		// Front
		Vector3(-1.f, -1.f, 1.f) * armSize, Vector3(-1.f, 1.f, 1.f) * armSize, Vector3(1.f, 1.f, 1.f) * armSize, Vector3(1.f, -1.f, 1.f) * armSize,
		// Back
		Vector3(-1.f, -1.f, -1.f) * armSize, Vector3(-1.f, 1.f, -1.f) * armSize, Vector3(1.f, 1.f, -1.f) * armSize, Vector3(1.f, -1.f, -1.f) * armSize,
		// Left
		Vector3(1.f, -1.f, -1.f) * armSize, Vector3(1.f, -1.f, 1.f) * armSize, Vector3(1.f, 1.f, 1.f) * armSize, Vector3(1.f, 1.f, -1.f) * armSize,
		// Top
		Vector3(-1.f, 1.f, -1.f) * armSize, Vector3(1.f, 1.f, -1.f) * armSize, Vector3(1.f, 1.f, 1.f) * armSize, Vector3(-1.f, 1.f, 1.f) * armSize,
		// Bottom
		Vector3(-1.f, -1.f, -1.f) * armSize, Vector3(1.f, -1.f, -1.f) * armSize, Vector3(1.f, -1.f, 1.f) * armSize, Vector3(-1.f, -1.f, 1.f) * armSize
	};
	rightArmMesh._Indices = {
		0, 1, 2, 0, 2, 3, // Right
		4, 6, 5, 4, 7, 6, // Front
		8, 9, 10, 8, 10, 11, // Back
		12, 14, 13, 12, 15, 14, // Left
		16, 18, 17, 16, 19, 18, // Top
		20, 21, 22, 20, 22, 23  // Bottom
	};
	rightArmMesh._UVs = {
		// Right
		Vector2(40.f, 32.f) / 64.f, Vector2(44.f, 32.f) / 64.f, Vector2(44.f, 44.f) / 64.f, Vector2(40.f, 44.f) / 64.f,
		// Front
		Vector2(44.f, 32.f) / 64.f, Vector2(44.f, 44.f) / 64.f, Vector2(48.f, 44.f) / 64.f, Vector2(48.f, 32.f) / 64.f,
		// Back
		Vector2(52.f, 32.f) / 64.f, Vector2(52.f, 44.f) / 64.f, Vector2(48.f, 44.f) / 64.f, Vector2(48.f, 32.f) / 64.f,
		// Left
		Vector2(56.f, 32.f) / 64.f, Vector2(52.f, 32.f) / 64.f, Vector2(52.f, 44.f) / 64.f, Vector2(56.f, 44.f) / 64.f,
		// Top
		Vector2(44.f, 48.f) / 64.f, Vector2(48.f, 48.f) / 64.f, Vector2(48.f, 44.f) / 64.f, Vector2(44.f, 44.f) / 64.f,
		// Bottom
		Vector2(48.f, 48.f) / 64.f, Vector2(52.f, 48.f) / 64.f, Vector2(52.f, 44.f) / 64.f, Vector2(48.f, 44.f) / 64.f
	};

	leftLegMesh._Vertices = {
		// Right 
		Vector3(-1.f, -1.f, -1.f) * armSize, Vector3(-1.f, -1.f, 1.f) * armSize, Vector3(-1.f, 1.f, 1.f) * armSize, Vector3(-1.f, 1.f, -1.f) * armSize,
		// Front
		Vector3(-1.f, -1.f, 1.f) * armSize, Vector3(-1.f, 1.f, 1.f) * armSize, Vector3(1.f, 1.f, 1.f) * armSize, Vector3(1.f, -1.f, 1.f) * armSize,
		// Back
		Vector3(-1.f, -1.f, -1.f) * armSize, Vector3(-1.f, 1.f, -1.f) * armSize, Vector3(1.f, 1.f, -1.f) * armSize, Vector3(1.f, -1.f, -1.f) * armSize,
		// Left
		Vector3(1.f, -1.f, -1.f) * armSize, Vector3(1.f, -1.f, 1.f) * armSize, Vector3(1.f, 1.f, 1.f) * armSize, Vector3(1.f, 1.f, -1.f) * armSize,
		// Top
		Vector3(-1.f, 1.f, -1.f) * armSize, Vector3(1.f, 1.f, -1.f) * armSize, Vector3(1.f, 1.f, 1.f) * armSize, Vector3(-1.f, 1.f, 1.f) * armSize,
		// Bottom
		Vector3(-1.f, -1.f, -1.f) * armSize, Vector3(1.f, -1.f, -1.f) * armSize, Vector3(1.f, -1.f, 1.f) * armSize, Vector3(-1.f, -1.f, 1.f) * armSize
	};
	leftLegMesh._Indices = {
		0, 1, 2, 0, 2, 3, // Right
		4, 6, 5, 4, 7, 6, // Front
		8, 9, 10, 8, 10, 11, // Back
		12, 14, 13, 12, 15, 14, // Left
		16, 18, 17, 16, 19, 18, // Top
		20, 21, 22, 20, 22, 23  // Bottom
	};
	leftLegMesh._UVs = {
		// Right
		Vector2(16.f, 0.f) / 64.f, Vector2(20.f, 0.f) / 64.f, Vector2(20.f, 12.f) / 64.f, Vector2(16.f, 12.f) / 64.f,
		// Front
		Vector2(20.f, 0.f) / 64.f, Vector2(20.f, 12.f) / 64.f, Vector2(24.f, 12.f) / 64.f, Vector2(24.f, 0.f) / 64.f,
		// Back
		Vector2(28.f, 0.f) / 64.f, Vector2(28.f, 12.f) / 64.f, Vector2(24.f, 12.f) / 64.f, Vector2(24.f, 0.f) / 64.f,
		// Left
		Vector2(32.f, 0.f) / 64.f, Vector2(28.f, 0.f) / 64.f, Vector2(28.f, 12.f) / 64.f, Vector2(32.f, 12.f) / 64.f,
		// Top
		Vector2(20.f, 16.f) / 64.f, Vector2(24.f, 16.f) / 64.f, Vector2(24.f, 12.f) / 64.f, Vector2(20.f, 12.f) / 64.f,
		// Bottom
		Vector2(24.f, 16.f) / 64.f, Vector2(28.f, 16.f) / 64.f, Vector2(28.f, 12.f) / 64.f, Vector2(24.f, 12.f) / 64.f
	};

	rightLegMesh._Vertices = {
		// Right 
		Vector3(-1.f, -1.f, -1.f) * armSize, Vector3(-1.f, -1.f, 1.f) * armSize, Vector3(-1.f, 1.f, 1.f) * armSize, Vector3(-1.f, 1.f, -1.f) * armSize,
		// Front
		Vector3(-1.f, -1.f, 1.f) * armSize, Vector3(-1.f, 1.f, 1.f) * armSize, Vector3(1.f, 1.f, 1.f) * armSize, Vector3(1.f, -1.f, 1.f) * armSize,
		// Back
		Vector3(-1.f, -1.f, -1.f) * armSize, Vector3(-1.f, 1.f, -1.f) * armSize, Vector3(1.f, 1.f, -1.f) * armSize, Vector3(1.f, -1.f, -1.f) * armSize,
		// Left
		Vector3(1.f, -1.f, -1.f) * armSize, Vector3(1.f, -1.f, 1.f) * armSize, Vector3(1.f, 1.f, 1.f) * armSize, Vector3(1.f, 1.f, -1.f) * armSize,
		// Top
		Vector3(-1.f, 1.f, -1.f) * armSize, Vector3(1.f, 1.f, -1.f) * armSize, Vector3(1.f, 1.f, 1.f) * armSize, Vector3(-1.f, 1.f, 1.f) * armSize,
		// Bottom
		Vector3(-1.f, -1.f, -1.f) * armSize, Vector3(1.f, -1.f, -1.f) * armSize, Vector3(1.f, -1.f, 1.f) * armSize, Vector3(-1.f, -1.f, 1.f) * armSize
	};
	rightLegMesh._Indices = {
		0, 1, 2, 0, 2, 3, // Right
		4, 6, 5, 4, 7, 6, // Front
		8, 9, 10, 8, 10, 11, // Back
		12, 14, 13, 12, 15, 14, // Left
		16, 18, 17, 16, 19, 18, // Top
		20, 21, 22, 20, 22, 23  // Bottom
	};
	rightLegMesh._UVs = {
		// Right
		Vector2(0.f, 32.f) / 64.f, Vector2(4.f, 32.f) / 64.f, Vector2(4.f, 44.f) / 64.f, Vector2(0.f, 44.f) / 64.f,
		// Front
		Vector2(4.f, 32.f) / 64.f, Vector2(4.f, 44.f) / 64.f, Vector2(8.f, 44.f) / 64.f, Vector2(8.f, 32.f) / 64.f,
		// Back
		Vector2(12.f, 32.f) / 64.f, Vector2(12.f, 44.f) / 64.f, Vector2(8.f, 44.f) / 64.f, Vector2(8.f, 32.f) / 64.f,
		// Left
		Vector2(16.f, 32.f) / 64.f, Vector2(12.f, 32.f) / 64.f, Vector2(12.f, 44.f) / 64.f, Vector2(16.f, 44.f) / 64.f,
		// Top
		Vector2(4.f, 48.f) / 64.f, Vector2(8.f, 48.f) / 64.f, Vector2(8.f, 44.f) / 64.f, Vector2(4.f, 44.f) / 64.f,
		// Bottom
		Vector2(8.f, 48.f) / 64.f, Vector2(12.f, 48.f) / 64.f, Vector2(12.f, 44.f) / 64.f, Vector2(8.f, 44.f) / 64.f
	};

	headMesh.CalculateBounds();
	bodyMesh.CalculateBounds();
	leftArmMesh.CalculateBounds();
	rightArmMesh.CalculateBounds();
	leftLegMesh.CalculateBounds();
	rightLegMesh.CalculateBounds();

	AddMesh(GameEngine::HeadMeshKey, headMesh);
	AddMesh(GameEngine::BodyMeshKey, bodyMesh);
	AddMesh(GameEngine::LArmMeshKey, leftArmMesh);
	AddMesh(GameEngine::RArmMeshKey, rightArmMesh);
	AddMesh(GameEngine::LLegMeshKey, leftLegMesh);
	AddMesh(GameEngine::RLegMeshKey, rightLegMesh);

	// 텍스쳐 로딩
	Texture diffuseTexture(SteveTexturePath);
	if(!diffuseTexture.IsIntialized())
	{
		return false;
	}
	AddTexture(GameEngine::DiffuseTexture, diffuseTexture);

	return true;
}

bool GameEngine::LoadScene()
{
	static const float baseScale = 100.f;
	static const Vector3 bodyOffset(0.f, 0.75f, 0.f);
	static const Vector3 headPivotOffset(0.f, 0.75f, 0.f);
	static const Vector3 headOffset(0.f, 0.5f, 0.f);
	static const Vector3 lArmPivotOffset(-0.75f, 0.5f, 0.f);
	static const Vector3 lArmOffset(0.f, -0.5f, 0.f);
	static const Vector3 rArmPivotOffset(0.75f, 0.5f, 0.f);
	static const Vector3 rArmOffset(0.f, -0.5f, 0.f);
	static const Vector3 lLegPivotOffset(-0.25f, -0.75f, 0.f);
	static const Vector3 lLegOffset(0.f, -0.75f, 0.f);
	static const Vector3 rLegPivotOffset(0.25f, -0.75f, 0.f);
	static const Vector3 rLegOffset(0.f, -0.75f, 0.f);

	// 플레이어 루트 설정
	GameObject player(GameEngine::PlayerKey);

	// 바디 설정
	GameObject body(GameEngine::BodyKey);
	body.SetMesh(GameEngine::BodyMeshKey);
	body.GetTransform().SetPosition(bodyOffset);
	body.SetParent(player);

	// 머리 피봇 설정
	GameObject headPivot(GameEngine::HeadPivotKey);
	headPivot.SetParent(body);
	headPivot.GetTransform().SetPosition(headPivotOffset);

	// 머리 설정
	GameObject head(GameEngine::HeadKey);
	head.SetMesh(GameEngine::HeadMeshKey);
	head.SetParent(headPivot);
	head.GetTransform().SetPosition(headOffset);

	// 왼팔 피봇 설정
	GameObject leftArmPivot(GameEngine::LArmPivotKey);
	leftArmPivot.SetParent(body);
	leftArmPivot.GetTransform().SetPosition(lArmPivotOffset);

	// 왼팔 설정
	GameObject leftArm(GameEngine::LArmKey);
	leftArm.SetMesh(GameEngine::LArmMeshKey);
	leftArm.SetParent(leftArmPivot);
	leftArm.GetTransform().SetPosition(lArmOffset);

	// 오른팔 피봇 설정
	GameObject rightArmPivot(GameEngine::RArmPivotKey);
	rightArmPivot.SetParent(body);
	rightArmPivot.GetTransform().SetPosition(rArmPivotOffset);

	// 오른팔 설정
	GameObject rightArm(GameEngine::RArmKey);
	rightArm.SetMesh(GameEngine::RArmMeshKey);
	rightArm.SetParent(rightArmPivot);
	rightArm.GetTransform().SetPosition(rArmOffset);

	// 왼발 피봇 설정
	GameObject leftLegPivot(GameEngine::LLegPivotKey);
	leftLegPivot.SetParent(body);
	leftLegPivot.GetTransform().SetPosition(lLegPivotOffset);

	// 왼발 설정
	GameObject leftLeg(GameEngine::LLegKey);
	leftLeg.SetMesh(GameEngine::LLegMeshKey);
	leftLeg.SetParent(leftLegPivot);
	leftLeg.GetTransform().SetPosition(lLegOffset);

	// 오른발 피봇 설정
	GameObject rightLegPivot(GameEngine::RLegPivotKey);
	rightLegPivot.SetParent(body);
	rightLegPivot.GetTransform().SetPosition(rLegPivotOffset);

	// 오른발 설정
	GameObject rightLeg(GameEngine::RLegKey);
	rightLeg.SetMesh(GameEngine::RLegMeshKey);
	rightLeg.SetParent(rightLegPivot);
	rightLeg.GetTransform().SetPosition(rLegOffset);

	// 플레이어 크기 설정
	player.GetTransform().SetScale(Vector3::One * baseScale);
	player.GetTransform().SetRotation(Rotator(180.f, 0.f, 0.f));

	// 최종 추가
	AddGameObject(player);
	AddGameObject(body);
	AddGameObject(headPivot);
	AddGameObject(head);
	AddGameObject(leftArmPivot);
	AddGameObject(leftArm);
	AddGameObject(rightArmPivot);
	AddGameObject(rightArm);
	AddGameObject(leftLegPivot);
	AddGameObject(leftLeg);
	AddGameObject(rightLegPivot);
	AddGameObject(rightLeg);

	// 카메라 설정
	_MainCamera.GetTransform().SetPosition(Vector3(0.f, 50.f, -700.f));

	return true;
}

bool GameEngine::AddMesh(const std::size_t& InKey, const Mesh& InMesh)
{
	auto meshPtr = std::make_unique<Mesh>(InMesh);
	return _Meshes.insert({ InKey, std::move(meshPtr) }).second;
}

bool GameEngine::AddTexture(const std::size_t& InKey, const Texture& InTexture)
{
	auto texturePtr = std::make_unique<Texture>(InTexture);
	return _Textures.insert({ InKey, std::move(texturePtr) }).second;
}

// 정렬하면서 삽입하기
bool GameEngine::AddGameObject(const GameObject& InGameObject)
{
	auto goPtr = std::make_unique<GameObject>(InGameObject);

	const auto it = std::lower_bound(SceneBegin(), SceneEnd(), InGameObject, 
		[](const std::unique_ptr<GameObject>& lhs, const GameObject& rhs) 
		{
			return lhs->GetHash() < rhs.GetHash(); 
		}
	);

	if (it == _Scene.end())
	{
		_Scene.push_back(std::move(goPtr));
		return true;
	}

	std::size_t inHash = InGameObject.GetHash();
	std::size_t targetHash = (*it)->GetHash();

	if (targetHash == inHash)
	{
		// 중복된 키 발생. 무시.
		return false;
	}
	else if (targetHash < inHash)
	{
		_Scene.insert(it + 1, std::move(goPtr));
	}
	else
	{
		_Scene.insert(it, std::move(goPtr));
	}

	return true;
}

GameObject& GameEngine::GetGameObject(const std::string& InName)
{
	std::size_t targetHash = std::hash<std::string>()(InName);
	const auto it = std::lower_bound(SceneBegin(), SceneEnd(), targetHash,
		[](const std::unique_ptr<GameObject>& lhs, std::size_t rhs)
	{
		return lhs->GetHash() < rhs;
	}
	);

	if (it != _Scene.end())
	{
		return *(*it).get();
	}

	return const_cast<GameObject&>(GameObject::NotFound);
}
