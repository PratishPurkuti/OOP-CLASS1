#include "Hand.hpp"

void Hand::addCard(const Card& c) {
    cards.push_back(c);
}

void Hand::clear() {
    cards.clear();
}

int Hand::size() const {
    return static_cast<int>(cards.size());
}

const Card& Hand::at(int i) const {
    return cards.at(i);
}