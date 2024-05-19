#include <iostream>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

class Table{
public:
    Table(int id, int price){
        this->id = id;
        this->price = price;
        this->isFree = true;
        this->total_time = 0;
        this->total_price = 0;
    }
    ~Table(){}
    int getId(){
        return this->id;
    }
    bool getIsFree(){
        return this->isFree;
    }
    int getTotalTime(){
        return this->total_time;
    }
    int getTotalPrice(){
        return this->total_price;
    }
    void setIsFree(bool isFree){
        this->isFree = isFree;
    }
    void setTotalTime(int total_time){
        this->total_time = total_time;
    }
    // void setTotalPrice(int total_price){
    //     this->total_price = total_price;
    // }
    void addTotalTime(int time){
        this->total_time += time;
    }
private:
    int id, total_time, total_price, price;
    bool isFree;
};

class Custumer{
public:
    Custumer(string name){
        this->name = name;
        this->table_id = 0;
        this->start_time = "00:00";
    }
    ~Custumer(){}
    string getName(){
        return this->name;
    }
    int getTableId(){
        return this->table_id;
    }
    string getStartTime(){
        return this->start_time;
    }
    void setTableId(int table_id){
        this->table_id = table_id;
    }
    void setStartTime(string start_time){
        this->start_time = start_time;
    }
private:
    string name, start_time;
    int table_id;
};

int timeToInt(string time){
    int hours = stoi(time.substr(0, 2));
    int minutes = stoi(time.substr(3, 2));
    return hours * 60 + minutes;
}

string intToTime(int time){
    int hours = time / 60;
    int minutes = time % 60;
    return to_string(hours) + ":" + to_string(minutes);
}

void freeTable(Table table){
    table.setIsFree(true);
}

int findCustumerID(vector<Custumer> custumers, string name){
    for (int i = 0; i < custumers.size(); i++){
        if (custumers[i].getName() == name){
            return i;
        }
    }
    return -1;
}

int countFreeTables(vector<Table> tables){
    int count = 0;
    for (int i = 0; i < tables.size(); i++){
        if (tables[i].getIsFree()){
            count++;
        }
    }
    return count;
}

int countPrice(int time, int price){
    return time * price;
}
string requests_handler(vector<Table> &tables, vector<Custumer> &custumers, string line, string begin_time, string end_time, int count_of_tables, vector<string> &queue){
    string start_time = line.substr(0, 5);
    string res, cust_name;
    int id = (int)line[6]-48, table_id, cust_id;
    res = line + '\n';
    switch (id){
        case 1:
            if (timeToInt(start_time) >= timeToInt(begin_time) && timeToInt(start_time) <= timeToInt(end_time)){
                custumers.push_back(Custumer(line.substr(8, line.length()-8)));
            }
            else{
                res += start_time + ' ' + "13 NotOpenYet\n";
            }
            break;
        case 2:
            table_id = (int)line[line.length()-1]-48;
            cust_name = line.substr(8, line.rfind(' ')-8);
            cust_id = findCustumerID(custumers, cust_name);
            if (cust_id == -1){
                res += start_time + ' ' + "13 ClientUnknown\n";
            }
            else if (!tables[table_id-1].getIsFree()){
                res += start_time + ' ' + "13 PlaceIsBusy\n";
            }
            else {
                custumers[cust_id].setTableId(table_id);
                custumers[cust_id].setStartTime(start_time);
                tables[table_id-1].setIsFree(false);
            }
            break;
        case 3:
            if (countFreeTables(tables) != 0){
                queue.push_back(line.substr(8, line.rfind(' ')-8));
                res += start_time + ' ' + "13 ICanWaitNoLonger!\n";
            }
            if (queue.size() < count_of_tables){
                queue.push_back(line.substr(8, line.rfind(' ')-8));
            }
            else{
                res += start_time + " 11 " + line.substr(8, line.rfind(' ')-8) + "\n";
            }
            break;
        case 4:
            cust_name = line.substr(8, line.rfind(' ')-8);
            cust_id = findCustumerID(custumers, cust_name);
            if (cust_id == -1){
                res += start_time + ' ' + "13 ClientUnknown\n";
            }
            else{
                
                cust_name = custumers[0].getName();
                custumers.erase(custumers.begin());
                custumers[cust_id].setTableId(table_id);
                custumers[cust_id].setStartTime(start_time);
                tables[table_id-1].setIsFree(false);
            }
            break;
        default:
            break;
    };
    return res;
}

int main(int argc, char *argv[]){
    ifstream myfile(argv[1]);
    ofstream myfile2("out.txt");
    int count_of_tables, price;
    string begin_time, end_time, line, var;  
    vector<Table> tables;
    vector<Custumer> custumers;
    vector<string> queue;
    int i = 0;
    if (myfile.is_open())
    {
        getline(myfile, var);
        count_of_tables = stoi(var);
        getline(myfile, begin_time, ' ');
        getline(myfile, end_time);
        getline(myfile, var);
        price = stoi(var);
        for (int i = 0; i < count_of_tables; i++){
            tables.push_back(Table(i+1, price));
        }
        myfile2 << begin_time << '\n';
        while (getline(myfile, line)){
            myfile2 << requests_handler(tables, custumers, line, begin_time, end_time, count_of_tables, queue);
        }
        myfile2 << end_time << '\n';
        myfile.close();
        myfile2.close();
    }
    else    cout << "Unable to open file";
    return 0;
}