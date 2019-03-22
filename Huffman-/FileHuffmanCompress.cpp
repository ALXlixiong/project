#include "FileHuffmanCompress.hpp"

void FileHuffmanCompress::GetLeafNode(HuffmanNode<CharInfo> *pRoot, std::vector<CharInfo>& Info)
{
	if (pRoot == nullptr)
		return;

	GetLeafNode(pRoot->_pLeft, Info);
	GetLeafNode(pRoot->_pRight, Info);
	if (pRoot->_pLeft == nullptr && pRoot->_pRight == nullptr)
	{
		HuffmanNode<CharInfo> *pCur = pRoot;
		while (pCur->_pParent != nullptr)
		{
			if (pCur == pCur->_pParent->_pLeft)
				Info[(unsigned char)pRoot->_weight._Char]._CharCode += '0';
			else
				Info[(unsigned char)pRoot->_weight._Char]._CharCode += '1';
			pCur = pCur->_pParent;
		}
		reverse(Info[(unsigned char)pRoot->_weight._Char]._CharCode.begin(), Info[(unsigned char)pRoot->_weight._Char]._CharCode.end());
	}
}

void FileHuffmanCompress::GetHuffmanCode(Huffman<CharInfo>& hf, std::vector<CharInfo>& Info)
{
		HuffmanNode<CharInfo> *pRoot = hf.GetRoot();
		GetLeafNode(pRoot, Info);
}

void FileHuffmanCompress::FileCompress(FILE* Fin,FILE* Fout, std::vector<CharInfo>& Info)
{
	
	char *buff = new char[1024];
	while (!feof(Fin))
	{
		memset(buff, 0, 1024);
		
		size_t rdSize = fread(buff, 1, 1024, Fin);
		std::string str("");
		for (size_t i = 0; i < rdSize; ++i)
		{
			for (size_t j = 0; j < Info[(unsigned char)buff[i]]._CharCode.size(); ++j)
			{
				if (str.size() < 8)
				{
					str += Info[(unsigned char)buff[i]]._CharCode[j];
				}
				else
				{
					//ABBBCCCCCDDDDDDD
					//A:100  B:101  C:11  D:0
					//10010110 11011111 11111100 00000
					//0x96       0xdf      0xfc    0x00
					char ch = 0;
					for (int i = 0; i < 8; ++i)
					{
						ch <<= 1;
						if (str[i] == '1')
							ch = ch | 1;
						else
							ch = ch | 0;
					}
					fputc(ch, Fout);
					ch = 0;
					str = "";
					j = j - 1;
				}
			}
		}
		if (str.size() <= 8 && str.size() > 0)
		{
			char ch = 0;
			for (size_t i = 0; i < str.size(); ++i)
			{
				ch <<= 1;
				if (str[i] == '1')
					ch = ch | 1;
				else
					ch = ch | 0;
			}
			fputc(ch, Fout);
		}
	}
}

std::string FileHuffmanCompress::GetFileExtension(const std::string& filename)
{
	size_t pos = filename.rfind('.');
	return filename.substr(pos);
}

void FileHuffmanCompress::GetCharCountString(std::string& RetString, long long CharCount)
{
	if (CharCount == 0)
		return;
	GetCharCountString(RetString, CharCount / 10);
	//�����Ǹ��ӣ���������ַ���0������������Ͳ���RetString��Ԫ�ز���CharCount���ַ���ʽ������ascll����ڸ�����
	//���Ա���Ҫ���ϸ��ַ�0
	RetString += (CharCount % 10)+'0';
}

void FileHuffmanCompress::HuffmanCompress(const std::string filename)
{
	//1.ͳ��ÿ���ַ����ֵĴ���
	FILE* Fin = fopen(filename.c_str(), "rb");
	if (nullptr == Fin)
	{
		perror("fopen");
		return;
	}
	char *buff = new char[1024];
	std::vector<CharInfo> Info(256);
	for (int i = 0; i < 256; ++i)
		Info[i]._Char = i;
	while (1)
	{
		size_t rdSize = fread(buff, 1, 1024, Fin);
		if (rdSize == 0)
			break;
		for (size_t i = 0; i < rdSize; ++i)
			++Info[(unsigned char)buff[i]]._CharCount;
	}
	//�Ѿ���ȡ��һ���ļ�������������Ҫ���°ɹ�����õ���ʼλ��
	fseek(Fin, 0, SEEK_SET);
	//2.������������
	Huffman<CharInfo> hf;
	hf.CreateHuffman(Info);
	//3.��ȡ����������
	GetHuffmanCode(hf,Info);
	//4.ѹ���ļ�
	FILE* Fout = fopen("./2.JPG", "wb");
	if (nullptr == Fout)
	{
		perror("fopen");
		return;
	}
	fseek(Fin, 0, SEEK_SET);
	//�洢��׺
	std::string FileExtensinon = GetFileExtension(filename);
	fwrite(FileExtensinon.c_str(), FileExtensinon.size(), 1, Fout);
	fputc('\n', Fout);
	//�洢huffman���ڵ�
	long long lineCount = 0;
	for (size_t i = 0; i < Info.size(); ++i)
	{
		if (Info[i]._CharCode != "")
			++lineCount;
	}
	std::string LineString = "";
	GetCharCountString(LineString, lineCount);
	fwrite(LineString.c_str(), LineString.size(), 1, Fout);
	fputc('\n', Fout);
	for (size_t i = 0; i < Info.size(); ++i)
	{
		if (Info[i]._CharCode != "")
		{
			fputc(Info[i]._Char, Fout);
			fputc(',', Fout);
			//char buff[10];
			//Info[i]._CharCount��һ��long long���ͣ��洢ʱ��Ҫת��Ϊ�ַ���
			std::string RetString = "";
			GetCharCountString(RetString,Info[i]._CharCount);
			fwrite(RetString.c_str(), RetString.size(), 1, Fout);
			fputc('\n', Fout);
		}
	}
	FileCompress(Fin,Fout,Info);
	fclose(Fin);
	fclose(Fout);
}

void FileHuffmanCompress::GetLine(FILE* Fin, std::string& LastName)
{
	int ch = 0;
	while (1)
	{
		ch = fgetc(Fin);
		if (ch == '\n')
			break;
		LastName += ch;
	}
}

void FileHuffmanCompress::UnhuffmanCompress(const std::string &filename)
{
	FILE* Fin = fopen(filename.c_str(), "rb");
	if (nullptr == Fin)
	{
		perror("fopen");
		return;
	}
	char *buff = new char[1024];
	std::string Lastname = "";//��׺��
	std::vector<CharInfo> v;//�ַ���Ȩ������
	int LineCount = 0;//����
	std::string line = "";

	GetLine(Fin, Lastname);
	GetLine(Fin, line);
	LineCount = atoi(line.c_str());
	v.resize(256);
	for (int i = 0; i < 256; ++i)
		v[i]._Char = i;
	std::string tmp = "";
	for (int i = 0; i < LineCount; ++i)
	{
		tmp = "";
		GetLine(Fin, tmp);
		if (tmp.empty())
		{
			tmp += '\n';
			GetLine(Fin, tmp);
		}
		std::string index = tmp.substr(tmp.find(',')+1);
		v[(unsigned char)tmp[0]]._CharCount = atoi(index.c_str());
	}
	Huffman<CharInfo> hf;
	//������������
	hf.CreateHuffman(v);
	//��ѹ�ļ�
	UNcompress(Fin,hf.GetRoot(),Lastname);
}

void FileHuffmanCompress::UNcompress(FILE* Fin, HuffmanNode<CharInfo>* pRoot, std::string &Lastname)
{
	std::string Name = "�ı�";
	Name += Lastname;
	long long FileSize = pRoot->_weight._CharCount;
	FILE* FOut = fopen(Name.c_str(), "wb");
	assert(FOut);
	char* buff = new char[1024];
	HuffmanNode<CharInfo>* root = pRoot;
	while (!feof(Fin))
	{
		size_t rdSize = fread(buff, 1, 1024, Fin);
		if (rdSize == 0)
			break;
		if (rdSize > 0)
		{
			for (size_t i = 0; i < rdSize; ++i)
			{
				for (int j = 7; j >=0; --j)
				{
					int ch = buff[i] & (1 << j);
					if (ch == 0)
						pRoot = pRoot->_pLeft;
					else
						pRoot = pRoot->_pRight;
					if (pRoot->_pLeft == nullptr && pRoot->_pRight == nullptr)
					{
						fputc(pRoot->_weight._Char, FOut);
						--FileSize;
						if (FileSize == 0)
							break;
						pRoot = root;
					}
				}
			}
		}
	}
	fclose(Fin);
	fclose(FOut);
}
