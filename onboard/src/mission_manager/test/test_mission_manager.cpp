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
        parser = std::make_shared<MissionFileParser>();
    }
};

TEST_F(TestMissionManager, NodeConstruction) {
    ASSERT_NO_THROW({
        create_node();
    });

    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->get_name(), std::string("mission_manager"));
}

// When more robust pathing exists, should use a seperate test_mission_files folder
TEST_F(TestParser, MissionList) {
    create_parser();
    try {
        parser->parse_mission_list_file("test_file.txt");
        parser->parse_new_files();
    } catch (std::exception& e) {
        FAIL();
    }    

    EXPECT_EQ(parser->get_current_mission_name(), "sample_mission_file.json");
    parser->jump_to_mission(1);
    EXPECT_EQ(parser->get_current_mission_name(), "sample_mission_file_2.json");
}

#ifdef ENABLE_TESTING

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif // ENABLE_TESTING
