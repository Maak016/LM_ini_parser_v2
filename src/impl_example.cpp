#include "LM_ini_parser.h"

int main() {
	LM_ini_parser reader("example_ini/steam_emu.ini");

	std::cout << reader.get("Interfaces", "SteamController") << std::endl;

	LM_ini_writer writer("example_ini/steam_emu_written.ini", &reader);

	writer.add("Interfaces", "SteamController", "idklolol");

	writer.write();

	return 0;
}