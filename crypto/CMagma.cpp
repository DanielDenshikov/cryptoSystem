#include "CMagma.h"

CMagma::CMagma()
{
	Result = 0;
	Nonce = 1;
	Encrypt(Nonce);
}

CMagma::~CMagma()
{

}

void CMagma::InitN(unsigned __int64 Message)
{
	N1 = 0;
	for (int i = 0; i < 32; i++)
	{
		N1 <<= 1;
		if (Message & 1) N1++;
		Message >>= 1;
	}
	N2 = 0;
	for (int i = 0; i < 32; i++)
	{
		N2 <<= 1;
		if (Message & 1) N2++;
		Message >>= 1;
	}
}

void CMagma::InitResult(void)
{
	Result = 0;
	for (int i = 0; i < 32; i++)
	{
		Result <<= 1;
		if (N2 & 1) Result++;
		N2 >>= 1;
	}
	for (int i = 0; i < 32; i++)
	{
		Result <<= 1;
		if (N1 & 1) Result++;
		N1 >>= 1;
	}
}

void CMagma::EncryptFunction(char KeyNumber)
{
	CM1 = (unsigned __int64)N1 + X[KeyNumber];

	K.X = CM1;
	for (int i = 4; i > -1; i--)
	{
		K.Bytes[i] = SBox[i][K.Bytes[i]];
	}
	CM2 = K.X;

	CM2 = (CM2 << 11) | (CM2 >> 21);

	CM2 ^= N2;
	N2 = N1;
	N1 = CM2;
}

unsigned __int64 CMagma::StartNewSession(unsigned __int64 NewNonce = 0)
{
	Result = 0;
	if (NewNonce) Nonce = NewNonce;
	else Nonce++;
	Encrypt(Nonce);

	return Nonce;
}

unsigned __int64 CMagma::Encrypt(unsigned __int64 Message)
{
	OldResult = Result;
	Message ^= OldResult;

	InitN(Message);

	CM1 = 0;
	CM2 = 0;

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 8; j++)
			EncryptFunction(j);

	for (int j = 7; j >= 0; j--)
		EncryptFunction(j);

	N1 = N2;
	N2 = CM2;

	InitResult();

	return Result;
}

unsigned __int64 CMagma::Decrypt(unsigned __int64 Message)
{
	InitN(Message);

	CM1 = 0;
	CM2 = 0;

	for (int j = 0; j < 8; j++)
		EncryptFunction(j);

	for (int i = 0; i < 3; i++)
		for (int j = 7; j >= 0; j--)
			EncryptFunction(j);

	N1 = N2;
	N2 = CM2;
	OldResult = Result;
	InitResult();
	OldResult ^= Result;
	Result = Message;
	return OldResult;
}

bool CMagma::CheckMagma(unsigned __int64 CheckMessage, unsigned __int64 CheckNonce)
{
	Result = 0;
	Encrypt(CheckNonce);
	unsigned __int64 EncryptResult = Encrypt(CheckMessage);
	Result = 0;
	Encrypt(CheckNonce);
	unsigned __int64 MagmaResult = Decrypt(EncryptResult);
	if (MagmaResult ^ CheckMessage) return 0;
	else return 1;
}
