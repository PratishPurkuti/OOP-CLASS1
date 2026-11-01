#include "Tests.hpp"
#include "Blackjack.hpp"
#include <iostream>

int runTests() {
    int failures = 0;

    auto check = [&](const char* name, int actual, int expected) {
        if (actual == expected) {
            std::cout << "PASS: " << name << '\n';
        } else {
            std::cout << "FAIL: " << name
                      << " (expected " << expected
                      << ", got " << actual << ")\n";
            failures++;
        }
    };

    Hand hand;

    hand.addCard(Card(0));
    hand.addCard(Card(12));
    check("Ace + King = 21", Blackjack::handValue(hand), 21);

    hand.clear();
    hand.addCard(Card(0));
    hand.addCard(Card(5));
    check("Ace + 6 = 17", Blackjack::handValue(hand), 17);

    hand.addCard(Card(9));
    check("Ace + 6 + 10 = 17", Blackjack::handValue(hand), 17);

    hand.clear();
    hand.addCard(Card(0));
    hand.addCard(Card(13));
    hand.addCard(Card(8));
    check("Ace + Ace + 9 = 21", Blackjack::handValue(hand), 21);

    hand.clear();
    hand.addCard(Card(12));
    hand.addCard(Card(11));
    hand.addCard(Card(4));
    check("King + Queen + 5 = 25", Blackjack::handValue(hand), 25);

    hand.clear();
    hand.addCard(Card(0));
    hand.addCard(Card(12));
    check("Ace + King is natural", Blackjack::isNatural(hand), true);

    hand.clear();
    hand.addCard(Card(0));
    hand.addCard(Card(5));
    check("Ace + 6 is soft", Blackjack::isSoft(hand), true);

    hand.addCard(Card(9));
    check("Ace + 6 + 10 is hard", Blackjack::isSoft(hand), false);

    hand.clear();
    hand.addCard(Card(0));
    hand.addCard(Card(13));
    hand.addCard(Card(8));
    check("Ace + Ace + 9 is soft", Blackjack::isSoft(hand), true);
    check("Three-card 21 is not natural",
        Blackjack::isNatural(hand), false);

    hand.clear();
    hand.addCard(Card(9));
    hand.addCard(Card(6));
    check("10 + 7 is hard", Blackjack::isSoft(hand), false);
    Blackjack game;

    check("New game starts with 100 chips", game.chips(), 100);
    check("New game starts in Betting",
        game.phase() == Phase::Betting, true);

    check("Zero bet rejected", game.startRound(0), false);
    check("Negative bet rejected", game.startRound(-5), false);
    check("Oversized bet rejected", game.startRound(101), false);
    check("Rejected bets preserve chips", game.chips(), 100);
    check("Rejected bets leave player hand empty",
        game.playerHand().size(), 0);

    check("Valid bet starts a round", game.startRound(10), true);
    check("Player receives two cards", game.playerHand().size(), 2);
    check("Dealer receives two cards", game.dealerHand().size(), 2);

    check("Cannot start another round immediately",
    game.startRound(10), false);
    Blackjack naturalGame;
    naturalGame.setNextDeck(
        Deck({Card(0), Card(8), Card(12), Card(6)})
    );
    naturalGame.startRound(10);

    check("Player natural wins 15 chips",
        naturalGame.chips(), 115);
    check("Player natural outcome",
        naturalGame.outcome() == Outcome::PlayerBlackjack, true);
    check("Natural ends the round",
        naturalGame.phase() == Phase::RoundOver, true);
    check("Dealer hole card revealed after natural",
        naturalGame.dealerHoleHidden(), false);

    Blackjack pushGame;
    pushGame.setNextDeck(
        Deck({Card(0), Card(13), Card(12), Card(25)})
    );
    pushGame.startRound(10);

    check("Both naturals return the bet", pushGame.chips(), 100);
    check("Both naturals produce Push",
        pushGame.outcome() == Outcome::Push, true);

    Blackjack dealerGame;
    dealerGame.setNextDeck(
        Deck({Card(8), Card(0), Card(6), Card(12)})
    );
    dealerGame.startRound(10);

    check("Dealer natural loses the player's bet",
        dealerGame.chips(), 90);
    check("Dealer natural outcome",
        dealerGame.outcome() == Outcome::DealerWin, true);

    Blackjack waitingGame;

    check("Hit rejected during Betting", waitingGame.hit(), false);
    check("Rejected hit preserves chips", waitingGame.chips(), 100);
    check("Rejected hit preserves empty hand",
        waitingGame.playerHand().size(), 0);
    check("Rejected hit preserves Betting phase",
        waitingGame.phase() == Phase::Betting, true);

    Blackjack hitGame;
    hitGame.setNextDeck(
        Deck({Card(9), Card(8), Card(5), Card(6), Card(4), Card(1)})
    );
    hitGame.startRound(10);

    check("Dealer hole card hidden during player turn",
        hitGame.dealerHoleHidden(), true);

    check("Hit accepted during player turn", hitGame.hit(), true);
    check("Hit adds one card", hitGame.playerHand().size(), 3);
    check("Hit produces 21", Blackjack::handValue(hitGame.playerHand()), 21);
    check("21 from three cards is not natural",
        Blackjack::isNatural(hitGame.playerHand()), false);

    check("Another hit accepted", hitGame.hit(), true);
    check("Player total becomes 23",
        Blackjack::handValue(hitGame.playerHand()), 23);
    check("Bust outcome recorded",
        hitGame.outcome() == Outcome::PlayerBust, true);
    check("Bust ends the round",
        hitGame.phase() == Phase::RoundOver, true);
    check("Bust leaves 90 chips", hitGame.chips(), 90);
    check("Dealer does not draw after player bust",
        hitGame.dealerHand().size(), 2);

    check("Hit rejected after round ends", hitGame.hit(), false);
    check("Rejected hit leaves four player cards",
        hitGame.playerHand().size(), 4);

    Blackjack standWaiting;
    check("Stand rejected during Betting", standWaiting.stand(), false);

    Blackjack soft17Game;
    soft17Game.setNextDeck(
        Deck({Card(9), Card(0), Card(7), Card(5), Card(12)})
    );
    soft17Game.startRound(10);
    check("Stand accepted", soft17Game.stand(), true);
    check("Dealer stands on soft 17",
        soft17Game.dealerHand().size(), 2);
    check("Dealer soft 17 total",
        Blackjack::handValue(soft17Game.dealerHand()), 17);
    check("18 beats dealer 17",
        soft17Game.outcome() == Outcome::PlayerWin, true);
    check("Normal win leaves 110 chips", soft17Game.chips(), 110);
    check("Stand reveals dealer hole card",
        soft17Game.dealerHoleHidden(), false);
    check("Stand ends round",
        soft17Game.phase() == Phase::RoundOver, true);
    check("Second stand rejected", soft17Game.stand(), false);
    check("Second stand cannot pay twice", soft17Game.chips(), 110);

    Blackjack dealerHitGame;
    dealerHitGame.setNextDeck(
        Deck({Card(9), Card(9), Card(7), Card(5), Card(4)})
    );
    dealerHitGame.startRound(10);
    dealerHitGame.stand();
    check("Dealer hits on 16",
        dealerHitGame.dealerHand().size(), 3);
    check("Dealer draws to 21",
        Blackjack::handValue(dealerHitGame.dealerHand()), 21);
    check("Dealer higher total wins",
        dealerHitGame.outcome() == Outcome::DealerWin, true);
    check("Normal loss leaves 90 chips", dealerHitGame.chips(), 90);

    Blackjack dealerBustGame;
    dealerBustGame.setNextDeck(
        Deck({Card(9), Card(9), Card(7), Card(5), Card(12)})
    );
    dealerBustGame.startRound(10);
    dealerBustGame.stand();
    check("Dealer bust outcome",
        dealerBustGame.outcome() == Outcome::DealerBust, true);
    check("Dealer bust pays normal win",
        dealerBustGame.chips(), 110);

    Blackjack tieGame;
    tieGame.setNextDeck(
        Deck({Card(9), Card(12), Card(7), Card(20)})
    );
    tieGame.startRound(10);
    tieGame.stand();
    check("Equal totals produce Push",
        tieGame.outcome() == Outcome::Push, true);
    check("Push returns original bet", tieGame.chips(), 100);

    Blackjack nextGame;

    check("Next round rejected during Betting",
        nextGame.nextRound(), false);

    nextGame.setNextDeck(
        Deck({Card(9), Card(8), Card(7), Card(6), Card(1)})
    );
    nextGame.startRound(10);

    check("Next round rejected during PlayerTurn",
        nextGame.nextRound(), false);

    nextGame.stand();

    check("Next round accepted after settlement",
        nextGame.nextRound(), true);
    check("Next round returns to Betting",
        nextGame.phase() == Phase::Betting, true);
    check("Next round preserves chip balance", nextGame.chips(), 100);
    check("Next round clears player hand",
        nextGame.playerHand().size(), 0);
    check("Next round clears dealer hand",
        nextGame.dealerHand().size(), 0);
    check("Next round clears outcome",
        nextGame.outcome() == Outcome::None, true);

    Blackjack brokeGame(10);
    brokeGame.setNextDeck(
        Deck({Card(8), Card(0), Card(6), Card(12)})
    );
    brokeGame.startRound(10);

    check("All-in loss leaves zero chips", brokeGame.chips(), 0);
    check("Finished all-in loss can advance",
        brokeGame.nextRound(), true);
    check("Zero chips transitions to GameOver",
        brokeGame.phase() == Phase::GameOver, true);
    check("Cannot bet after GameOver",
        brokeGame.startRound(1), false);
    check("Cannot advance after GameOver",
        brokeGame.nextRound(), false);

    std::cout << "Failures: " << failures << '\n';
    return failures == 0 ? 0 : 1;
}