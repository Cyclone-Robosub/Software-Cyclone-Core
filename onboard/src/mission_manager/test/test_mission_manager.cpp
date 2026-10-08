#include <gtest/gtest.h>
#include "rclcpp/rclcpp.hpp"

#include "mission_manager.hpp"

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
        node = std::make_shared<MissionManager>();
    }
};

TEST_F(TestSubscriber, NodeConstruction) {
    ASSERT_NO_THROW({
        create_node();
    });

    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->get_name(), std::string("mission_manager"));
}

#ifdef ENABLE_TESTING

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif // ENABLE_TESTING
