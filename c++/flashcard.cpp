#include <iostream>
#include <vector>
#include <string>
#include <limits> 

using namespace std;

class FlashCard {
public:
    string front;
    string back;
    
    FlashCard(string f = "", string b = "") : front(f), back(b) {}
};

class Deck {
private:
    vector<FlashCard> cards;
public:
    void addCard(const FlashCard& card) {
        if(!card.front.empty() && !card.back.empty()) { 
            cards.push_back(card);
            cout << "Card added successfully!\n"; 
        }
    }
    
    const vector<FlashCard>& getCards() const { return cards; } 
    
    size_t getCardCount() const { return cards.size(); }
};

class FlashCardApp {
private:
    Deck deck;
    
    void clearInput() { 
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    
public:
    void run() {
        while(true) {
            cout << "\n===== FLASHCARD APP =====\n";
            cout << "1. Create Card\n";
            cout << "2. Review Cards (" << deck.getCardCount() << " cards)\n"; 
            cout << "3. Exit\n";
            cout << "Choose (1-3): ";
            
            int choice;
            cin >> choice;
            clearInput(); 
            
            switch(choice) { 
                case 1: createCard(); break;
                case 2: reviewCards(); break;
                case 3: return;
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
        if(deck.getCardCount() == 0) { 
            cout << "\nNo cards to review!\n";
            return;
        }
        
        cout << "\n=== REVIEW MODE ===\n";
        for(const auto& card : deck.getCards()) { 
            cout << "\nQ: " << card.front << "\n";
            cout << "Press enter to show answer...";
            cin.ignore();
            cout << "A: " << card.back << "\n\n";
        }
        cout << "Review complete!\n";
    }
};

int main() {
    FlashCardApp app;
    app.run();
    return 0;
}
