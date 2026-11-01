#include "Blackjack.hpp"

int Blackjack::handValue(const Hand& hand) {
    int total = 0;
    int aces = 0;

    for (int i = 0; i < hand.size(); i++) {
        int rank = hand.at(i).getRank();

        if (rank == 0) {
            total += 11;
            aces++;
        } else if (rank >= 10) {
            total += 10;
        } else {
            total += rank + 1;
        }
    }

    while (total > 21 && aces > 0) {
        total -= 10;
        aces--;
    }

    return total;
}

bool Blackjack::isSoft(const Hand& hand) {
    int minimumTotal = 0;
    bool hasAce = false;

    for (int i = 0; i < hand.size(); i++) {
        int rank = hand.at(i).getRank();

        if (rank == 0) {
            minimumTotal += 1;
            hasAce = true;
        } else if (rank >= 10) {
            minimumTotal += 10;
        } else {
            minimumTotal += rank + 1;
        }
    }

    return hasAce && minimumTotal + 10 <= 21;
}

bool Blackjack::isNatural(const Hand& hand) {
    return hand.size() == 2 && handValue(hand) == 21;
}

Blackjack::Blackjack(int startingChips)
    : chipCount(startingChips > 0 ? startingChips : 0),
      currentBet(0),
      currentPhase(chipCount > 0 ? Phase::Betting : Phase::GameOver),
      currentOutcome(Outcome::None) {
}

Phase Blackjack::phase() const {
    return currentPhase;
}

int Blackjack::chips() const {
    return chipCount;
}

const Hand& Blackjack::playerHand() const {
    return player;
}

const Hand& Blackjack::dealerHand() const {
    return dealer;
}

bool Blackjack::dealerHoleHidden() const {
    return currentPhase == Phase::PlayerTurn;
}

Outcome Blackjack::outcome() const {
    return currentOutcome;
}

bool Blackjack::startRound(int bet) {
    if (currentPhase != Phase::Betting) {
        return false;
    }

    if (bet < 1 || bet > chipCount) {
        return false;
    }

    if (nextDeck.has_value()) {
        deck = nextDeck.value();
        nextDeck.reset();
    } else {
        deck = Deck();
        deck.shuffle();
    }

    player.clear();
    dealer.clear();

    currentBet = bet;
    chipCount -= bet;
    currentOutcome = Outcome::None;

    player.addCard(deck.deal());
    dealer.addCard(deck.deal());
    player.addCard(deck.deal());
    dealer.addCard(deck.deal());

    bool playerNatural = isNatural(player);
    bool dealerNatural = isNatural(dealer);

    if (playerNatural && dealerNatural) {
        chipCount += currentBet;
        currentOutcome = Outcome::Push;
    } else if (playerNatural) {
        chipCount += currentBet + currentBet * 3 / 2;
        currentOutcome = Outcome::PlayerBlackjack;
    } else if (dealerNatural) {
        currentOutcome = Outcome::DealerWin;
    } else {
        currentPhase = Phase::PlayerTurn;
        return true;
    }

    currentPhase = Phase::RoundOver;
    return true;
}

bool Blackjack::setNextDeck(const Deck& stackedDeck) {
    if (currentPhase != Phase::Betting || stackedDeck.size() < 4) {
        return false;
    }

    nextDeck = stackedDeck;
    return true;
}

bool Blackjack::hit() {
    if (currentPhase != Phase::PlayerTurn || deck.empty()) {
        return false;
    }

    player.addCard(deck.deal());

    if (handValue(player) > 21) {
        currentOutcome = Outcome::PlayerBust;
        currentPhase = Phase::RoundOver;
    }

    return true;
}

bool Blackjack::stand() {
    if (currentPhase != Phase::PlayerTurn) {
        return false;
    }

    while (handValue(dealer) < 17) {
        if (deck.empty()) {
            return false;
        }

        dealer.addCard(deck.deal());
    }

    int playerTotal = handValue(player);
    int dealerTotal = handValue(dealer);

    if (dealerTotal > 21) {
        currentOutcome = Outcome::DealerBust;
        chipCount += currentBet * 2;
    } else if (playerTotal > dealerTotal) {
        currentOutcome = Outcome::PlayerWin;
        chipCount += currentBet * 2;
    } else if (playerTotal == dealerTotal) {
        currentOutcome = Outcome::Push;
        chipCount += currentBet;
    } else {
        currentOutcome = Outcome::DealerWin;
    }

    currentPhase = Phase::RoundOver;
    return true;
}

bool Blackjack::nextRound() {
    if (currentPhase != Phase::RoundOver) {
        return false;
    }

    player.clear();
    dealer.clear();
    currentBet = 0;
    currentOutcome = Outcome::None;

    if (chipCount > 0) {
        currentPhase = Phase::Betting;
    } else {
        currentPhase = Phase::GameOver;
    }

    return true;
}