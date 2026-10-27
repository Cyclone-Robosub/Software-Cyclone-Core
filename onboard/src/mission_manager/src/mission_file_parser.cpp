#include "mission_file_parser.hpp"

#include <fstream>
#include <iostream>
#include <map>

Mission::Mission(std::shared_ptr<Root> root, std::string name) :
    root(root),
    name(name) {
    // intentionally blank
}

std::shared_ptr<Root> Mission::get_mission() {
    return root;
}

std::string Mission::get_name() {
    return name;
}

MissionFileParser::MissionFileParser(std::string mission_folder_path) : next_mission_index(0), mission_folder_path(mission_folder_path) {
    // Intentionally left blank
}

void MissionFileParser::add_file_to_parse(std::string file_name) {
    mission_files_not_yet_parsed.push(file_name);
    registered_mission_files.push(file_name);
}

void MissionFileParser::add_from_mission_list_file(std::string file_name) {
    // TODO: more robust path handling
    auto file = std::ifstream(mission_folder_path + file_name);
    if (!file.good()) {
        throw std::invalid_argument("Mission list '" + file_name + "' not recognized!\n");
    }
    std::string mission_file;
    while (getline(file, mission_file)) {
        if (mission_file.size() == 0 || mission_file[0] == '#') { continue; }
        add_file_to_parse(mission_file);
    }
}

void MissionFileParser::add_from_always_run_file() {
    add_from_mission_list_file("always_run.txt");
}

void MissionFileParser::parse_all_files() {
    parsed_missions.clear();
    mission_files_not_yet_parsed = registered_mission_files;
    parse_new_files();
}

void MissionFileParser::reset_to_first_mission() {
    next_mission_index = 0;
}

bool MissionFileParser::new_mission_available() {
    return next_mission_index < parsed_missions.size();
}

uint8_t MissionFileParser::get_current_mission_index() {
    return (next_mission_index == 0) ? 0 : (next_mission_index - 1);
}

std::string MissionFileParser::get_current_mission_name() {
    return parsed_missions[get_current_mission_index()].get_name();
}

bool MissionFileParser::jump_to_mission(uint8_t mission_index) {
    if (mission_index >= parsed_missions.size()) {
        return false;
    }
    next_mission_index = mission_index;
    return true;
}

std::shared_ptr<Root> MissionFileParser::get_next_mission() {
    return parsed_missions[next_mission_index++].get_mission();
}

void MissionFileParser::print_mission_tree_at_index(uint8_t mission_index) {
    if (mission_index >= parsed_missions.size()) {
        // TODO better error handling
        return;
    }
    print_mission_tree(parsed_missions[mission_index].get_mission(), 0);
}

void MissionFileParser::print_mission_tree(std::shared_ptr<Command> tree, uint8_t num_spaces) {
    std::cout << tree->get_name() << "\n";
    if (!tree->get_next()->mission_complete()) {
        for (int i = 0; i < num_spaces; i++) {
            std::cout << " ";
        }
        std::cout << "|--next: ";
        print_mission_tree(tree->get_next(), num_spaces + 3);
    }

    if (!tree->get_failure()->mission_complete()) {
        for (int i = 0; i < num_spaces; i++) {
            std::cout << " ";
        }
        std::cout << "|--fail: ";
        print_mission_tree(tree->get_failure(), num_spaces + 3);
    }
}

void MissionFileParser::print_all_registered_missions() {
    auto files_to_parse_copy = registered_mission_files;
    while (!files_to_parse_copy.empty()) {
        std::cout << registered_mission_files.front() << "\n";
        files_to_parse_copy.pop();
    }
}

void MissionFileParser::parse_new_files() {
    global_definitions.clear();
    parse_global_definitions();
    std::string file_name;
    while (!mission_files_not_yet_parsed.empty()) {
        file_name = mission_files_not_yet_parsed.front();
        auto file = std::ifstream(mission_folder_path + file_name);
        if (!file.good()) {
            throw std::invalid_argument("Mission file '" + file_name + "' not recognized!\n");
        }

        std::string reference;
        std::string action;
        std::string global_reference;
        json parameters;
        std::optional<std::array<uint8_t,16>> trick_id;
        std::shared_ptr<Command> command_entry;

        try {
            json mission_file_data = json::parse(file);

            auto command_registry = std::map<std::string,std::pair<bool,std::shared_ptr<Command>>>();

            // TODO potential cool feature: allow override of global params by setting locally (e.g. provide different timeout)
            for (json command : mission_file_data["commands"]) {
                /* This must come before the global reference check to ensure global commands are given the correct name. */
                reference = command["reference"];
                /* Check if this is a globally-scoped definition: if so, grab it from the global command definitions */
                if (command["global_reference"] != nullptr) {
                    global_reference = command["global_reference"];
                    command = global_definitions[global_reference];
                    if (command == nullptr) {
                        throw std::runtime_error("Reference to global command " + global_reference + " not found in global command definitions!\n");
                    }
                }

                action = command["action"];
                parameters = command["parameters"];
                if (parameters["trick_id"] == nullptr) {
                    trick_id = std::nullopt;
                } else {
                    trick_id = string_to_array(parameters["trick_id"]);
                }
                command_entry = nullptr;

                if (action == "drive_to_world_waypoint") {
                    command_entry = std::make_shared<DriveToWorldWaypoint>(
                        reference,
                        make_waypoint(parameters["goal_waypoint"]),
                        make_waypoint_mask(parameters["waypoint_mask"]),
                        make_tolerance(parameters["tolerance"]),
                        parameters["hold_time"],
                        parameters["timeout"],
                        trick_id
                    );
                } else if (action == "drive_to_world_waypoint_seeking") {
                    command_entry = std::make_shared<DriveToWorldWaypointSeeking>(
                        reference,
                        make_waypoint(parameters["goal_waypoint"]),
                        make_waypoint_mask(parameters["waypoint_mask"]),
                        string_to_array(parameters["object_id"]),
                        parameters["confidence_required"],
                        parameters["timeout"],
                        trick_id
                    );
                } else if (action == "idle") {
                    if (trick_id.has_value()) {
                        throw std::runtime_error("trick_id provided for idle, but idle doesn't accept trick_id\n");
                    }
                    command_entry = std::make_shared<Idle>(
                        reference,
                        parameters["duration"]
                    );
                } else if (action == "track_object_waypoint") {
                    command_entry = std::make_shared<TrackObjectWaypoint>(
                        reference,
                        make_waypoint(parameters["tracking_position"]),
                        make_waypoint_mask(parameters["waypoint_mask"]),
                        make_tolerance(parameters["tolerance"]),
                        string_to_array(parameters["object_id"]),
                        parameters["hold_time"],
                        parameters["timeout"],
                        trick_id
                    );
                } else if (action == "duration_trick") {
                    if (!trick_id.has_value()) {
                        throw std::runtime_error("trick_id not provided for duration_trick\n");
                    }
                    command_entry = std::make_shared<DurationTrick>(
                        reference,
                        make_waypoint_mask(parameters["waypoint_mask"]),
                        make_tolerance(parameters["tolerance"]),
                        trick_id.value(),
                        parameters["hold_time"],
                        parameters["duration"],
                        parameters["timeout"]
                    );
                } else if (action == "distance_trick") {
                    if (!trick_id.has_value()) {
                        throw std::runtime_error("trick_id not provided for distance_trick\n");
                    }
                    command_entry = std::make_shared<DistanceTrick>(
                        reference,
                        make_waypoint(parameters["destination_position"]),
                        make_waypoint_mask(parameters["waypoint_mask"]),
                        make_tolerance(parameters["tolerance"]),
                        trick_id.value(),
                        parameters["hold_time"],
                        parameters["timeout"]
                    );
                } else {
                    throw std::runtime_error("Command name " + action + " does not match any known command\n");
                }
                command_registry[reference] = {false, command_entry};
            }
            json tree = mission_file_data["tree"];
            command_registry = parse_tree(tree, command_registry);
            auto root = std::make_shared<Root>();
            root->set_next(command_registry[tree["ref"]].second);
            parsed_missions.push_back(Mission(root,file_name));
            mission_files_not_yet_parsed.pop();
        } catch (const std::exception& e) {
            throw std::runtime_error("Was on command \"" + reference + "\" and failed to parse mission file \"" + file_name + "\": " + e.what());
        }
    }
}

std::map<std::string,std::pair<bool,std::shared_ptr<Command>>> MissionFileParser::parse_tree(json tree, std::map<std::string,std::pair<bool,std::shared_ptr<Command>>> registry) {
    auto updated_registry = registry;
    json ref = tree["ref"];
    json next = tree["next"];
    json failure = tree["failure"];

    if (next != nullptr) {
        updated_registry = parse_tree(next, updated_registry);
        // Prevent double-defined tree nodes
        if (!updated_registry[ref].first) {
            updated_registry[next["ref"]].first = true;
            updated_registry[ref].second->set_next(updated_registry[next["ref"]].second);
        } else {
            throw std::runtime_error("Uh oh, bad tree! You defined a node twice. Don't do that!\n");
        }
    }
    if (failure != nullptr) {
        updated_registry = parse_tree(failure, updated_registry);
        // Prevent double-defined tree nodes
        if (!updated_registry[ref].first) {
            updated_registry[failure["ref"]].first = true;
            updated_registry[ref].second->set_failure(updated_registry[failure["ref"]].second);
        } else {
            throw std::runtime_error("Uh oh, bad tree! You defined a node twice. Don't do that!\n");
        }
    }
    return updated_registry;
}

void MissionFileParser::parse_global_definitions() {
    std::string file_name = "global_command_definitions.json";
    auto file = std::ifstream(mission_folder_path + file_name);
    if (!file.good()) {
        throw std::invalid_argument("Mission file '" + file_name + "' not recognized!\n");
    }
    json commands = json::parse(file)["commands"];
    for (json command : commands) {
        global_definitions[command["reference"]] = command;
    }

}

std::unique_ptr<custom_interfaces::msg::Pose6D> MissionFileParser::make_waypoint(json parameters_field) {
    auto waypoint = std::make_unique<custom_interfaces::msg::Pose6D>();
    waypoint->x = parameters_field["x"];
    waypoint->y = parameters_field["y"];
    waypoint->z = parameters_field["z"];
    waypoint->roll = parameters_field["roll"];
    waypoint->pitch = parameters_field["pitch"];
    waypoint->yaw = parameters_field["yaw"];
    return waypoint;
}

std::unique_ptr<custom_interfaces::msg::Pose6DMask> MissionFileParser::make_waypoint_mask(json parameters_field) {
    auto waypoint_mask = std::make_unique<custom_interfaces::msg::Pose6DMask>();
    waypoint_mask->x = parameters_field["x"];
    waypoint_mask->y = parameters_field["y"];
    waypoint_mask->z = parameters_field["z"];
    waypoint_mask->roll = parameters_field["roll"];
    waypoint_mask->pitch = parameters_field["pitch"];
    waypoint_mask->yaw = parameters_field["yaw"];
    return waypoint_mask;
}

std::unique_ptr<custom_interfaces::msg::Tolerance6D> MissionFileParser::make_tolerance(json parameters_field) {
    auto tolerance = std::make_unique<custom_interfaces::msg::Tolerance6D>();
    tolerance->x = parameters_field["x"];
    if (tolerance->x == 0) {
        tolerance->x = 0.2;
    }
    tolerance->y = parameters_field["y"];
    if (tolerance->y == 0) {
        tolerance->y = 0.2;
    }
    tolerance->z = parameters_field["z"];
    if (tolerance->z == 0) {
        tolerance->z = 0.2;
    }
    tolerance->roll = parameters_field["roll"];
    if (tolerance->roll == 0) {
        tolerance->roll = 0.1;
    }
    tolerance->pitch = parameters_field["pitch"];
    if (tolerance->pitch == 0) {
        tolerance->pitch = 0.1;
    }
    tolerance->yaw = parameters_field["yaw"];
    if (tolerance->yaw == 0) {
        tolerance->yaw = 0.1;
    }
    return tolerance;
}

std::array<uint8_t,16> MissionFileParser::string_to_array(std::string string) {
    std::array<uint8_t,16> array = {0};
    if (string.size() > 16) {
        throw std::runtime_error("Object ID too long! Unable to parse, returning empty.\n");
        return array;
    }
    for (int i = 0; i < 16; i++) {
        array[i] = string[i];
    }
    return array;
}
 