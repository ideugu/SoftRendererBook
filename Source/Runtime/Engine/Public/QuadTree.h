#pragma once

namespace CK
{

class QuadTree
{
public:
	QuadTree() = default;
	QuadTree(const Rectangle& InBound, int InLevel) : Bound(InBound), Level(InLevel) { }
	QuadTree(const Rectangle& InBound, int InLevel, int InMaxLevel, size_t InNodeCapacity) : Bound(InBound), Level(InLevel), MaxLevel(InMaxLevel), NodeCapacity(InNodeCapacity) { }
	~QuadTree() { Clear(); }

private:
	enum SubNames
	{
		TopLeft = 0,
		TopRight = 1,
		BottomRight = 2,
		BottomLeft = 3
	};

	struct TreeNode
	{
		TreeNode() = delete;
		TreeNode(const std::string& InNodeKey, const Rectangle& InBound) : NodeKey(InNodeKey), NodeBound(InBound) { }

		Rectangle NodeBound;
		std::string NodeKey;
	};

public:
	bool Insert(const std::string& InKey, const Rectangle& InBound);
	void Clear();
	void Query(const Rectangle& InRectangleToQuery, std::vector<std::string>& InOutItems) const;
	const Rectangle& GetBound() const { return Bound; }
	bool HasItem() { return !IsLeaf; }
	QuadTree* GetNextTree()
	{
		if (IsLeaf)
		{
			return nullptr;
		}
		else
		{

		}
	}

private:
	void Split();
	bool Contains(const Rectangle& InBox) const;
	QuadTree* FindSubTree(const Rectangle& InBound);

private:
	Rectangle Bound;
	std::list<TreeNode> Nodes;

	bool IsLeaf = true;
	int Level = 1;
	int MaxLevel = 10;
	size_t NodeCapacity = 6;

	QuadTree* SubTrees[4] = { nullptr };
};

}

