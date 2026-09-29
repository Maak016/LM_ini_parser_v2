#include "LM_ini_parser.h"

//Table of content:
//- ini_key class
//- ini_section class
//- LM_ini_parser class
//- util class

///
// ini_key class space
///

int ini_key::get_equal_sign_index(const string raw_line_data) {
	for (int i = 0; i < raw_line_data.length(); i++) {
		if (raw_line_data[i] == '=') return i;
	}

	return -1;
}

bool ini_key::is_valid_line(const string raw_line_data) {
	if (raw_line_data.empty()) return false;

	if (ini_key::get_equal_sign_index(raw_line_data) >= raw_line_data.length() - 1) return false;
	if (ini_key::get_equal_sign_index(raw_line_data) <= 0) return false;

	if (raw_line_data[0] == '[' || raw_line_data[0] == ';' || raw_line_data[0] == '#') return false;

	return true;
}

bool ini_key::is_valid_key() {
	return this->is_valid;
}

ini_key::ini_key(const string raw_line_data) {
	this->assign_key_value_pair(raw_line_data);
}

void ini_key::assign_key_value_pair(const string raw_line_data) {
	if (!ini_key::is_valid_line(raw_line_data)) {
		this->is_valid = false;
		return;
	}

	int eq_sign_index = ini_key::get_equal_sign_index(raw_line_data);

	this->raw_line_data = raw_line_data;

	assign_key(eq_sign_index);
	assign_val(eq_sign_index);

	this->is_valid = true;
}

void ini_key::assign_key(int eq_sign_index) {
	string key_name = this->raw_line_data.substr(0, eq_sign_index);

	this->key_name = util::trim_whitespace(key_name);
}

void ini_key::assign_val(int eq_sign_index) {
	string key_value = this->raw_line_data.substr(eq_sign_index + 1);

	this->key_value = util::trim_whitespace(key_value);
}

string ini_key::get_key() {
	return this->key_name;
}

string ini_key::get_val() {
	return this->key_value;
}

///
// ini_section class space
///

ini_section::ini_section(const string raw_section_data) {
	this->process_attributes(raw_section_data);
}

void ini_section::process_attributes(const string raw_section_data) {
	this->raw_section_data = raw_section_data;

	this->parse_section_name();
	this->parse_keys_in_section();
}

void ini_section::parse_section_name() {
	int closing_bracket_idx = 0;
	this->section_name = util::read_until_char(this->raw_section_data, ']', closing_bracket_idx);
}

void ini_section::parse_keys_in_section() {
	string line_data   = "";
	int	   current_idx = 0;

	while (util::read_until_newline(this->raw_section_data, line_data, current_idx)) {
		ini_key curr_key(line_data);

		if (!curr_key.is_valid_key()) continue;

		this->keys_in_section.push_back(curr_key);
	}
}

string ini_section::get_ini_key_val(const string key_name) {
	for (auto& key : this->keys_in_section) {
		if (key.get_key() == key_name) return key.get_val();
	}

	return "";
}

string ini_section::get_section_name() {
	return this->section_name;
}


///
// LM_ini_parser class space
///

LM_ini_parser::LM_ini_parser(filesystem::path file_path) {
	this->process_attributes(file_path);
}

void LM_ini_parser::process_attributes(filesystem::path file_path) {
	if (!filesystem::exists(file_path) || filesystem::is_directory(file_path)) return;

	this->file_path = file_path.string();

	this->parse_raw_file_data();

	this->parse_sections_in_file();
}

void LM_ini_parser::parse_raw_file_data() {
	std::ifstream input_file(this->file_path);

	if (!input_file.is_open()) {
		std::cerr << "LM_ini_parser_v2 error: Failed to open ini file in " << this->file_path << std::endl;
		return;
	}

	std::stringstream data_stream;
	data_stream << input_file.rdbuf();

	this->raw_file_data = data_stream.str();

	data_stream.clear();
	input_file.close();
}

void LM_ini_parser::parse_sections_in_file() {
	int first_bracket_opening = 0;
	util::read_until_char(this->raw_file_data, '[', first_bracket_opening);

	this->iterate_single_section(first_bracket_opening);
}

void LM_ini_parser::iterate_single_section(int idx_in_raw_data) {
	if (idx_in_raw_data >= this->raw_file_data.length()) return;

	idx_in_raw_data++;
	string next_section = util::read_until_char(this->raw_file_data, '[', idx_in_raw_data);

	this->sections_in_file.push_back(ini_section(next_section));

	this->iterate_single_section(idx_in_raw_data);
}

string LM_ini_parser::get(const string section_name, const string key_name) {
	for (auto& section : this->sections_in_file) {
		if (section.get_section_name() == section_name) return section.get_ini_key_val(key_name);
	}

	return "";
}


///
// util class space
///

string util::read_until_char(const string& input, char finding_target, int& ref_index) {
	string read_result = "";

	for (int& i = ref_index; i < input.length(); i++) {
		if (input[i] == finding_target) break;

		read_result += input[i];
	}

	return read_result;
}

bool util::read_until_newline(const string& input, string& output, int& ref_index) {
	if (ref_index >= input.length()) return false;

	output = "";

	for (int& i = ref_index; i < input.length(); i++) {
		if (input[i] == '\n') break;

		output += input[i];
	}

	ref_index++;

	return true;
}

bool util::is_whitespace(const char input) {
	if (input == ' ' || input == '\t' || input == '\r') return true;
	return false;
}

string util::trim_whitespace(const string& input) {
	string trim_result = input;

	int first_valid_idx = 0;
	int last_valid_idx = input.length() - 1;

	for (int i = first_valid_idx; i < input.length(); i++) {
		if (!util::is_whitespace(input[i])) break;
	}

	for (int i = last_valid_idx; i >= 0; i--) {
		if (!util::is_whitespace(input[i])) break;
	}

	trim_result = trim_result.substr(first_valid_idx, (last_valid_idx - first_valid_idx) + 1);

	return trim_result;
}