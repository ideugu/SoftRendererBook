
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
		// �ִ� ������ �����ϸ� �и� ���� �����۸� �߰�.
		// �� ��� Capacity�� ����.
		Nodes.emplace_back(InKey, InBound);
		return true;
	}
	else
	{
		// ������ ����ε� �뷮�� �� á���� 4���� �ڽ� ��带 ���Ӱ� ����
		bool doSplit = IsLeaf && (Nodes.size() >= NodeCapacity);
		if (doSplit)
		{
			// �и�
			Split();

			// ���Ӱ� �и��� ��忡 ���ԵǴ��� �˻��ϰ� ����� ���� ���� ��忡 �ְų� �ڽ� ���� ����������.
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
				// ������ ����ε� �뷮�� �˳��ϸ� �߰��ϰ� ������
				Nodes.emplace_back(InKey, InBound);
				return true;
			}
			else
			{
				// ������ ��尡 �ƴϸ� ��ġ���� �˻��ϰ� ����� ���� ���� ��忡 �ְų� �ڽ� ���� ����������.
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
		// ������ ���� ���� ��� ����� ��� �߰�.
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

	// Top Left ����
	Vector2 topLeftMin = center - xExtent;
	Vector2 topLeftMax = center + yExtent;
	SubTrees[SubNames::TopLeft] = new QuadTree(Rectangle(topLeftMin, topLeftMax), nextLevel, MaxLevel, NodeCapacity);

	// Top Right ����
	Vector2 topRightMin = center;
	Vector2 topRightMax = Bound.Max;
	SubTrees[SubNames::TopRight] = new QuadTree(Rectangle(topRightMin, topRightMax), nextLevel, MaxLevel, NodeCapacity);

	// Bottom Right ����
	Vector2 bottomRightMin = center - yExtent;
	Vector2 bottomRightMax = center + xExtent;
	SubTrees[SubNames::BottomRight] = new QuadTree(Rectangle(bottomRightMin, bottomRightMax), nextLevel, MaxLevel, NodeCapacity);

	// Bottom Left ����
	Vector2 bottomLeftMin = Bound.Min;
	Vector2 bottomLeftMax = center;
	SubTrees[SubNames::BottomLeft] = new QuadTree(Rectangle(bottomLeftMin, bottomLeftMax), nextLevel, MaxLevel, NodeCapacity);

	// �� �̻� �� ��尡 �ƴ�
	IsLeaf = false;

	// ���� ��� �� ��ġ�� �ʴ� ���� �ϴ����� �̵��ϰ� ��ġ�� ��常 ���ܵ�.
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

	// ���� ��� ����Ʈ�� �����ϰ� ��ġ�� ��� �������� ����
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

	// �� ���� �ڽ� ��带 ���鼭 ���� �����ϴ� �ڽ� Ʈ���� �ִ��� ����.
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