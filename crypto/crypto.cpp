#include "class_crypto.h"

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		cout << "\nHELP for crypto (v1)\n";
		cout << "\nSupported encryption methods:\n";
		cout << "Symmetric encryption - GOST 28147-89 (Magma)\n";
		cout << "EDS - RSA 1024\n";
		cout << "HASH - RipeMD 160\n";
		cout << "\nThe function prototype:\n";
		cout << "crypto <:mode:> <filename> <:rsa_mode:> <sym_pass>]\n";
		cout << "<:mode:> = [encrypt | decrypt]\n";
		cout << "<:rsa_mode:> = [with_new_rsa_pass | with_old_rsa_pass]\n";
		return 0;
	}
	else
		if (argc == 5)
		{
			class_crypto func;
			switch (func.f_crypto(argv[1], argv[2], argv[3], argv[4]))
			{
			case 0:
				cout << "\nSuccessfull!\n";
				return 0;
			case 1:
				cout << "\nFile not found!\n";
				return 1;
			case 2:
				cout << "\nRSA keys not found!\n";
				cout << "Please, put file MyKey.eds into this directory!\n";
				return 2;
			case 3:
				cout << "\nHash is uncorrect!\n";
				return 3;
			case 4:
				cout << "\nUncorrect mode!\n";
				cout << "For more information call HELP:\n";
				cout << "crypto (without arguments)\n";
				return 3;
			};
			return 0;
		}
		else
		{
			cout << "\nError in the function call:\n";
			cout << "Incorrect number of arguments.\n";
			cout << "For more information call HELP:\n";
			cout << "crypto (without arguments)\n";
			return 1;
		}

}
