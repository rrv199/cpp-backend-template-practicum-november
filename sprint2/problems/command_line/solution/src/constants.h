#pragma once

#include <string_view>

namespace endpoints {
    constexpr std::string_view JOIN_GAME = "/api/v1/game/join";
    constexpr std::string_view GET_PLAYERS = "/api/v1/game/players";
    constexpr std::string_view GAME_STATE = "/api/v1/game/state";
    constexpr std::string_view PLAYER_ACTION = "/api/v1/game/player/action";
    constexpr std::string_view TICK = "/api/v1/game/tick";
}
