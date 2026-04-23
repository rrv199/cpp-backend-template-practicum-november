#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "../src/tv.h"

class TVByDefault : public ::testing::Test {
protected:
    TV tv_;
};

class TurnedOnTV : public TVByDefault {
protected:
    void SetUp() override {
        tv_.TurnOn();
    }
};

class TurnedOffTV : public TVByDefault {
protected:
    void SetUp() override {
        tv_.TurnOn();
        tv_.TurnOff();
    }
};

TEST_F(TVByDefault, IsOffByDefault) {
    EXPECT_FALSE(tv_.IsTurnedOn());
}

TEST_F(TVByDefault, DoesntShowChannelByDefault) {
    EXPECT_FALSE(tv_.GetChannel().has_value());
}

TEST_F(TurnedOnTV, IsOnAfterTurnOn) {
    EXPECT_TRUE(tv_.IsTurnedOn());
}

TEST_F(TurnedOnTV, ShowsChannel1AfterFirstTurnOn) {
    EXPECT_THAT(tv_.GetChannel(), testing::Optional(1));
}

TEST_F(TurnedOffTV, IsOffAfterTurnOff) {
    EXPECT_FALSE(tv_.IsTurnedOn());
}

TEST_F(TurnedOffTV, DoesntShowChannelAfterTurnOff) {
    EXPECT_FALSE(tv_.GetChannel().has_value());
}

TEST_F(TurnedOnTV, SelectChannelChangesChannel) {
    tv_.SelectChannel(5);
    EXPECT_THAT(tv_.GetChannel(), testing::Optional(5));
}

TEST_F(TurnedOnTV, SelectChannelOutOfRangeIgnored) {
    tv_.SelectChannel(0);
    EXPECT_THAT(tv_.GetChannel(), testing::Optional(1));
    
    tv_.SelectChannel(1000);
    EXPECT_THAT(tv_.GetChannel(), testing::Optional(1));
}

TEST_F(TurnedOnTV, SelectChannel999IsValid) {
    tv_.SelectChannel(999);
    EXPECT_THAT(tv_.GetChannel(), testing::Optional(999));
}

TEST_F(TurnedOffTV, SelectChannelIgnoredWhenOff) {
    tv_.SelectChannel(5);
    EXPECT_FALSE(tv_.GetChannel().has_value());
}

TEST_F(TurnedOnTV, SelectPreviousChannelWithNoHistory) {
    tv_.SelectPreviousChannel();
    EXPECT_THAT(tv_.GetChannel(), testing::Optional(1));
}

TEST_F(TurnedOnTV, SelectPreviousChannelWorks) {
    tv_.SelectChannel(5);
    tv_.SelectChannel(8);
    tv_.SelectPreviousChannel();
    EXPECT_THAT(tv_.GetChannel(), testing::Optional(5));
    
    tv_.SelectPreviousChannel();
    EXPECT_THAT(tv_.GetChannel(), testing::Optional(8));
}

TEST_F(TurnedOnTV, SelectPreviousChannelAfterSameChannel) {
    tv_.SelectChannel(5);
    tv_.SelectChannel(5);
    tv_.SelectPreviousChannel();
    EXPECT_THAT(tv_.GetChannel(), testing::Optional(1));
}

TEST(TV, PreviousChannelRememberedAfterTurnOffAndOn) {
    TV tv;
    tv.TurnOn();
    tv.SelectChannel(7);
    tv.TurnOff();
    tv.TurnOn();
    EXPECT_THAT(tv.GetChannel(), testing::Optional(7));
    tv.SelectPreviousChannel();
    EXPECT_THAT(tv.GetChannel(), testing::Optional(1));
}

TEST_F(TurnedOnTV, MultipleOperations) {
    tv_.SelectChannel(3);
    tv_.SelectChannel(10);
    tv_.SelectPreviousChannel();
    tv_.SelectChannel(7);
    tv_.SelectPreviousChannel();
    tv_.SelectPreviousChannel();
    EXPECT_THAT(tv_.GetChannel(), testing::Optional(7));
}
