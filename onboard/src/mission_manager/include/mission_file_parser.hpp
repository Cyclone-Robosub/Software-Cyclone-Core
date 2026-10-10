#ifndef MISSION_FILE_PARSER_HPP
#define MISSION_FILE_PARSER_HPP

#include <robot_command.hpp>

class MissionFileParser {
public:
    MissionFileParser();

    void parse_files();
    std::vector<std::shared_ptr<Root>> get_parsed_missions();
    // TODO
};

#endif // MISSION_FILE_PARSER_HPP
