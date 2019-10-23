#pragma once
#include "CMagma.h"
#include "CRipeMD160.h"
#include "CRSA.h"

class class_crypto
{
public:
	class_crypto();
	~class_crypto();

public:
	int f_crypto(char* mode, char* filename, char* EDSmode, char* withpass);

protected:
	int magma_encrypt(char* infilename, char* outfilename, char* sympass);
	int magma_decrypt(char* infilename, char* outfilename, char* sympass);

protected:
	int rsa_encrypt(char* infilename, char* outfilename, char* Keyfile, char mode);
	int rsa_decrypt(char* infilename, char* outfilename, char* Keyfile);

protected:
	int ripe_hash(char* infilename, char* outfilename);
	int check_ripe_hash(char* infilename, char* checkfilename);
};
