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

    virtual void add_file_to_parse(std::string file_name);
    virtual void parse_mission_list_file(std::string file_name);
    virtual void parse_always_run_file();
    virtual void parse_new_files();
    virtual void parse_all_files();
    virtual void reset_to_first_mission();
    virtual bool new_mission_available();
    virtual uint8_t get_current_mission_index();
    virtual std::string get_current_mission_name();
    virtual bool jump_to_mission(uint8_t mission_index);
    virtual std::shared_ptr<Root> get_next_mission();
    virtual void print_mission_tree_at_index(uint8_t mission_index);
    virtual void print_all_registered_missions();
protected:
    virtual void print_mission_tree(std::shared_ptr<Command> tree, uint8_t num_spaces);
    virtual std::unique_ptr<custom_interfaces::msg::Pose6D> make_waypoint(json parameters_field);
    virtual std::unique_ptr<custom_interfaces::msg::Pose6DMask> make_waypoint_mask(json parameters_field);
    virtual std::unique_ptr<custom_interfaces::msg::Tolerance6D> make_tolerance(json parameters_field);
    virtual std::array<uint8_t,16> string_to_array(std::string string);
    virtual std::map<std::string,std::pair<bool,std::shared_ptr<Command>>> parse_tree(json tree, std::map<std::string,std::pair<bool,std::shared_ptr<Command>>> registry);

    std::queue<std::string> registered_mission_files;
    std::queue<std::string> mission_files_not_yet_parsed;
    std::vector<Mission> parsed_missions;
    uint8_t next_mission_index;
};

#endif // MISSION_FILE_PARSER_HPP
