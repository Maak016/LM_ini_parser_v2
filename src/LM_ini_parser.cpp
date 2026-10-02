#include "LM_ini_parser.h"

//Table of content:
//- ini_key class
//- ini_section_reader class
//- LM_ini_parser class
//- ini_section_writer class
//- LM_ini_writer class
//- util class

///
// ini_key class space
///

string ini_key::convert_data_to_raw_line(const string key_name, const string key_value) {
	if (key_name.empty() || key_value.empty()) return "";

	return key_name + "=" + key_value;
}

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
// ini_section_reader class space
///

ini_section_reader::ini_section_reader(const string raw_section_data) {
	this->process_attributes(raw_section_data);
}

void ini_section_reader::process_attributes(const string raw_section_data) {
	this->raw_section_data = raw_section_data;

	this->parse_section_name();
	this->parse_keys_in_section();
}

void ini_section_reader::parse_section_name() {
	int closing_bracket_idx = 0;
	this->section_name = util::read_until_char(this->raw_section_data, ']', closing_bracket_idx);
}

void ini_section_reader::parse_keys_in_section() {
	string line_data   = "";
	int	   current_idx = 0;

	while (util::read_until_newline(this->raw_section_data, line_data, current_idx)) {
		ini_key curr_key(line_data);

		if (!curr_key.is_valid_key()) continue;

		this->keys_in_section.push_back(curr_key);
	}
}

string ini_section_reader::get_ini_key_val(const string key_name) {
	for (auto& key : this->keys_in_section) {
		if (key.get_key() == key_name) return key.get_val();
	}

	return "";
}

string ini_section_reader::get_section_name() {
	return this->section_name;
}

vector<ini_key> ini_section_reader::get_keys_in_section() {
	return this->keys_in_section;
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
	string raw_file_data = "";

	if (!util::read_from_file(&raw_file_data, this->file_path)) return;

	this->raw_file_data = raw_file_data;
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

	this->sections_in_file.push_back(ini_section_reader(next_section));

	this->iterate_single_section(idx_in_raw_data);
}

vector<ini_section_reader> LM_ini_parser::get_read_sections() {
	return this->sections_in_file;
}

string LM_ini_parser::get(const string section_name, const string key_name) {
	for (auto& section : this->sections_in_file) {
		if (section.get_section_name() == section_name) return section.get_ini_key_val(key_name);
	}

	return "";
}


///
// ini_section_writer class space
///

string ini_section_writer::get_header_line(const string section_name) {
	if (section_name.empty()) return "";
	return "[" + section_name + "]";
}

void ini_section_writer::append_key_to_written_data(string* buf, ini_key& key) {
	if (buf->empty()) return;	//if the buf string is empty (meaning nothing has been written to it) the header containing the section name has not been written

	*buf += '\n';

	string written_line = key.get_key() + "=" + key.get_val();

	*buf += written_line;
}

ini_section_writer::ini_section_writer(ini_section_reader* read_section) {
	this->adapt_from_read_section(read_section);
}

ini_section_writer::ini_section_writer(string new_section_name) {
	this->section_name = new_section_name;
}

void ini_section_writer::adapt_from_read_section(ini_section_reader* read_section) {
	this->section_name = read_section->get_section_name();

	vector<ini_key> keys_from_section = read_section->get_keys_in_section();

	this->keys_in_section = keys_from_section;
}

void ini_section_writer::write_key(const string key_name, const string key_val) {
	if (key_name.empty() || key_val.empty()) return;

	string raw_string = ini_key::convert_data_to_raw_line(key_name, key_val);

	if (this->get_key_index(key_name) != -1) {
		this->keys_in_section[get_key_index(key_name)] = ini_key(raw_string);
		return;
	}

	this->keys_in_section.push_back(ini_key(raw_string));
}

void ini_section_writer::parse_written_data() {
	string written_data = "";

	string header = ini_section_writer::get_header_line(this->section_name);

	written_data += header;

	for (auto& key : this->keys_in_section) {
		this->append_key_to_written_data(&written_data, key);
	}

	this->written_data = written_data;
}

string ini_section_writer::get_written_data() {
	return this->written_data;
}

string ini_section_writer::get_section_name() {
	return this->section_name;
}

int ini_section_writer::get_key_index(const string key_name) {
	for (int i = 0; i < this->keys_in_section.size(); i++) {
		if (this->keys_in_section[i].get_key() == key_name) return i;
	}
	
	return -1;
}


///
// LM_ini_writer class space
///

void LM_ini_writer::append_section_to_written_data(string* buf, ini_section_writer& writer) {
	*buf += '\n';

	writer.parse_written_data();
	*buf += writer.get_written_data();
}

LM_ini_writer::LM_ini_writer(filesystem::path file_path, LM_ini_parser* read_file) {
	if (filesystem::is_directory(file_path)) return;

	this->file_path = file_path.string();
	
	if (read_file) this->adapt_from_read_file(read_file);
}

void LM_ini_writer::adapt_from_read_file(LM_ini_parser* read_file) {
	vector<ini_section_reader> sections_from_file = read_file->get_read_sections();

	for (auto& read_section : sections_from_file) {
		this->sections_in_file.push_back(ini_section_writer(&read_section));
	}
}

void LM_ini_writer::add(const string section_name, const string key_name, const string key_val) {
	if (this->get_section_index(section_name) == -1) {
		this->add_section(section_name);
	}

	this->write_key(get_section_index(section_name), key_name, key_val);
}

void LM_ini_writer::add_section(const string section_name) {
	this->sections_in_file.push_back(ini_section_writer(section_name));
}

void LM_ini_writer::write_key(const int section_index, const string key_name, const string key_val) {
	ini_section_writer& instance = this->sections_in_file.at(section_index);

	instance.write_key(key_name, key_val);
}

int LM_ini_writer::get_section_index(const string section_name) {
	for (int i = 0; i < this->sections_in_file.size(); i++) {
		if (sections_in_file[i].get_section_name() == section_name) return i;
	}

	return -1;
}

void LM_ini_writer::parse_written_data() {
	string written_data = "";

	for (auto& section : this->sections_in_file) {
		this->append_section_to_written_data(&written_data, section);
	}

	this->written_data = written_data;
}

void LM_ini_writer::write() {
	this->parse_written_data();

	if (!util::write_to_file(this->file_path, this->written_data))
		std::cerr << "LM_ini_parser_v2 error: Could not write data to ini file at " << this->file_path << std::endl;
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

	for (int& i = first_valid_idx; i < input.length(); i++) {
		if (!util::is_whitespace(input[i])) break;
	}

	for (int& i = last_valid_idx; i >= 0; i--) {
		if (!util::is_whitespace(input[i])) break;
	}

	trim_result = trim_result.substr(first_valid_idx, (last_valid_idx - first_valid_idx) + 1);

	return trim_result;
}

bool util::write_to_file(const string file_path, const string& write_data) {
	std::ofstream output_file(file_path);

	if (!output_file.is_open()) {
		std::cerr << "LM_ini_parser_v2 error: Failed to open ini file in " << file_path << std::endl;
		return false;
	}

	output_file << write_data;

	output_file.close();

	return true;
}

bool util::read_from_file(string* output, const string file_path) {
	std::ifstream input_file(file_path);

	if (!input_file.is_open()) {
		std::cerr << "LM_ini_parser_v2 error: Failed to open ini file in " << file_path << std::endl;
		return false;;
	}

	std::stringstream data_stream;
	data_stream << input_file.rdbuf();

	*output = data_stream.str();

	data_stream.clear();
	input_file.close();

	return true;
}