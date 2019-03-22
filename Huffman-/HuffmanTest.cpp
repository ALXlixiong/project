#define CRT_SECURE_NO_WARNINGS 
#include "HuffmanNode.hpp"
#include "FileHuffmanCompress.hpp"
int main()
{
	FileHuffmanCompress hf;
	hf.HuffmanCompress("IMG_2322.JPG");
	hf.UnhuffmanCompress("2.JPG");
	
	return 0;
}


