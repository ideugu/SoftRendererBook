
#include "Precompiled.h"
using namespace CK;

bool QuadTree::Insert(const std::string& InKey, const Rectangle& InBound)
{
	if (!Bound.Intersect(InBound))
	{
		return false;
	}

	if (Level >= MaxLevel)
	{
		// 최대 레벨에 도달하면 분리 없이 아이템만 추가.
		// 이 경우 Capacity는 무시.
		Nodes.emplace_back(InKey, InBound);
		return true;
	}
	else
	{
		// 마지막 노드인데 용량이 꽉 찼으면 4개의 자식 노드를 새롭게 분할
		bool doSplit = IsLeaf && (Nodes.size() >= NodeCapacity);
		if (doSplit)
		{
			// 분리
			Split();

			// 새롭게 분리된 노드에 포함되는지 검사하고 결과에 따라 현재 노드에 넣거나 자식 노드로 내려보내기.
			QuadTree* newNode = FindSubTree(InBound);
			if (newNode == nullptr)
			{
				Nodes.emplace_back(InKey, InBound);
			}
			else
			{
				newNode->Insert(InKey, InBound);
			}
			return true;
		}
		else
		{
			if (IsLeaf)
			{
				// 마지막 노드인데 용량이 넉넉하면 추가하고 끝내기
				Nodes.emplace_back(InKey, InBound);
				return true;
			}
			else
			{
				// 마지막 노드가 아니면 겹치는지 검사하고 결과에 따라 현재 노드에 넣거나 자식 노드로 내려보내기.
				QuadTree* newNode = FindSubTree(InBound);
				if (newNode == nullptr)
				{
					Nodes.emplace_back(InKey, InBound);
				}
				else
				{
					newNode->Insert(InKey, InBound);
				}

				return true;
			}
		}
	}
}

void QuadTree::Clear()
{
	if (!IsLeaf)
	{
		for (QuadTree* subTree : SubTrees)
		{
			assert(subTree != nullptr);
			delete subTree;
			subTree = nullptr;
		}
	}
	else
	{
		assert(SubTrees[SubNames::TopLeft] == nullptr);
		assert(SubTrees[SubNames::TopRight] == nullptr);
		assert(SubTrees[SubNames::BottomRight] == nullptr);
		assert(SubTrees[SubNames::BottomLeft] == nullptr);
	}
}

void QuadTree::Query(const Rectangle& InRectangleToQuery, std::vector<std::string>& InOutKeys) const
{
	if (Bound.Intersect(InRectangleToQuery))
	{
		// 마지막 노드면 현재 노드 목록을 모두 추가.
		for (const TreeNode& node : Nodes)
		{
			InOutKeys.emplace_back(node.NodeKey);
		}

		if (!IsLeaf)
		{
			for (QuadTree* subTree : SubTrees)
			{
				assert(subTree != nullptr);
				subTree->Query(InRectangleToQuery, InOutKeys);
			}
		}
	}
}

void QuadTree::Split()
{
	Vector2 extent, center;
	Bound.GetCenterAndExtent(center, extent);

	Vector2 xExtent = Vector2(extent.X, 0.f);
	Vector2 yExtent = Vector2(0.f, extent.Y);

	int nextLevel = Level + 1;

	// Top Left 영역
	Vector2 topLeftMin = center - xExtent;
	Vector2 topLeftMax = center + yExtent;
	SubTrees[SubNames::TopLeft] = new QuadTree(Rectangle(topLeftMin, topLeftMax), nextLevel, MaxLevel, NodeCapacity);

	// Top Right 영역
	Vector2 topRightMin = center;
	Vector2 topRightMax = Bound.Max;
	SubTrees[SubNames::TopRight] = new QuadTree(Rectangle(topRightMin, topRightMax), nextLevel, MaxLevel, NodeCapacity);

	// Bottom Right 영역
	Vector2 bottomRightMin = center - yExtent;
	Vector2 bottomRightMax = center + xExtent;
	SubTrees[SubNames::BottomRight] = new QuadTree(Rectangle(bottomRightMin, bottomRightMax), nextLevel, MaxLevel, NodeCapacity);

	// Bottom Left 영역
	Vector2 bottomLeftMin = Bound.Min;
	Vector2 bottomLeftMax = center;
	SubTrees[SubNames::BottomLeft] = new QuadTree(Rectangle(bottomLeftMin, bottomLeftMax), nextLevel, MaxLevel, NodeCapacity);

	// 더 이상 끝 노드가 아님
	IsLeaf = false;

	// 기존 노드 중 겹치지 않는 노드는 하단으로 이동하고 겹치는 노드만 남겨둠.
	std::list<TreeNode> overlappingNodes;

	for (const TreeNode& node : Nodes)
	{
		QuadTree* newNode = FindSubTree(node.NodeBound);
		if (newNode == nullptr)
		{
			overlappingNodes.push_back(node);
		}
		else
		{
			newNode->Insert(node.NodeKey, node.NodeBound);
		}
	}

	// 기존 노드 리스트를 제거하고 겹치는 노드 모음으로 변경
	Nodes.clear();
	Nodes.insert(Nodes.end(), overlappingNodes.begin(), overlappingNodes.end());
}

bool QuadTree::Contains(const Rectangle& InRectangle) const
{
	return Bound.IsInside(InRectangle);
}

QuadTree* QuadTree::FindSubTree(const Rectangle& InBound)
{
	if (IsLeaf)
	{
		return nullptr;
	}

	// 네 개의 자식 노드를 돌면서 완전 포함하는 자식 트리가 있는지 조사.
	for (QuadTree* subTree : SubTrees)
	{
		assert(subTree != nullptr);
		if (subTree->Contains(InBound))
		{
			return subTree;
		}
	}

	return nullptr;
}