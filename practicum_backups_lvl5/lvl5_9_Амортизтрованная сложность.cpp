#include <algorithm>
#include <deque>
#include <string>
#include <unordered_map>
#include <iostream>

using namespace std;

struct Ticket {
    int id;
    string name;
};

class TicketOffice {
public:
    // добавить билет в систему
    void PushTicket(const string& name) {
        tickets_.emplace_back(Ticket{last_id_++, name});
        ticket_count_[name]++;
    }

    // получить количество доступных билетов
    int GetAvailable() const {
        return tickets_.size();
    }

    // получить количество доступных билетов определённого типа
    int GetAvailable(const string& name) const {
        auto it = ticket_count_.find(name);
        return it != ticket_count_.end() ? it->second : 0;
    }

    // отозвать старые билеты (до определённого id)
    void Invalidate(int minimum) {
        while (!tickets_.empty() && tickets_.front().id < minimum) {
            ticket_count_[tickets_.front().name]--;
            tickets_.pop_front();
        }
    }

private:
    int last_id_ = 0;
    deque<Ticket> tickets_;
    unordered_map<string, int> ticket_count_; // Хранит количество билетов каждого типа
};

int main()
{
    TicketOffice tickets;

    tickets.PushTicket("Swan Lake"); // id - 0
    tickets.PushTicket("Swan Lake"); // id - 1
    tickets.PushTicket("Boris Godunov"); // id - 2
    tickets.PushTicket("Boris Godunov"); // id - 3
    tickets.PushTicket("Swan Lake"); // id - 4
    tickets.PushTicket("Boris Godunov"); // id - 5
    tickets.PushTicket("Boris Godunov"); // id - 6

    cout << tickets.GetAvailable() << endl; // Вывод: 7
    cout << tickets.GetAvailable("Swan Lake") << endl; // Вывод: 3
    cout << tickets.GetAvailable("Boris Godunov") << endl; // Вывод: 4

    // Invalidate удалит билеты с номерами 0, 1, 2:
    tickets.Invalidate(3);

    cout << tickets.GetAvailable() << endl; // Вывод: 4
    cout << tickets.GetAvailable("Swan Lake") << endl; // Вывод: 1
    cout << tickets.GetAvailable("Boris Godunov") << endl; // Вывод: 3

    tickets.PushTicket("Swan Lake"); // id - 7
    tickets.PushTicket("Swan Lake"); // id - 8

    cout << tickets.GetAvailable("Swan Lake") << endl; // Вывод: 3

    return 0;
}