#pragma once

#include "Hand.hpp"
#include "Deck.hpp"
#include <optional>

enum class Phase {
    Betting,
    PlayerTurn,
    RoundOver,
    GameOver
};

enum class Outcome {
    None,
    PlayerBlackjack,
    PlayerWin,
    DealerWin,
    Push,
    PlayerBust,
    DealerBust
};

class Blackjack {
private:
    Deck deck;
    Hand player;
    Hand dealer;

    int chipCount;
    int currentBet;

    Phase currentPhase;
    Outcome currentOutcome;

    std::optional<Deck> nextDeck;

public:
    explicit Blackjack(int startingChips = 100);

    bool startRound(int bet);

    Phase phase() const;
    int chips() const;
    const Hand& playerHand() const;
    const Hand& dealerHand() const;
    bool dealerHoleHidden() const;
    Outcome outcome() const;

    static int handValue(const Hand& hand);
    static bool isSoft(const Hand& hand);
    static bool isNatural(const Hand& hand);
    bool setNextDeck(const Deck& stackedDeck);
    bool hit();
    bool stand();
    bool nextRound();
};