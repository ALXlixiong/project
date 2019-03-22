#pragma once
#include "HuffmanNode.hpp"

class FileHuffmanCompress
{
public:
	void HuffmanCompress(const std::string filename);
	void UnhuffmanCompress(const std::string &filename);
	void GetHuffmanCode(Huffman<CharInfo>& hf,std::vector<CharInfo>& Info);
	void GetLeafNode(HuffmanNode<CharInfo>* pRoot, std::vector<CharInfo>& Info);
	void FileCompress(FILE* Fin, FILE* Fout, std::vector<CharInfo>& Info);
	std::string GetFileExtension(const std::string& filename);
	void GetCharCountString(std::string& RetString, long long CharCount);
	void GetLine(FILE* Fin, std::string& LastName);
	void UNcompress(FILE* Fin,HuffmanNode<CharInfo>* pRoot, std::string &Lastname);
private:
	CharInfo info;
};