#pragma once

#include "GameView.hpp"
#include "Blackjack.hpp"

class TuiView : public GameView {
private:
    Blackjack& game;

public:
    explicit TuiView(Blackjack& game);
    void run() override;
};