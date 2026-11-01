#pragma once

#include "GameView.hpp"
#include "Blackjack.hpp"

class ConsoleView : public GameView {
private:
    Blackjack& game;

    void showHands() const;
    void showResult() const;

public:
    explicit ConsoleView(Blackjack& game);
    void run() override;
};