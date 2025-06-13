#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>

using namespace std;

class FlashCard {
public:
    string front;
    string back;

    FlashCard(string f = "", string b = "") : front(f), back(b) {}
};

class CardRecord {
public:
    FlashCard card;
    int timesReviewed = 0;
    int timesCorrect = 0;

    double getAccuracy() const {
        return timesReviewed > 0 ? (timesCorrect * 100.0 / timesReviewed) : 0;
    }
};

class Deck {
private:
    vector<CardRecord> cards;

    string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t");
        if (first == string::npos) return "";
        size_t last = str.find_last_not_of(" \t");
        return str.substr(first, (last-first+1));
    }

public:
    void addCard(const FlashCard& card) {
        if (!card.front.empty() && !card.back.empty()) {
            CardRecord cr;
            cr.card = card;
            cards.push_back(cr);
            cout << "Card added successfully!\n";
        }
    }

    vector<CardRecord>& getCards() { return cards; }
    size_t getCardCount() const { return cards.size(); }

    bool save(const string& filename) {
        ofstream file(filename);
        if (!file) {
            cerr << "Error saving to " << filename << "\n";
            return false;
        }

        for (const auto& cr : cards) {
            file << cr.card.front << "|" << cr.card.back << "|"
                 << cr.timesReviewed << "|" << cr.timesCorrect << "\n";
        }
        cout << "Saved " << cards.size() << " cards to " << filename << "\n";
        return true;
    }

    bool load(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "No save file found, starting fresh\n";
            return false;
        }

        cards.clear();
        string line;
        while (getline(file, line)) {
            line = trim(line);
            if (line.empty()) continue;

            vector<string> parts;
            stringstream ss(line);
            string part;

            while (getline(ss, part, '|')) {
                parts.push_back(part);
            }

            if (parts.size() != 4) continue;

            try {
                CardRecord cr;
                cr.card = FlashCard(parts[0], parts[1]);
                cr.timesReviewed = stoi(parts[2]);
                cr.timesCorrect = stoi(parts[3]);
                cards.push_back(cr);
            } catch (...) {
                cerr << "Error parsing card data\n";
            }
        }
        cout << "Loaded " << cards.size() << " cards\n";
        return true;
    }
};

class FlashCardApp {
private:
    Deck deck;
    const string filename = "cards.txt";

    void clearInput() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

public:
    FlashCardApp() {
        deck.load(filename);
    }

    ~FlashCardApp() {
        deck.save(filename);
    }

    void run() {
        while (true) {
            cout << "\n===== FLASHCARD =====\n";
            cout << "1. Create Card\n";
            cout << "2. Review Cards (" << deck.getCardCount() << " cards)\n";
            cout << "3. Show Statistics\n";
            cout << "4. Exit\n";
            cout << "Choose (1-4): ";

            int choice;
            cin >> choice;
            clearInput();

            switch (choice) {
                case 1: createCard(); break;
                case 2: reviewCards(); break;
                case 3: showStatistics(); break;
                case 4: return;
                default: cout << "Invalid choice!\n";
            }
        }
    }

    void createCard() {
        string front, back;
        cout << "\nEnter front (question): ";
        getline(cin, front);
        cout << "Enter back (answer): ";
        getline(cin, back);

        deck.addCard(FlashCard(front, back));
    }

    void reviewCards() {
        if (deck.getCardCount() == 0) {
            cout << "\nNo cards to review!\n";
            return;
        }

        cout << "\n=== REVIEW MODE ===\n";
        for (auto& cr : deck.getCards()) {
            cout << "\nQ: " << cr.card.front << "\n";
            cout << "Press enter to show answer...";
            cin.ignore();
            cout << "A: " << cr.card.back << "\n";

            string response;
            do {
                cout << "Did you know it? (y/n): ";
                getline(cin, response);
            } while (response != "y" && response != "n");

            cr.timesReviewed++;
            if (response == "y") cr.timesCorrect++;

            cout << "Accuracy: " << fixed << setprecision(1)
                 << cr.getAccuracy() << "%\n";
        }
        cout << "\nReview complete!\n";
    }

    void showStatistics() {
        if (deck.getCardCount() == 0) {
            cout << "\nNo cards available!\n";
            return;
        }

        cout << "\n=== STATISTICS ===\n";
        cout << "Total cards: " << deck.getCardCount() << "\n";

        int totalReviews = 0;
        int totalCorrect = 0;
        for (const auto& cr : deck.getCards()) {
            totalReviews += cr.timesReviewed;
            totalCorrect += cr.timesCorrect;
        }

        double overallAccuracy = totalReviews > 0 ?
            (totalCorrect * 100.0 / totalReviews) : 0;

        cout << "Total reviews: " << totalReviews << "\n";
        cout << "Overall accuracy: " << fixed << setprecision(1)
             << overallAccuracy << "%\n";

        cout << "\nCard Details:\n";
        for (size_t i = 0; i < deck.getCards().size(); i++) {
            const auto& cr = deck.getCards()[i];
            cout << "\nCard #" << (i+1) << "\n";
            cout << "Q: " << cr.card.front << "\n";
            cout << "Reviews: " << cr.timesReviewed
                 << " | Correct: " << cr.timesCorrect
                 << " | Accuracy: " << fixed << setprecision(1)
                 << cr.getAccuracy() << "%\n";
        }
    }
};

int main() {
    FlashCardApp app;
    app.run();
    return 0;
}
