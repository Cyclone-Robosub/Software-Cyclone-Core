#include <gtest/gtest.h>
#include "rclcpp/rclcpp.hpp"

#include "mission_manager.hpp"
#include "mission_file_parser.hpp"

class TestMissionManager : public ::testing::Test {
protected:
    std::shared_ptr<MissionManager> node;

    void SetUp() override {
        if (!rclcpp::ok()) {
            rclcpp::init(0, nullptr);
        }
    }

    void TearDown() override {
        node.reset();
    }

    void create_node() {
        node = std::make_shared<MissionManager>(std::move(std::make_unique<MissionFileParser>()));
    }
};

class TestParser : public ::testing::Test {
protected:
    std::shared_ptr<MissionFileParser> parser;

    void SetUp() override {
        if (!rclcpp::ok()) {
            rclcpp::init(0, nullptr);
        }
    }

    void TearDown() override {
        parser.reset();
    }

    void create_parser() {
        parser = std::make_shared<MissionFileParser>("../../onboard/src/mission_manager/test/test_mission_files/");
    }
};

TEST_F(TestMissionManager, NodeConstruction) {
    ASSERT_NO_THROW({
        create_node();
    });

    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->get_name(), std::string("mission_manager"));
}

// TEST_F(TestParser, MissionList) {
//     create_parser();
//     try {
//         parser->add_from_mission_list_file("test_file.txt");
//         parser->parse_new_files();
//     } catch (const std::exception& e) {
//         FAIL();
//     }

//     // Seemingly getting segfaults from running parser->get_next_mission()
//     auto root = parser->get_next_mission();
//     EXPECT_EQ(parser->get_current_mission_name(), "test_sample_mission_file.json");
//     root = parser->get_next_mission();
//     EXPECT_EQ(parser->get_current_mission_name(), "test_sample_mission_file_2.json");
//     // Cannot move to last index to check name without going out of bounds. Unknown if this will be an issue for running program
// }

// Test whether an error is thrown properly on a tree that overwrites a node's children
// TEST_F(TestParser, DuplicateNodeErr) {
//     create_parser();
//     parser->add_file_to_parse("test_duplicate_node_mission_file.json");
//     EXPECT_ANY_THROW(parser->parse_new_files());
// }

// High Priority Tests:
/*
Trees Fail to parse if there are duplicate nodes

*/

#ifdef ENABLE_TESTING

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif // ENABLE_TESTING
