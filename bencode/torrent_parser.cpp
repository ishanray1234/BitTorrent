#include <iostream>
#include <string>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std;

auto encode_bencode(const json& j) -> std::string;
auto decode_bencoded_value(const std::string& encoded_value, size_t& pos) -> json;

std::string encode_bencode(const json& j) {
    std::ostringstream oss;

    if (j.is_string()) {
        const std::string& str = j.get<std::string>();
        oss << str.length() << ":" << str;
    }
    else if (j.is_number_integer()) {
        oss << "i" << j.get<int64_t>() << "e";
    }
    else if (j.is_array()) {
        oss << "l";
        for (const auto& item : j) {
            oss << encode_bencode(item);
        }
        oss << "e";
    }
    else if (j.is_object()) {
        oss << "d";
        // Bencode requires keys in dictionaries to be sorted
        std::vector<std::string> keys;
        for (auto it = j.begin(); it != j.end(); ++it) {
            keys.push_back(it.key());
        }
        std::sort(keys.begin(), keys.end());

        for (const auto& key : keys) {
            oss << key.length() << ":" << key;
            oss << encode_bencode(j.at(key));
        }
        oss << "e";
    }
    else {
        throw std::runtime_error("Unsupported JSON type for Bencode serialization.");
    }

    return oss.str();
}


auto decode_integer(const std::string& encoded_value, size_t& pos) -> int64_t {
    pos++;  // skip 'i'
    const auto end = encoded_value.find('e', pos);
    if (end == std::string::npos) throw std::runtime_error("Invalid bencode: missing 'e' for integer");
    
    const std::string number_string = encoded_value.substr(pos, end - pos);
    pos = end + 1;
    return std::strtoll(number_string.c_str(), nullptr, 10);
}

auto decode_string_length(const std::string& encoded_value, const size_t pos, size_t& colon_index) -> int64_t
{
    colon_index = encoded_value.find(':', pos);
    const std::string number_string = encoded_value.substr(pos, colon_index);
    const int64_t length = std::strtoll(number_string.c_str(), nullptr, 10);
    return length;
}
auto decode_string(const std::string& encoded_value, size_t& pos) -> std::string
{
    size_t colon_index = 0;
    const int64_t length = decode_string_length(encoded_value, pos, colon_index);
    std::string str = encoded_value.substr(colon_index + 1, length);
    pos = colon_index + length + 1;
    return str;
}
auto decode_list(const std::string& encoded_value, size_t& pos) -> json
{
    json list = json::array();
    pos++;
    while (encoded_value[pos] != 'e')
    {
        list.push_back(decode_bencoded_value(encoded_value, pos));
    }
    pos++;
    return list;
}
auto decode_dictionary(const std::string& encoded_value, size_t& pos) -> json
{
    json dictionary = json::object();
    pos++;
    while (encoded_value[pos] != 'e')
    {
        const auto key = decode_string(encoded_value, pos);
        const auto value = decode_bencoded_value(encoded_value, pos);
        dictionary[key] = value;
    }
    pos++;
    return dictionary;
}
auto decode_bencoded_value(const std::string& encoded_value, size_t& pos) -> json
{
    // string
    // Example: "5:hello" -> "hello"
    if (std::isdigit(encoded_value[pos]))
    {
        return json(decode_string(encoded_value, pos));
    }
    // Integer
    // Example: "i42e" -> 42
    if (encoded_value[pos] == 'i')
    {
        return json(decode_integer(encoded_value, pos));
    }
    // List
    // Example: "l5:helloi42ee" -> ["hello", 42] 
    if (encoded_value[pos] == 'l')
    {
        return decode_list(encoded_value, pos);
    }
    // Dictionary
    // Example: d3:foo3:bar5:helloi52ee -> {"foo":"bar","hello":52}
    if (encoded_value[pos] == 'd')
    {
        return decode_dictionary(encoded_value, pos);
    }
    throw std::runtime_error("Unhandled encoded value: " + encoded_value);
}
// auto main(int argc, char* argv[]) -> int
// {
//     // Flush after every std::cout / std::cerr
//     std::cout << std::unitbuf;
//     std::cerr << std::unitbuf;
//     if (argc < 2)
//     {
//         std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
//         return 1;
//     }
//     if (const std::string command = argv[1]; command == "decode")
//     {
//         if (argc < 3)
//         {
//             std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
//             return 1;
//         }
//         const std::string encoded_value = argv[2];
//         size_t pos = 0;
//         const json decoded_value = decode_bencoded_value(encoded_value, pos);
//         std::cout << decoded_value.dump() << std::endl;
//     }
//     else if (command == "info")
//     {
//         auto file_path = argv[2];
//         std::ifstream file(file_path);
//         std::ostringstream buffer;
//         buffer << file.rdbuf();
        
//         std::string content = buffer.str();
//         std::cout<< "Content read from file: " << file_path << std::endl;
//         size_t pos = 0;
//         auto dictionary = decode_dictionary(content, pos);
//         auto announce = dictionary["announce"].get<std::string>();
//         auto info = dictionary["info"];
//         auto length = info["length"];
//         auto name = info["name"];
//         auto piece_length = info["piece length"];
//         auto pieces = info["pieces"];
//         std::cout << "Name: " << name.get<std::string>() << std::endl;
//         std::cout << "Piece Length: " << piece_length.get<int>() << " bytes" << std::endl;
//         std::cout << "Pieces: " << pieces.get<std::string>() << std::endl;
//         std::cout << "Tracker URL: " << announce << std::endl;
//         std::cout << "Length: " << length << std::endl;

//         // test encoding
//         cout<<info<<endl;
//         std::cout << "Encoded Bencode: " << encode_bencode(info) << std::endl;
//     }
//     else
//     {
//         std::cerr << "unknown command: " << command << std::endl;
//         return 1;
//     }
//     return 0;
// }