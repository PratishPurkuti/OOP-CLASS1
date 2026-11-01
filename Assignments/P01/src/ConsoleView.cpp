#include "ConsoleView.hpp"

#include <iostream>
#include <sstream>
#include <string>

ConsoleView::ConsoleView(Blackjack& game)
    : game(game) {
}

void ConsoleView::showHands() const {
    std::cout << "\nDealer: ";

    for (int i = 0; i < game.dealerHand().size(); i++) {
        if (i == 1 && game.dealerHoleHidden()) {
            std::cout << "[hidden] ";
        } else {
            std::cout << game.dealerHand().at(i).toString() << ' ';
        }
    }

    if (!game.dealerHoleHidden()) {
        std::cout << " Total: "
                  << Blackjack::handValue(game.dealerHand());
    }

    std::cout << "\nPlayer: ";

    for (int i = 0; i < game.playerHand().size(); i++) {
        std::cout << game.playerHand().at(i).toString() << ' ';
    }

    std::cout << " Total: "
              << Blackjack::handValue(game.playerHand()) << '\n';
}

void ConsoleView::showResult() const {
    switch (game.outcome()) {
        case Outcome::PlayerBlackjack:
            std::cout << "Blackjack! You win.\n";
            break;
        case Outcome::PlayerWin:
            std::cout << "You win.\n";
            break;
        case Outcome::DealerWin:
            std::cout << "Dealer wins.\n";
            break;
        case Outcome::Push:
            std::cout << "Push. Your bet is returned.\n";
            break;
        case Outcome::PlayerBust:
            std::cout << "You busted.\n";
            break;
        case Outcome::DealerBust:
            std::cout << "Dealer busted. You win.\n";
            break;
        case Outcome::None:
            break;
    }
}

void ConsoleView::run() {
    std::string input;

    while (true) {
        std::cout << "\nChips: " << game.chips() << '\n';

        if (game.phase() == Phase::GameOver) {
            std::cout << "Game over. No chips remain.\n";
            return;
        }

        if (game.phase() == Phase::Betting) {
            std::cout << "Enter bet (q to quit): ";

            if (!std::getline(std::cin, input) || input == "q") {
                return;
            }

            std::istringstream parser(input);
            int bet;
            std::string extra;

            if (!(parser >> bet) || (parser >> extra)) {
                std::cout << "Enter a whole number only.\n";
            } else if (!game.startRound(bet)) {
                std::cout << "Bet must be between 1 and "
                          << game.chips() << ".\n";
            }
        } else if (game.phase() == Phase::PlayerTurn) {
            showHands();
            std::cout << "h = hit, s = stand, q = quit: ";

            if (!std::getline(std::cin, input) || input == "q") {
                return;
            }

            if (input == "h") {
                if (!game.hit()) {
                    std::cout << "Cannot hit.\n";
                }
            } else if (input == "s") {
                if (!game.stand()) {
                    std::cout << "Cannot stand.\n";
                }
            } else {
                std::cout << "Unknown command.\n";
            }
        } else if (game.phase() == Phase::RoundOver) {
            showHands();
            showResult();

            std::cout << "Chips: " << game.chips()
                      << "\nEnter = continue, q = quit: ";

            if (!std::getline(std::cin, input) || input == "q") {
                return;
            }

            if (input.empty()) {
                game.nextRound();
            }
        }
    }
}