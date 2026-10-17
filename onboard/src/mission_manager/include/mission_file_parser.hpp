#ifndef MISSION_FILE_PARSER_HPP
#define MISSION_FILE_PARSER_HPP

#include <robot_command.hpp>

#include "json.hpp"

using json = nlohmann::json;

class MissionFileParser {
public:
    MissionFileParser();

    void add_file_to_parse(std::string file_name);
    void parse_new_files();
    void parse_all_files();
    void reset_to_first_mission();
    bool new_mission_available();
    uint8_t get_current_mission_index();
    bool jump_to_mission(uint8_t mission_index);
    std::shared_ptr<Root> get_next_mission();
    void print_mission_tree_at_index(uint8_t mission_index);
private:
    void parse_files(std::vector<std::string> files);
    void print_mission_tree(std::shared_ptr<Command> tree, uint8_t num_spaces);
    std::unique_ptr<custom_interfaces::msg::Pose6D> make_waypoint(json parameters_field);
    std::unique_ptr<custom_interfaces::msg::Pose6DMask> make_waypoint_mask(json parameters_field);
    std::unique_ptr<custom_interfaces::msg::Tolerance6D> make_tolerance(json parameters_field);
    std::array<uint8_t,16> string_to_array(std::string string);
    std::map<std::string,std::pair<bool,std::shared_ptr<Command>>> parse_tree(json tree, std::map<std::string,std::pair<bool,std::shared_ptr<Command>>> registry);

    std::vector<std::string> files_to_parse;
    std::vector<std::string> files_not_yet_parsed;
    std::vector<std::shared_ptr<Root>> parsed_missions;
    uint8_t next_mission_index;
};

#endif // MISSION_FILE_PARSER_HPP
