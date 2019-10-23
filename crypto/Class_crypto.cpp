#include "class_crypto.h"
#include <string.h>
#include <iostream>
#include <fstream>

class_crypto::class_crypto()
{

}

class_crypto::~class_crypto()
{

}

int class_crypto::f_crypto(char* mode, char* filename, char* EDSmode, char* withpass)
{
	if (!strcmp(mode, "encrypt"))
	{
		char* gostfilename = new char[strlen(filename) + 6];
		strncpy(gostfilename, filename, strlen(filename));
		gostfilename[strlen(filename)] = '\0';
		strcat(gostfilename, ".gost");

		if (magma_encrypt(filename, gostfilename, withpass))
		{
			delete[] gostfilename;
			return 1;
		}

		char* hashfilename = new char[strlen(filename) + 6];
		strncpy(hashfilename, filename, strlen(filename));
		hashfilename[strlen(filename)] = '\0';
		strcat(hashfilename, ".hash");

		ripe_hash(gostfilename, hashfilename);
		delete[] hashfilename;

		char* rsafilename = new char[strlen(filename) + 6];
		strncpy(rsafilename, filename, strlen(filename));
		rsafilename[strlen(filename)] = '\0';
		strcat(rsafilename, ".eds");

		if (rsa_encrypt(gostfilename, rsafilename, "MyKey.eds", (!strcmp(EDSmode, "with_new_rsa_pass")) ? 1 : 0))
		{
			delete[] rsafilename;
			delete[] gostfilename;
			return 2;
		}

		delete[] rsafilename;
		delete[] gostfilename;
		return 0;
	}
	else
		if (!strcmp(mode, "decrypt"))
		{
			char* point = strrchr(filename, '.');
			char filenamelen = strlen(filename) - strlen(point);
			char* decfilename = new char[filenamelen + 6];
			strncpy(decfilename, filename, filenamelen);
			decfilename[filenamelen] = '\0';
			filenamelen++;

			char* gostfilename = new char[filenamelen + 6];
			strncpy(gostfilename, decfilename, filenamelen);
			gostfilename[strlen(filename)] = '\0';
			strcat(gostfilename, ".gost");

			switch (rsa_decrypt(filename, gostfilename, "MyKey.eds"))
			{
			case 1:
				delete[] decfilename;
				delete[] gostfilename;
				return 1;
			case 2:
				delete[] decfilename;
				delete[] gostfilename;
				return 2;
			}

			char* hashfilename = new char[filenamelen + 6];
			strncpy(hashfilename, decfilename, filenamelen);
			hashfilename[strlen(filename)] = '\0';
			strcat(hashfilename, ".hash");

			if (check_ripe_hash(gostfilename, hashfilename))
			{
				delete[] hashfilename;
				delete[] gostfilename;
				delete[] decfilename;
				return 3;
			}
			delete[] hashfilename;

			magma_decrypt(gostfilename, decfilename, withpass);

			delete[] gostfilename;
			delete[] decfilename;
			return 0;
		}
		else return 4;
}

int class_crypto::magma_encrypt(char* infilename, char* outfilename, char* sympass)
{
	ifstream fin(infilename);
	if (fin.is_open())
	{
		fin.seekg(0, std::ios::end);
		__int64 infilesize = fin.tellg();
		char* infilestr = new char[infilesize];
		char* outfilestr = new char[8 * (infilesize / 8 + 1) + 1];
		fin.close();
		fin.open(infilename);

		fin.read(infilestr, infilesize);
		infilestr[infilesize] = 0;
		fin.close();

		CMagma GOST;
		//GOST.SetKey(sympass);
		GOST.StartNewSession(1);

		union
		{
			unsigned char chars[8];
			unsigned __int64 X;
		} CONVERT;

		int i = 0;
		while (i < (infilesize - 8))
		{
			for (int k = 0; k < 8; k++) CONVERT.chars[k] = infilestr[i + k];
			CONVERT.X = GOST.Encrypt(CONVERT.X);
			for (int k = 0; k < 8; k++) outfilestr[i + k] = CONVERT.chars[k];
			i += 8;
		}

		for (int k = 0; k < 8; k++) CONVERT.chars[k] = ((i + k) < infilesize) ? infilestr[i + k] : 0;
		CONVERT.X = GOST.Encrypt(CONVERT.X);
		for (int k = 0; k < 8; k++) outfilestr[i + k] = CONVERT.chars[k];
		outfilestr[8 * (infilesize / 8 + 1)] = 0;

		ofstream fout(outfilename);
		fout.write(outfilestr, 8 * (infilesize / 8 + 1));
		fout.close();

		return 0;
	}
	else return 1;
}

int class_crypto::magma_decrypt(char* infilename, char* outfilename, char* sympass)
{
	ifstream fin(infilename);
	if (fin.is_open())
	{
		fin.seekg(0, std::ios::end);
		__int64 infilesize = fin.tellg();
		infilesize++;
		char* infilestr = new char[infilesize];

		char* outfilestr = new char[(infilesize - 1)];
		fin.close();
		fin.open(infilename);

		fin.read(infilestr, infilesize);
		fin.close();
		infilesize--;

		CMagma GOST;
		//GOST.SetKey(sympass);
		GOST.StartNewSession(1);

		union
		{
			unsigned char chars[8];
			unsigned __int64 X;
		} CONVERT;

		int i = 0;
		while (i < infilesize)
		{
			for (int k = 0; k < 8; k++) CONVERT.chars[k] = infilestr[i + k];
			CONVERT.X = GOST.Decrypt(CONVERT.X);
			for (int k = 0; k < 8; k++) outfilestr[i + k] = CONVERT.chars[k];
			i += 8;
		}

		ofstream fout(outfilename);
		fout << outfilestr;
		fout.close();

		return 0;
	}
	else return 1;
}

int class_crypto::rsa_encrypt(char* infilename, char* outfilename, char* Keyfile, char mode)
{
	std::ifstream fin(infilename);
	if (fin.is_open())
	{
		std::ofstream fout(outfilename);

		CRSA RSA;
		if (mode)
			RSA.GenerateNewKeys(Keyfile);
		else
			if (RSA.SetKey(Keyfile)) return 2;

		RSA.Encrypt(fin, fout);
		fin.close();
		fout.close();
		return 0;
	}
	else return 1;
}

int class_crypto::rsa_decrypt(char* infilename, char* outfilename, char* Keyfile)
{
	std::ifstream fin(infilename);
	if (fin.is_open())
	{
		std::ofstream fout(outfilename);

		CRSA RSA;
		if (RSA.SetKey(Keyfile)) return 2;
		RSA.Decrypt(fin, fout);
		fin.close();
		fout.close();
		return 0;
	}
	else return 1;
}

int class_crypto::ripe_hash(char* infilename, char* outfilename)
{
	ifstream fin(infilename);
	if (fin.is_open())
	{
		fin.seekg(0, std::ios::end);
		unsigned __int64 size = fin.tellg();
		char* str = new char[size];
		fin.close();
		fin.open(infilename);

		fin.getline(str, size);
		fin.close();
		CRipeMD160 Ripe;
		char* hash = new char[41];

		hash = Ripe.GetHASH(str);
		cout << "HASH of file " << infilename << ": " << hash << '\n';

		ofstream fout(outfilename);
		fout << hash;
		fout.close();

		delete[] str;
		delete[] hash;
		return 0;
	}
	else return 1;
}

int class_crypto::check_ripe_hash(char* infilename, char* checkfilename)
{
	ifstream fin(infilename);
	if (fin.is_open())
	{
		fin.seekg(0, std::ios::end);
		unsigned __int64 size = fin.tellg();
		char* str = new char[size];
		fin.close();
		fin.open(infilename);

		fin.getline(str, size);
		fin.close();
		CRipeMD160 Ripe;
		char* hash = new char[41];

		hash = Ripe.GetHASH(str);

		ifstream fchk(checkfilename);
		char* checkhash = new char[41];
		fchk >> checkhash;
		fchk.close();

		int res = strcmp(checkhash, hash);
		if (!res) cout << "HASH checked!\n";
		delete[] str;
		delete[] hash;
		delete[] checkhash;
		return res ? 1 : 0;
	}
	else return 1;
}
