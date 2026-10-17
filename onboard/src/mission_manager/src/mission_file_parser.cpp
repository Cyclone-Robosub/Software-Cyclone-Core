#include "mission_file_parser.hpp"

#include <fstream>
#include <map>


MissionFileParser::MissionFileParser() {
    // TODO
}

void MissionFileParser::add_file_to_parse(std::string file_name) {
    files_not_yet_parsed.push_back(file_name);
    files_to_parse.push_back(file_name);
}

void MissionFileParser::parse_new_files() {
    parse_files(files_not_yet_parsed);
    files_not_yet_parsed.clear();
}

void MissionFileParser::parse_all_files() {
    parsed_missions.clear();
    parse_files(files_to_parse);
    files_not_yet_parsed.clear();
}

void MissionFileParser::reset_to_first_mission() {
    next_mission_index = 0;
}

bool MissionFileParser::new_mission_available() {
    return next_mission_index < parsed_missions.size();
}

bool MissionFileParser::jump_to_mission(uint8_t mission_index) {
    if (mission_index >= parsed_missions.size()) {
        return false;
    }
    next_mission_index = mission_index;
    return true;
}

std::shared_ptr<Root> MissionFileParser::get_next_mission() {
    return parsed_missions[next_mission_index++];
}

// TODO: add trick id if it exists
// TODO: handle bad parsing (maybe catch exception, report, abort)
void MissionFileParser::parse_files(std::vector<std::string> files) {
    for (std::string file_name : files) {
        // TODO: more robust path handling
        auto file = std::ifstream("onboard/src/mission_manager/mission_files/" + file_name);
        json mission_file_data = json::parse(file);

        auto command_registry = std::map<std::string,std::pair<bool,std::shared_ptr<Command>>>();

        for (auto command : mission_file_data["commands"]) {
            std::string reference = command["reference"];
            std::string action = command["action"];
            json parameters = command["parameters"];
            std::shared_ptr<Command> command_entry = nullptr;

            if (action == "drive_to_world_waypoint") {
                command_entry = std::make_shared<DriveToWorldWaypoint>(
                    make_waypoint(parameters["goal_waypoint"]),
                    make_waypoint_mask(parameters["waypoint_mask"]),
                    make_tolerance(parameters["tolerance"]),
                    parameters["hold_time"],
                    parameters["timeout"]
                );
            } else if (action == "drive_to_world_waypoint_seeking") {
                command_entry = std::make_shared<DriveToWorldWaypointSeeking>(
                    make_waypoint(parameters["goal_waypoint"]),
                    make_waypoint_mask(parameters["waypoint_mask"]),
                    string_to_array(parameters["object_id"]),
                    parameters["confidence_required"],
                    parameters["timeout"]
                );
            } else if (action == "idle") {
                command_entry = std::make_shared<Idle>(
                    parameters["duration"]
                );
            } else if (action == "track_object_waypoint") {
                command_entry = std::make_shared<TrackObjectWaypoint>(
                    make_waypoint(parameters["tracking_position"]),
                    make_waypoint_mask(parameters["waypoint_mask"]),
                    make_tolerance(parameters["tolerance"]),
                    string_to_array(parameters["object_id"]),
                    parameters["hold_time"],
                    parameters["timeout"]
                );
            } else if (action == "duration_trick") {
                command_entry = std::make_shared<DurationTrick>(
                    make_waypoint_mask(parameters["waypoint_mask"]),
                    make_tolerance(parameters["tolerance"]),
                    string_to_array(parameters["trick_id"]),
                    parameters["hold_time"],
                    parameters["duration"],
                    parameters["timeout"]
                );
            } else if (action == "distance_trick") {
                command_entry = std::make_shared<DistanceTrick>(
                    make_waypoint(parameters["destination_position"]),
                    make_waypoint_mask(parameters["waypoint_mask"]),
                    make_tolerance(parameters["tolerance"]),
                    string_to_array(parameters["trick_id"]),
                    parameters["hold_time"],
                    parameters["timeout"]
                );
            } else {
                // TODO: throw an exception here or something?
                fprintf(stderr, "Unknown command type!\n");
                continue;
            }
            
            command_registry[reference] = {false, command_entry};
        }
        json tree = mission_file_data["tree"];
        command_registry = parse_tree(tree, command_registry);
        auto root = std::make_shared<Root>();
        root->set_next(command_registry[tree["ref"]].second);
        parsed_missions.push_back(root);
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
            // TODO: better error handling
            fprintf(stderr, "Uh oh, bad tree! You defined a node twice. Don't do that!\n");
        }
    }
    if (failure != nullptr) {
        updated_registry = parse_tree(failure, updated_registry);
        // Prevent double-defined tree nodes
        if (!updated_registry[ref].first) {
            updated_registry[failure["ref"]].first = true;
            updated_registry[ref].second->set_failure(updated_registry[failure["ref"]].second);
        } else {
            fprintf(stderr, "Uh oh, bad tree! You defined a node twice. Don't do that!\n");
        }
    }
    return updated_registry;
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
    tolerance->y = parameters_field["y"];
    tolerance->z = parameters_field["z"];
    tolerance->roll = parameters_field["roll"];
    tolerance->pitch = parameters_field["pitch"];
    tolerance->yaw = parameters_field["yaw"];
    return tolerance;
}

std::array<uint8_t,16> MissionFileParser::string_to_array(std::string string) {
    std::array<uint8_t,16> array = {0};
    if (string.size() > 16) {
        fprintf(stderr, "Object ID too long! Unable to parse, returning empty.");
        return array;
    }
    for (int i = 0; i < 16; i++) {
        array[i] = string[i];
    }
    return array;
}
 