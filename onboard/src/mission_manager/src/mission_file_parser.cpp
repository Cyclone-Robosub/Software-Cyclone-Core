#include "mission_file_parser.hpp"

MissionFileParser::MissionFileParser() {
    // TODO
}

void MissionFileParser::parse_files() {
    // TODO
}

std::vector<std::shared_ptr<Root>> MissionFileParser::get_parsed_missions() {
    // TODO
    auto missions = std::vector<std::shared_ptr<Root>>();
    missions.push_back(std::make_shared<Root>());
    return missions;
}
 