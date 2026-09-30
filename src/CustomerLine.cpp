// SPDX-License-Identifier: MIT
#include "cha_craft/CustomerLine.hpp"

#include "cha_craft/Constants.hpp"

#include <algorithm>

namespace cha_craft {

CustomerLine::CustomerLine() = default;

bool CustomerLine::IsFull() const {
    return customers_.size() >= MAX_CUSTOMERS;
}

int CustomerLine::Tick(float dt) {
    for (auto& c : customers_) c.patience -= dt;

    int walkedOut = 0;
    for (std::size_t i = 0; i < customers_.size(); ) {
        if (customers_[i].patience <= 0.0f) {
            customers_.erase(customers_.begin() + i);
            ++walkedOut;
        } else {
            ++i;
        }
    }
    return walkedOut;
}

void CustomerLine::Spawn(const std::string& recipeName, CustomerType type, float patience, int id) {
    if (IsFull()) return;
    Customer c;
    c.id           = id;
    c.type         = type;
    c.recipeName   = recipeName;
    c.maxPatience  = patience;
    c.patience     = patience;
    customers_.push_back(c);
}

ServeResult CustomerLine::ServeHighestPriority(const RecipeBook& recipes,
                                               const std::vector<std::string>& cupSnapshotSorted) {
    ServeResult result{ServeResult::Kind::EMPTY, -1, CustomerType::REGULAR, 0, false};
    if (customers_.empty()) return result;

    // Rebuild a fresh priority_queue so ordering reflects the up-to-date
    // patience values. std::priority_queue has no decrease-key, so this
    // is the standard pattern when priorities change after insertion.
    std::priority_queue<Customer, std::vector<Customer>, CustomerPriorityCompare> pq(
        customers_.begin(), customers_.end(), CustomerPriorityCompare());

    Customer top = pq.top();
    auto required = recipes.IngredientsFor(top.recipeName);
    // Game rule: ingredient order doesn't matter. Sort the recipe's list
    // so == does a multiset-style comparison against the already-sorted
    // cup snapshot. Without sorting here, vector::operator== is sequence
    // equality and a correct cup in the wrong push order would mismatch.
    std::sort(required.begin(), required.end());

    if (cupSnapshotSorted == required) {
        // Success: remove the customer and report reward.
        for (std::size_t i = 0; i < customers_.size(); ++i) {
            if (customers_[i].id == top.id) {
                customers_.erase(customers_.begin() + i);
                break;
            }
        }
        int base = (top.type == CustomerType::VIP) ? VIP_REWARD : REGULAR_REWARD;
        int bonus = static_cast<int>(top.patience * PATIENCE_BONUS_MULT);
        result = {ServeResult::Kind::MATCH, top.id, top.type, base + bonus,
                  top.type == CustomerType::VIP};
    } else {
        // Wrong recipe: customer stays, penalty only.
        result = {ServeResult::Kind::MISMATCH, top.id, top.type, -WRONG_RECIPE_PENALTY,
                  top.type == CustomerType::VIP};
    }
    return result;
}

void CustomerLine::Clear() { customers_.clear(); }

} // namespace cha_craft
