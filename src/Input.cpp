// SPDX-License-Identifier: MIT
#include "cha_craft/Input.hpp"

#include "raylib.h"

namespace cha_craft {

Action PollAction() {
    // Order = priority: SERVE / RESTART / UNDO / ADD are deliberately
    // checked before CAROUSEL navigation, but the original code used
    // IsKeyPressed (edge-triggered), so no two actions fire on the
    // same frame in practice. We mirror that exactly.
    if (IsKeyPressed(KEY_S))        return Action::Serve;
    if (IsKeyPressed(KEY_R))        return Action::Restart;
    if (IsKeyPressed(KEY_BACKSPACE)) return Action::Undo;
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) return Action::AddIngredient;
    if (IsKeyPressed(KEY_L))        return Action::ChooseNext;
    if (IsKeyPressed(KEY_H))        return Action::ChoosePrev;
    if (IsKeyPressed(KEY_RIGHT))    return Action::ChooseNext;
    if (IsKeyPressed(KEY_LEFT))     return Action::ChoosePrev;
    return Action::None;
}

} // namespace cha_craft
