#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <map>
#include <algorithm>
#include <cctype>

using namespace std;

const int MAX_BOX = 5;

class FlashCard {
public:
    string front;
    string back;

    FlashCard(string f = "", string b = "") : front(f), back(b) {}
};

class CardRecord {
private:
    FlashCard card;
    int box = 0;
    int dueRound = 0;
    int timesReviewed = 0;
    int timesCorrect = 0;

public:
    FlashCard getCard() const { return card; }
    void setCard(FlashCard c) { card = c; }

    int getBox() const { return box; }
    void setBox(int b) { box = b; }

    int getDueRound() const { return dueRound; }
    void setDueRound(int r) { dueRound = r; }

    int getTimesReviewed() const { return timesReviewed; }
    void setTimesReviewed(int t) { timesReviewed = t; }

    int getTimesCorrect() const { return timesCorrect; }
    void setTimesCorrect(int t) { timesCorrect = t; }

    double getAccuracy() const {
        return timesReviewed > 0 ? (timesCorrect * 100.0 / timesReviewed) : 0;
    }

    string getProgressBar() const {
        const int width = 20;
        int filled = (timesReviewed > 0) ?
                    (width * timesCorrect / timesReviewed) : 0;
        string bar(filled, '=');
        bar += string(width - filled, '-');
        return "[" + bar + "]";
    }

    void markCorrect(int currentRound) {
        timesCorrect++;
        timesReviewed++;
        if (box < MAX_BOX) box++;
        dueRound = currentRound + (box >= 2 ? (1 << (box - 1)) : 1);
    }

    void markIncorrect(int currentRound) {
        timesReviewed++;
        box = 0;
        dueRound = currentRound + 1;
    }
};

class Deck {
private:
    vector<CardRecord> cards;
    int currentRound = 0;

    string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t");
        if (first == string::npos) return "";
        size_t last = str.find_last_not_of(" \t");
        return str.substr(first, (last-first+1));
    }

    string normalizeString(const string& str) {
        string result;
        for (char c : str) {
            if (isalnum(c)) {
                result += tolower(c);
            }
        }
        return result;
    }

public:
    void addCard(const FlashCard& card) {
        if (!card.front.empty() && !card.back.empty()) {
            CardRecord cr;
            cr.setCard(card);
            cards.push_back(cr);
            cout << "Card added successfully!\n";
        }
    }

    vector<CardRecord>& getCards() { return cards; }
    size_t getCardCount() const { return cards.size(); }

    void nextRound() { currentRound++; }
    int getCurrentRound() const { return currentRound; }

    void reset() {
        cards.clear();
        currentRound = 0;
    }

    vector<CardRecord*> getDueCards() {
        vector<CardRecord*> due;
        for (auto& cr : cards) {
            if (cr.getDueRound() <= currentRound) {
                due.push_back(&cr);
            }
        }

        // Fisher-Yates shuffle
        for (size_t i = due.size() - 1; i > 0; i--) {
            size_t j = rand() % (i + 1);
            swap(due[i], due[j]);
        }

        return due;
    }

    bool checkAnswer(const string& userAnswer, const string& correctAnswer) {
        string normalizedUser = normalizeString(userAnswer);
        string normalizedCorrect = normalizeString(correctAnswer);
        return normalizedUser == normalizedCorrect;
    }

    bool save(const string& filename) {
        ofstream file(filename);
        if (!file) {
            cerr << "Error saving to " << filename << "\n";
            return false;
        }

        file << currentRound << "\n";
        for (const auto& cr : cards) {
            file << cr.getCard().front << "|" << cr.getCard().back << "|"
                 << cr.getBox() << "|" << cr.getDueRound() << "|"
                 << cr.getTimesReviewed() << "|" << cr.getTimesCorrect() << "\n";
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

        // First line is current round
        getline(file, line);
        try {
            currentRound = stoi(line);
        } catch (...) {
            currentRound = 0;
        }

        while (getline(file, line)) {
            line = trim(line);
            if (line.empty()) continue;

            vector<string> parts;
            stringstream ss(line);
            string part;

            while (getline(ss, part, '|')) {
                parts.push_back(part);
            }

            if (parts.size() != 6) continue;

            try {
                CardRecord cr;
                cr.setCard(FlashCard(parts[0], parts[1]));
                cr.setBox(stoi(parts[2]));
                cr.setDueRound(stoi(parts[3]));
                cr.setTimesReviewed(stoi(parts[4]));
                cr.setTimesCorrect(stoi(parts[5]));
                cards.push_back(cr);
            } catch (...) {
                cerr << "Error parsing card data\n";
            }
        }
        cout << "Loaded " << cards.size() << " cards\n";
        return true;
    }
};

class SessionManager {
private:
    void clearInput() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

public:
    void runSession(Deck& deck) {
        deck.nextRound();
        auto& allCards = deck.getCards();

        if (allCards.empty()) {
            cout << "\nNo cards available for review!\n";
            return;
        }

        cout << "\n=== REVIEW SESSION ===\n";
        cout << "Press Enter to reveal answer, then enter 'c' for correct or 'i' for incorrect\n";
        cout << "Enter 'q' to quit\n\n";

        // Shuffle cards
        for (size_t i = allCards.size() - 1; i > 0; i--) {
            size_t j = rand() % (i + 1);
            swap(allCards[i], allCards[j]);
        }

        for (auto& cr : allCards) {
            cout << "Q: " << cr.getCard().front << "\n";
            cout << "Press Enter to show answer...";
            string input;
            getline(cin, input);

            if (input == "q") {
                break;
            }

            cout << "\nA: " << cr.getCard().back << "\n\n";

            while (true) {
                cout << "Was your answer correct? (c/i): ";
                getline(cin, input);

                if (input == "c") {
                    cr.markCorrect(deck.getCurrentRound());
                    cout << " Marked as correct! (Box " << cr.getBox() << ")\n\n";
                    break;
                } else if (input == "i") {
                    cr.markIncorrect(deck.getCurrentRound());
                    cout << " Marked as incorrect (Box 0)\n\n";
                    break;
                } else if (input == "q") {
                    return;
                } else {
                    cout << "Invalid input. Please enter 'c' or 'i'.\n";
                }
            }
        }
        cout << "\nSession complete!\n";
    }
};

class FlashCardApp {
private:
    Deck deck;
    SessionManager sessionManager;
    const string filename = "spaced_cards.txt";

    void clearInput() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    void resetData() {
        cout << "\n=== RESET ALL DATA ===\n";
        cout << "This will delete ALL cards and reset all progress!\n";
        cout << "Are you sure? (yes/no): ";

        string confirmation;
        getline(cin, confirmation);

        if (confirmation == "yes") {
            deck.reset();
            remove(filename.c_str());
            cout << "All data has been reset.\n";
        } else {
            cout << "Reset cancelled.\n";
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

    void showStatistics() {
        if (deck.getCardCount() == 0) {
            cout << "\nNo cards available!\n";
            return;
        }

        cout << "\n=== PERFORMANCE STATISTICS ===\n";

        int totalReviews = 0;
        int totalCorrect = 0;
        map<int, int> boxCounts;
        for (int i = 0; i <= MAX_BOX; i++) boxCounts[i] = 0;

        for (const auto& cr : deck.getCards()) {
            totalReviews += cr.getTimesReviewed();
            totalCorrect += cr.getTimesCorrect();
            boxCounts[cr.getBox()]++;
        }

        double overallAccuracy = totalReviews > 0 ?
            (totalCorrect * 100.0 / totalReviews) : 0;

        cout << "\nOverall:\n";
        cout << "Total reviews: " << totalReviews << "\n";
        cout << "Correct answers: " << totalCorrect << "\n";
        cout << "Accuracy: " << fixed << setprecision(1)
             << overallAccuracy << "%\n";

        cout << "\nCard Details:\n";
        for (size_t i = 0; i < deck.getCards().size(); i++) {
            const auto& cr = deck.getCards()[i];
            cout << "\nCard #" << (i+1) << " (Box " << cr.getBox() << ")\n";
            cout << "Q: " << cr.getCard().front << "\n";
            cout << cr.getProgressBar() << "  "
                 << fixed << setprecision(1) << cr.getAccuracy() << "%\n";
            cout << "Reviews: " << cr.getTimesReviewed()
                 << " | Correct: " << cr.getTimesCorrect() << "\n";
        }
    }

    void showHeatmap() {
        if (deck.getCardCount() == 0) {
            cout << "\nNo cards available!\n";
            return;
        }

        map<int, int> boxCounts;
        for (int i = 0; i <= MAX_BOX; i++) {
            boxCounts[i] = 0;
        }

        for (const auto& cr : deck.getCards()) {
            boxCounts[cr.getBox()]++;
        }

        cout << "\n=== DIFFICULTY HEATMAP ===\n";
        cout << "Box 0: Easiest, Box " << MAX_BOX << ": Hardest\n\n";

        const int max_bar_length = 50;
        int max_count = 0;
        for (const auto& [box, count] : boxCounts) {
            if (count > max_count) max_count = count;
        }

        for (int i = 0; i <= MAX_BOX; i++) {
            int bar_length = max_count > 0 ?
                (boxCounts[i] * max_bar_length / max_count) : 0;
            cout << "[" << i << "] " << string(bar_length, '#')
                 << " " << boxCounts[i] << " cards\n";
        }
    }

public:
    FlashCardApp() {
        srand(time(0));
        deck.load(filename);
    }

    ~FlashCardApp() {
        deck.save(filename);
    }

    void run() {
        while (true) {
            cout << "\n===== FLASHCARDS =====\n";
            cout << "Total Cards: " << deck.getCardCount();
            cout << "\n\n1. Create Card\n";
            cout << "2. Review Session\n";
            cout << "3. Show Statistics\n";
            cout << "4. Show Heatmap\n";
            cout << "5. Reset All Data\n";
            cout << "6. Exit\n";
            cout << "Choose (1-6): ";

            int choice;
            cin >> choice;
            clearInput();

            switch (choice) {
                case 1: createCard(); break;
                case 2: sessionManager.runSession(deck); break;
                case 3: showStatistics(); break;
                case 4: showHeatmap(); break;
                case 5: resetData(); break;
                case 6: return;
                default: cout << "Invalid choice!\n";
            }
        }
    }
};

int main() {
    FlashCardApp app;
    app.run();
    return 0;
}
