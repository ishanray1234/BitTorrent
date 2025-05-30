#ifndef TORRENT_PARSER_H
#define TORRENT_PARSER_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

auto decode_bencoded_value(const std::string& encoded_value, size_t& pos) -> json;
auto decode_dictionary(const std::string& encoded_value, size_t& pos) -> json;
auto encode_bencode(const json& j) -> std::string;

#endif // BENCODE_DECODER_H
