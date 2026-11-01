#pragma once

#include <vector>
#include "Card.hpp"

class Hand {
private:
    std::vector<Card> cards;

public:
    void addCard(const Card& c);
    void clear();
    int size() const;
    const Card& at(int i) const;
};