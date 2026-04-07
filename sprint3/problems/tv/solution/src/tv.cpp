#include "tv.h"

bool TV::IsTurnedOn() const noexcept {
    return is_turned_on_;
}

std::optional<int> TV::GetChannel() const noexcept {
    if (is_turned_on_) {
        return channel_;
    }
    return std::nullopt;
}

void TV::TurnOn() noexcept {
    is_turned_on_ = true;
}

void TV::TurnOff() noexcept {
    is_turned_on_ = false;
}

void TV::SelectChannel(int channel) noexcept {
    if (is_turned_on_ && channel >= 1 && channel <= 999) {
        if (channel_ != channel) {
            previous_channel_ = channel_;
            channel_ = channel;
        }
    }
}

void TV::SelectPreviousChannel() noexcept {
    if (is_turned_on_ && previous_channel_.has_value()) {
        int new_channel = previous_channel_.value();
        previous_channel_ = channel_;
        channel_ = new_channel;
    }
}
