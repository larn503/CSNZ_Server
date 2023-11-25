#pragma once

#include <string>
#include <vector>
#include <random>

uint32_t ip_string_to_int(const std::string& in, bool* const success = nullptr);
std::string ip_to_string(uint32_t in, bool* const success = nullptr);
bool isNumber(const std::string& str);
bool yesOrNo(float probabilityOfYes);
const char* FormatSeconds(int seconds);
char* va(const char* format, ...);
const char* WSAGetLastErrorString();
std::vector<std::string> deserialize_array_str(std::string const& csv);
std::vector<int> deserialize_array_int(std::string const& csv);
std::vector<unsigned char> deserialize_array_uchar(std::string const& csv);
std::string serialize_array_str(const std::vector<std::string>& arr);
std::string serialize_array_int(const std::vector<int>& arr);
std::string serialize_array_uchar(const std::vector<unsigned char>& arr);
size_t findCaseInsensitive(std::string data, std::string toSearch, size_t pos = 0);
size_t findCaseInsensitive(std::string data, const std::vector<std::string>& toSearch, size_t pos = 0);
std::vector<std::string> ParseArguments(const std::string& str);
void SleepMS(unsigned int ms);
int GetNetworkError();

class Randomer
{
private:
	std::mt19937 gen;
	std::uniform_int_distribution<size_t> dis;
public:
	inline Randomer(size_t max) : dis(0, max), gen(std::random_device()()) {}
	inline Randomer(size_t max, unsigned int seed) : dis(0, max), gen(seed) {}
	inline size_t operator()() { return dis(gen); }
	// if you want predictable numbers
	inline void SetSeed(unsigned int seed) { gen.seed(seed); }
};