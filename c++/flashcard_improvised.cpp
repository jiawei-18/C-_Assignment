#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>

class Flashcard {
public:
    std::string question;
    std::string answer;
    bool easy;  // true = easy, false = hard
};

class FlashcardDeck {
public:
    std::vector<Flashcard> cards;

    // Load flashcards from a file (format: question|answer|easyFlag)
    void loadFromFile(const std::string& filename) {
        std::ifstream in_file(filename.c_str());
        if (!in_file) {
            std::cout << "No existing file found. Starting with an empty deck.\n";
            return;
        }
        std::string line;
        while (std::getline(in_file, line)) {
            if (line.empty()) continue;
            size_t p1 = line.find('|'), p2 = line.find('|', p1 + 1);
            if (p1 == std::string::npos || p2 == std::string::npos) continue;
            Flashcard card;
            card.question = line.substr(0, p1);
            card.answer = line.substr(p1 + 1, p2 - p1 - 1);
            char flag = line[p2 + 1];
            card.easy = (flag == 'e' || flag == 'E');
            cards.push_back(card);
        }
        in_file.close();
    }

    // Save flashcards to a file
    void saveToFile(const std::string& filename) {
        std::ofstream out_file(filename.c_str());
        for (const auto& card : cards) {
            out_file << card.question << "|" << card.answer << "|";
            out_file << (card.easy ? 'e' : 'h') << "\n";
        }
        out_file.close();
    }
};

int main() {
    std::cout << "Flashcard Program (Version 2 - With File I/O)\n";
    FlashcardDeck deck;
    const std::string filename = "flashcards.txt";

    // Load existing flashcards
    deck.loadFromFile(filename);

    // Input new flashcards
    while (true) {
        Flashcard card;
        std::cout << "\nEnter question (leave blank to stop adding): ";
        std::getline(std::cin, card.question);
        if (card.question.empty()) break;
        std::cout << "Enter answer: ";
        std::getline(std::cin, card.answer);
        card.easy = true;  // default to easy
        deck.cards.push_back(card);
    }

    // Review and mark easy/hard
    std::cout << "\n--- Review Flashcards ---\n";
    for (auto& card : deck.cards) {
        std::cout << "Q: " << card.question << "\n";
        std::cout << "(Press Enter to see answer)"; std::cin.get();
        std::cout << "A: " << card.answer << "\n";
        char choice;
        std::cout << "Mark this card as easy or hard? (e/h): ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        card.easy = (choice == 'e' || choice == 'E');
    }

    // Save to file
    deck.saveToFile(filename);
    std::cout << "\nFlashcards saved to " << filename << "\n";
    return 0;
}