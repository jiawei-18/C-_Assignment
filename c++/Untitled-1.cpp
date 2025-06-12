#include <iostream>
#include <vector>
#include <string>

using namespace std;

class FlashCard {
public:
    string front;
    string back;
    
    FlashCard(string f, string b) : front(f), back(b) {}
};

class Deck {
private:
    vector<FlashCard> cards;
public:
    void addCard(const FlashCard& card) {
        cards.push_back(card);
    }
    
    vector<FlashCard>& getCards() { return cards; }
};

class FlashCardApp {
private:
    Deck deck;
public:
    void run() {
        while(true) {
            cout << "1. Create Card\n";
            cout << "2. Review Cards\n";
            cout << "3. Exit\n";
            
            int choice;
            cin >> choice;
            cin.ignore();
            
            if(choice == 1) createCard();
            else if(choice == 2) reviewCards();
            else if(choice == 3) break;
        }
    }
    
    void createCard() {
        string front, back;
        cout << "Enter front: ";
        getline(cin, front);
        cout << "Enter back: ";
        getline(cin, back);
        deck.addCard(FlashCard(front, back));
    }
    
    void reviewCards() {
        for(auto& card : deck.getCards()) {
            cout << "\nFRONT: " << card.front << endl;
            cout << "Press enter to reveal answer...";
            cin.ignore();
            cout << "BACK: " << card.back << "\n\n";
        }
    }
};

int main() {
    FlashCardApp app;
    app.run();
    return 0;
}
