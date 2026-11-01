#include "TuiView.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <algorithm>
#include <string>

using namespace ftxui;

static Element drawCard(const Card& card) {
    return vbox({
        text(card.rankSymbol()),
        text(card.suitSymbol()) | bold | center,
        text(card.rankSymbol()) | align_right
    }) | color(card.isRed() ? Color::Red : Color::White)
       | size(WIDTH, EQUAL, 9)
       | border;
}

static Element drawHand(const Hand& hand, bool hideSecond) {
    Elements cards;

    for (int i = 0; i < hand.size(); i++) {
        if (i == 1 && hideSecond) {
            cards.push_back(
                vbox({
                    text("???"),
                    text("???") | center,
                    text("???") | align_right
                }) | color(Color::Blue)
                   | size(WIDTH, EQUAL, 9)
                   | border
            );
        } else {
            cards.push_back(drawCard(hand.at(i)));
        }
    }

    if (cards.empty()) {
        return text("No cards yet") | dim;
    }

    return hbox(cards);
}

static std::string resultText(Outcome outcome) {
    switch (outcome) {
        case Outcome::PlayerBlackjack: return "Blackjack! You win.";
        case Outcome::PlayerWin:       return "You win.";
        case Outcome::DealerWin:       return "Dealer wins.";
        case Outcome::Push:            return "Push. Bet returned.";
        case Outcome::PlayerBust:      return "You busted.";
        case Outcome::DealerBust:      return "Dealer busted. You win.";
        case Outcome::None:            return "";
    }

    return "";
}

TuiView::TuiView(Blackjack& game)
    : game(game) {
}

void TuiView::run() {
    auto screen = ScreenInteractive::TerminalOutput();
    int bet = 1;
    std::string message;

    auto renderer = Renderer([&] {
        std::string keys;
        std::string status;

        if (game.phase() == Phase::Betting) {
            keys = "Up/Down: change bet | Enter: deal | q: quit";
            status = "Choose your bet.";
        } else if (game.phase() == Phase::PlayerTurn) {
            keys = "h: hit | s: stand | q: quit";
            status = "Your turn.";
        } else if (game.phase() == Phase::RoundOver) {
            keys = "Enter: next round | q: quit";
            status = resultText(game.outcome());
        } else {
            keys = "q: quit";
            status = "Game over. No chips remain.";
        }

        std::string dealerTitle = "Dealer";

        if (!game.dealerHoleHidden() && game.dealerHand().size() > 0) {
            dealerTitle += " - Total: "
                + std::to_string(Blackjack::handValue(game.dealerHand()));
        }

        std::string playerTitle = "Player";

        if (game.playerHand().size() > 0) {
            playerTitle += " - Total: "
                + std::to_string(Blackjack::handValue(game.playerHand()));
        }

        return vbox({
            text("BLACKJACK") | bold | center,
            separator(),
            text("Chips: " + std::to_string(game.chips())
                 + "    Bet: " + std::to_string(bet)),
            separator(),
            text(dealerTitle) | bold,
            drawHand(game.dealerHand(), game.dealerHoleHidden()),
            separator(),
            text(playerTitle) | bold,
            drawHand(game.playerHand(), false),
            separator(),
            text(status) | bold,
            text(message) | color(Color::Yellow),
            separator(),
            text(keys) | dim
        }) | border;
    });

    auto app = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q')) {
            screen.Exit();
            return true;
        }

        if (game.phase() == Phase::Betting) {
            if (event == Event::ArrowUp) {
                if (bet < game.chips()) {
                    bet++;
                }
                return true;
            }

            if (event == Event::ArrowDown) {
                if (bet > 1) {
                    bet--;
                }
                return true;
            }

            if (event == Event::Return) {
                message = game.startRound(bet)
                    ? "" : "Cannot start round with that bet.";
                return true;
            }
        } else if (game.phase() == Phase::PlayerTurn) {
            if (event == Event::Character('h')) {
                message = game.hit() ? "" : "Cannot hit.";
                return true;
            }

            if (event == Event::Character('s')) {
                message = game.stand() ? "" : "Cannot stand.";
                return true;
            }
        } else if (game.phase() == Phase::RoundOver) {
            if (event == Event::Return) {
                if (game.nextRound()) {
                    bet = std::max(1, std::min(bet, game.chips()));
                    message.clear();
                }
                return true;
            }
        }

        return false;
    });

    screen.Loop(app);
}