#include "LM_ini_parser.h"

int main() {
	LM_ini_parser reader("example_ini/steam_emu.ini");

	std::cout << reader.get("Interfaces", "SteamController") << std::endl;

	return 0;
}