#pragma once
#include <NTL/ZZ.h>
NTL_CLIENT

#include <iostream>
#include <fstream>

struct RSAKeys
{
	ZZ n, e, d;
};

class CRSA
{
public:
	CRSA();
	~CRSA();

private:
	ZZ n, e, d;
public:
	int SetKey(char* EDSfile);
	void GenerateNewKeys(char* EDSfile);
	void Encrypt(std::ifstream &fin, std::ofstream &fout);
	void Decrypt(std::ifstream &fin, std::ofstream &fout);
};
