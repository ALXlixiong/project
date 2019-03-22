#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <functional>
#include <string>
#include <stdlib.h>
#include <assert.h>

class CharInfo
{
public:
	char _Char;
	long long _CharCount;
	std::string _CharCode;
	CharInfo()
	{
		_CharCount = 0;
		_CharCode = "";
	}

	CharInfo operator+(const CharInfo& info)
	{
		CharInfo tmp;
		tmp._CharCount = _CharCount + info._CharCount;
		return tmp;
	}

};
//模板不支持分离编译：也就是模板类型声明和定义放在同一个文件，当然我们可以使用hpp
template<class T>
struct HuffmanNode
{
	struct HuffmanNode *_pLeft;
	struct HuffmanNode *_pRight;
	struct HuffmanNode *_pParent;
	T _weight;
	HuffmanNode()
		: _pLeft(nullptr)
		, _pRight(nullptr)
		, _pParent(nullptr)
	{}
};


template<class T>
class Huffman
{
	typedef HuffmanNode<T> Node;
	typedef Node* pNode;
public:
	Huffman()
		:_pRoot(nullptr)
	{}
	struct cmp
	{
		bool operator()(pNode& left, pNode& right)
		{
			return left->_weight._CharCount > right->_weight._CharCount;
		}
	};
	void CreateHuffman(std::vector<T> &v)
	{
		//优先级队列默认是大堆，我们存储的是地址，我们所需要的是小堆，所以我们需要改变比较方式
		//比较器：参数也是优先级队列元素类型，节点的地址实际比较的就是权值。
		std::priority_queue<pNode, std::vector<pNode>, cmp> q;
		for (size_t i = 0; i < v.size(); ++i)
		{
			if (v[i]._CharCount != 0)
			{
				pNode node = new Node;
				node->_weight = v[i];
				node->_pLeft = nullptr;
				node->_pRight = nullptr;
				node->_pParent = nullptr;
				q.push(node);
			}
		}
		pNode Parent = nullptr;
		while (q.size()>1)
		{
			Parent = new Node;
			pNode left = q.top();
			q.pop();
			pNode right = q.top();
			q.pop();
			Parent->_pLeft = left;
			Parent->_pRight = right;
			left->_pParent = Parent;
			right->_pParent = Parent;
			Parent->_weight = left->_weight + right->_weight;
			q.push(Parent);
		}
		_pRoot = q.top();
	}

	void Destory(pNode& root)
	{
		if (nullptr == root)
			return;

		Destory(root->_pLeft);
		Destory(root->_pRight);
		delete root;
		root = nullptr;
	}
	~Huffman()
	{
		Destory(_pRoot);
	}
	pNode GetRoot()
	{
		return _pRoot;
	}
private:
	pNode _pRoot;
};