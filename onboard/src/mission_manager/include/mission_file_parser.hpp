#ifndef MISSION_FILE_PARSER_HPP
#define MISSION_FILE_PARSER_HPP

#include <robot_command.hpp>
#include <queue>
#include <exception>

#include "json.hpp"

using json = nlohmann::json;

class Mission {
public:
    Mission(std::shared_ptr<Root> root, std::string name);
    std::shared_ptr<Root> get_mission();
    std::string get_name();
private:
    std::shared_ptr<Root> root;
    std::string name;
};

class MissionFileParser {
public:
    MissionFileParser();

    void add_file_to_parse(std::string file_name);
    void parse_mission_list_file(std::string file_name);
    void parse_always_run_file();
    void parse_new_files();
    void parse_all_files();
    void reset_to_first_mission();
    bool new_mission_available();
    uint8_t get_current_mission_index();
    std::string get_current_mission_name();
    bool jump_to_mission(uint8_t mission_index);
    std::shared_ptr<Root> get_next_mission();
    void print_mission_tree_at_index(uint8_t mission_index);
protected:
    void print_mission_tree(std::shared_ptr<Command> tree, uint8_t num_spaces);
    std::unique_ptr<custom_interfaces::msg::Pose6D> make_waypoint(json parameters_field);
    std::unique_ptr<custom_interfaces::msg::Pose6DMask> make_waypoint_mask(json parameters_field);
    std::unique_ptr<custom_interfaces::msg::Tolerance6D> make_tolerance(json parameters_field);
    std::array<uint8_t,16> string_to_array(std::string string);
    std::map<std::string,std::pair<bool,std::shared_ptr<Command>>> parse_tree(json tree, std::map<std::string,std::pair<bool,std::shared_ptr<Command>>> registry);

    std::queue<std::string> files_to_parse;
    std::queue<std::string> files_not_yet_parsed;
    std::vector<Mission> parsed_missions;
    uint8_t next_mission_index;
};

#endif // MISSION_FILE_PARSER_HPP
