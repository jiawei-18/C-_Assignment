#include <iostream>
#include <string>
#include <vector>

class Flashcard {
public:
    std::string question;
    std::string answer;
};

int main() {
    std::vector<Flashcard> cards;
    std::cout << "Flashcard Program (Version 1 - Basic)\n";

    // Collect flashcards from user
    while (true) {
        Flashcard card;
        std::cout << "\nEnter question (leave blank to finish): ";
        std::getline(std::cin, card.question);
        if (card.question.empty()) break;
        std::cout << "Enter answer: ";
        std::getline(std::cin, card.answer);
        cards.push_back(card);
    }

    // Display all flashcards
    std::cout << "\n--- Flashcards ---\n";
    for (const auto& card : cards) {
        std::cout << "Q: " << card.question << "\n";
        std::cout << "A: " << card.answer << "\n\n";
    }
    std::cout << "End of flashcards.\n";
    return 0;
}