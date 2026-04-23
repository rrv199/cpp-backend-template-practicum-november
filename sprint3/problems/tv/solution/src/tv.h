#pragma once
#include <optional>

class TV {
public:
    bool IsTurnedOn() const noexcept;
    std::optional<int> GetChannel() const noexcept;
    void TurnOn() noexcept;
    void TurnOff() noexcept;
    void SelectChannel(int channel) noexcept;
    void SelectPreviousChannel() noexcept;

private:
    bool is_turned_on_ = false;
    int channel_ = 1;
    std::optional<int> previous_channel_;
};
