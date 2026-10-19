#include <gtest/gtest.h>
#include "rclcpp/rclcpp.hpp"

#include "mission_manager.hpp"
#include "mission_file_parser.hpp"

class TestSubscriber : public ::testing::Test {
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

TEST_F(TestSubscriber, NodeConstruction) {
    ASSERT_NO_THROW({
        create_node();
    });

    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->get_name(), std::string("mission_manager"));
}

// When more robust pathing exists, should use a seperate test_mission_files folder
TEST_F(TestSubscriber, MissionList) {
    create_node();
    // node->parse_mission_list_file("test_file.txt");
    // node->parse_new_files();

    // // Add checking for proper error thrown from always_run.txt and garbage_file
    // EXPECT_EQ(node->get_current_mission_name(), "sample_mission_file.json");
    // node->jump_to_mission(1);
    // EXPECT_EQ(node->get_current_mission_name(), "sample_mission_file_2.json");
}

#ifdef ENABLE_TESTING

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif // ENABLE_TESTING
