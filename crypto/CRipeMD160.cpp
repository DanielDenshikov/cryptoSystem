#include "CRipeMD160.h"
#include <stdio.h>

UINT f0(UINT x, UINT y, UINT z) { return x ^ y ^ z; }
UINT f1(UINT x, UINT y, UINT z) { return (x & y) | (~x & z); }
UINT f2(UINT x, UINT y, UINT z) { return (x | ~y) ^ z; }
UINT f3(UINT x, UINT y, UINT z) { return (x & z) | (y & ~z); }
UINT f4(UINT x, UINT y, UINT z) { return x ^ (y | ~z); }
pFunc f[5] = { f0, f1, f2, f3, f4 };

unsigned int inv(unsigned int name)
{
	unsigned int res = 0;

	res |= ((name >> 0) & 0xFF) << 24;
	res |= ((name >> 8) & 0xFF) << 16;
	res |= ((name >> 16) & 0xFF) << 8;
	res |= ((name >> 24) & 0xFF) << 0;

	return res;
}

char tohex(char A)
{
	switch (A)
	{
	case 0: return '0';
	case 1: return '1';
	case 2: return '2';
	case 3: return '3';
	case 4: return '4';
	case 5: return '5';
	case 6: return '6';
	case 7: return '7';
	case 8: return '8';
	case 9: return '9';
	case 10: return 'a';
	case 11: return 'b';
	case 12: return 'c';
	case 13: return 'd';
	case 14: return 'e';
	case 15: return 'f';
	default: return 'n';
	}
}

CRipeMD160::CRipeMD160()
{

}

CRipeMD160::~CRipeMD160()
{

}

unsigned __int64 CRipeMD160::strconvert(char* str)
{
	unsigned __int64 strlength = 0;
	while (str[strlength]) strlength++;

	unsigned __int64 alllength = strlength + 8;
	unsigned __int64 Xnumrows = alllength / 64;
	if (alllength % 64) Xnumrows++;

	X = new unsigned int*[Xnumrows];
	for (int i = 0; i < Xnumrows; i++)
		X[i] = new unsigned int[16];
	SCharstoint TRANSLATE;
	int i = 0;
	for (int row = 0; row < Xnumrows - 1; row++)
	{
		for (int column = 0; column < 16; column++)
		{
			for (int k = 0; k < 4; k++)	TRANSLATE.chars[k] = str[i++];
			X[row][column] = TRANSLATE.resint;
		}
	}

	for (int column = 0; column < 14; column++)
	{
		for (int k = 0; k < 4; k++)
			TRANSLATE.chars[k] = (i == strlength) ? (i++ - i + 128) : ((i < strlength) ? str[i++] : 0);
		X[Xnumrows - 1][column] = TRANSLATE.resint;
	}
	strlength *= 8;
	X[Xnumrows - 1][14] = strlength;
	X[Xnumrows - 1][15] = strlength >> 32;

	return Xnumrows;
}

char* CRipeMD160::GetHASH(char *str)
{
	unsigned __int64 Xnumrows = strconvert(str);

	unsigned int A0, B0, C0, D0, E0;
	unsigned int A1, B1, C1, D1, E1;
	unsigned int T;


	for (int i = 0; i < Xnumrows; i++)
	{
		A0 = (A1 = h[0]);
		B0 = (B1 = h[1]);
		C0 = (C1 = h[2]);
		D0 = (D1 = h[3]);
		E0 = (E1 = h[4]);
		for (int round = 0; round < 5; round++)
		{
			for (int j = 0; j < 16; j++)
			{
				T = A0 + f[round](B0, C0, D0) + X[i][Selections_R0[round][j]] + K0[round];
				T = (T << Shifts_S0[round][j]) | (T >> (32 - Shifts_S0[round][j]));
				T += E0;
				A0 = E0;
				E0 = D0;
				D0 = (C0 << 10) | (C0 >> 22);
				C0 = B0;
				B0 = T;

				T = A1 + f[4 - round](B1, C1, D1) + X[i][Selections_R1[round][j]] + K1[round];
				T = (T << Shifts_S1[round][j]) | (T >> (32 - Shifts_S1[round][j]));
				T += E1;
				A1 = E1;
				E1 = D1;
				D1 = (C1 << 10) | (C1 >> 22);
				C1 = B1;
				B1 = T;
			}
		}
		T = h[1] + C0 + D1;
		h[1] = h[2] + D0 + E1;
		h[2] = h[3] + E0 + A1;
		h[3] = h[4] + A0 + B1;
		h[4] = h[0] + B0 + C1;
		h[0] = T;
	}
	if (X)
	{
		for (int i = 0; i < Xnumrows; i++)
		{
			delete[] X[i];
		}
		delete[] X;
	}
	SCharstoint CONVERT;
	char* HASH = new char[41];
	for (int i = 0; i < 5; i++)
	{
		//h[i] = inv(h[i]);
		CONVERT.resint = h[i];
		for (int k = 0; k < 4; k++)
		{
			HASH[i * 8 + k * 2] = tohex(CONVERT.chars[k] >> 4);
			HASH[i * 8 + k * 2 + 1] = tohex(CONVERT.chars[k] & 0x0F);
		}

	}
	HASH[40] = 0;

	return HASH;
}
