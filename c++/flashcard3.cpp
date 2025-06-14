#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>

// Stores a flashcard (question, answer, hard flag)
class Flashcard {
public:
    std::string question;
    std::string answer;
    bool hard;  // true if marked hard

    Flashcard(std::string q="", std::string a="", bool h=false)
        : question(q), answer(a), hard(h) {}
};

// Tracks number of correct answers and total questions
class ScoreTracker {
public:
    int totalAsked = 0;
    int correctAnswers = 0;

    void recordAnswer(bool correct) {
        totalAsked++;
        if (correct) correctAnswers++;
    }
    void displayScore() const {
        std::cout << "\nScore: " << correctAnswers << "/" << totalAsked << " correct.\n";
    }
};

// Manages a set of flashcards and file I/O
class FlashcardDeck {
public:
    std::vector<Flashcard> cards;

    void loadFromFile(const std::string& filename) {
        std::ifstream in_file(filename.c_str());
        if (!in_file) return;
        std::string line;
        while (std::getline(in_file, line)) {
            if (line.empty()) continue;
            size_t p1 = line.find('|'), p2 = line.find('|', p1 + 1);
            if (p1 == std::string::npos || p2 == std::string::npos) continue;
            std::string q = line.substr(0, p1);
            std::string a = line.substr(p1 + 1, p2 - p1 - 1);
            bool h = (line[p2 + 1] == 'h' || line[p2 + 1] == 'H');
            cards.emplace_back(q, a, h);
        }
        in_file.close();
    }
    void saveToFile(const std::string& filename) {
        std::ofstream out_file(filename.c_str());
        for (const auto& card : cards) {
            out_file << card.question << "|" << card.answer << "|";
            out_file << (card.hard ? 'h' : 'e') << "\n";
        }
        out_file.close();
    }
};

// Implements a simple spaced-repetition review
class SpacedRepetition {
public:
    // Ask cards in a queue; re-add hard ones for another round
    void review(FlashcardDeck& deck, ScoreTracker& score) {
        std::vector<Flashcard> queue = deck.cards;
        while (!queue.empty()) {
            Flashcard card = queue.front();
            queue.erase(queue.begin());

            std::cout << "Q: " << card.question << "\n";
            std::cin.get();  // wait for user to press Enter
            std::cout << "A: " << card.answer << "\n";

            char response;
            std::cout << "Did you answer correctly? (y/n): ";
            std::cin >> response;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            bool correct = (response == 'y' || response == 'Y');
            score.recordAnswer(correct);

            // If correct, let user mark difficulty; if wrong, mark hard automatically
            if (correct) {
                char diff;
                std::cout << "Mark difficulty? (e for easy, h for hard): ";
                std::cin >> diff;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                card.hard = (diff == 'h' || diff == 'H');
            } else {
                card.hard = true;
            }

            if (card.hard) {
                // Re-add hard cards to the end of the queue
                queue.push_back(card);
            }
        }
    }
};

int main() {
    std::cout << "Flashcard Program (Version 3 - Spaced Repetition)\n";
    FlashcardDeck deck;
    const std::string filename = "flashcards.txt";

    // Load existing flashcards
    deck.loadFromFile(filename);

    // Add new flashcards
    while (true) {
        Flashcard card;
        std::cout << "\nEnter question (leave blank to finish): ";
        std::getline(std::cin, card.question);
        if (card.question.empty()) break;
        std::cout << "Enter answer: ";
        std::getline(std::cin, card.answer);
        card.hard = false;
        deck.cards.push_back(card);
    }

    // Run spaced-repetition review
    ScoreTracker score;
    SpacedRepetition trainer;
    trainer.review(deck, score);

    // Show score and save progress
    score.displayScore();
    deck.saveToFile(filename);
    std::cout << "\nProgress saved. Goodbye!\n";
    return 0;
}
