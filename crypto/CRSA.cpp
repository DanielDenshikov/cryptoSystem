#include "CRSA.h"
#include <ctime>

CRSA::CRSA()
{

}

CRSA::~CRSA()
{

}

int CRSA::SetKey(char* EDSfile)
{
	std::ifstream fin(EDSfile);
	if (fin.is_open())
	{
		fin >> n;
		fin >> e;
		fin >> d;
		return 0;
	}
	else return 1;
}

void CRSA::GenerateNewKeys(char* EDSfile)
{
	ZZ p, q, phi_n;
	SetSeed(conv<ZZ>((long)time(NULL)));
	GenPrime(p, 1024);

	GenPrime(q, 1024);
	while (p == q) GenPrime(q, 1024);

	n = p * q;
	phi_n = (p - 1) * (q - 1);

	e = pow(2, pow(2, (RandomBnd(5) + 3))) + 1;
	while (!(IsOne(GCD(e, phi_n))))	e = pow(2, pow(2, (RandomBnd(5) + 3))) + 1;

	d = InvMod(e, phi_n);

	std::ofstream fout(EDSfile);
	fout << n << '\n';
	fout << e << '\n';
	fout << d << '\n';
	fout.close();
}

void CRSA::Encrypt(std::ifstream &fin, std::ofstream &fout)
{
	ZZ message;
	ZZ ciphertext;

	fin.seekg(0, std::ios::end);
	__int64 infilesize = fin.tellg();
	char* infilestr = new char[infilesize];
	fin.seekg(0, std::ios::beg);

	fin.read(infilestr, infilesize);
	fin.close();
	infilestr[infilesize] = 0;

	int i = 0;
	while (i < ((infilesize / 224) * 224))
	{
		message = 0;
		for (int j = 223; j >= 0; j--)
		{
			message <<= 8;
			message |= (unsigned char)infilestr[i + j];
		}

		ciphertext = PowerMod(message, e, n);
		fout << ciphertext << '\n';
		i += 224;
	}

	if (infilesize % 224)
	{
		message = 0;
		for (int j = (infilesize % 224); j >= 0; j--)
		{
			message <<= 8;
			message |= (unsigned char)infilestr[i + j];
		}

		ciphertext = PowerMod(message, e, n);
		fout << ciphertext << '\n';
	}
}

void CRSA::Decrypt(std::ifstream &fin, std::ofstream &fout)
{
	ZZ ciphertext;
	ZZ text;
	char Sym = 1;
	while (fin >> ciphertext)
	{
		text = PowerMod(ciphertext, d, n);
		Sym = 0;
		for (int k = 0; k < 8; k++)
		{
			if (!IsZero(text & 1))Sym |= 1 << k;
			text >>= 1;
		}
		while (Sym)
		{
			fout << Sym;
			Sym = 0;
			for (int k = 0; k < 8; k++)
			{
				if (!IsZero(text & 1))Sym |= 1 << k;
				text >>= 1;
			}
		}
	}

}