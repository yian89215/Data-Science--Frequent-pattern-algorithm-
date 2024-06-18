#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <iomanip>
#include <math.h>
using namespace std;

class Node {
    public:
        int item;
        int count;
        Node* parent;
        vector<Node*> children;
        Node* next;

        Node(int item, int count, Node* parent) : item(item), count(count), parent(parent), next(nullptr) {}

        void addChild(Node* child) {
            children.push_back(child);
        }

        bool hasChild(int item) {
            for (auto* child : children) {
                if (child->item == item) {
                    return true;
                }
            }
            return false;
        }

        Node* getChild(int item) {
            for (auto* child : children) {
                if (child->item == item) {
                    return child;
                }
            }
            return nullptr;
        }

        void increase(int amount) {
            count += amount;
        }
};



using HeaderTable = unordered_map< int, pair< Node*, int> >; //HeaderTable : item, node*, total support

int total_transaction, itemTypes, minSupport;

vector<pair<vector<int>, int> >frequentPatterns;

void init(char**&, vector<vector<int> >&); //read data
void constructTree(HeaderTable&, Node*, vector<vector<int> >&, vector<int>&);// construct tree and headerTable
void mineTree(HeaderTable&, Node*, vector<int> &, vector<int>);//mine the tree
void conditionalPatternBase(Node*, vector<vector<int> >&);//contruct conditional pattern base
void show(string, Node*);
void output(vector<pair<vector<int>, int> >, char**& );

int main(int argc, char** argv){
    vector<vector<int> > transactions;
    HeaderTable headerTable;//fast to find the node with same item
    vector<int> supportTable, prefix;
    Node* root = new Node(-1, 0, nullptr);

    init(argv, transactions);
    constructTree(headerTable, root, transactions, supportTable);
    //show("",root);
    mineTree(headerTable, root, prefix, supportTable);
    output(frequentPatterns, argv);
    return 0;
}

void init(char** &argv, vector<vector<int> >&transactions){
    ifstream inputfile(argv[2]);//creating an input file stream object named inputfile
    string line;
    while(getline(inputfile, line)){
        stringstream ss(line);//a stringstream object named ss which is used to parse string
        vector<int> each_transaction;
        for(int item ; ss >> item;){
            each_transaction.push_back(item);
            if(item > itemTypes){//the maximum item number is the number of item types
                itemTypes = item;
            }
            if (ss.peek() == ','){
                ss.ignore();
            }
        }
        transactions.push_back(each_transaction);
        total_transaction++;
    }
    inputfile.close();
    minSupport = ceil(total_transaction * stod(argv[1]));
    itemTypes += 1;
}

void constructTree(HeaderTable& headerTable,Node* root, vector<vector<int> >& transactions, vector<int>& supportTable){
    //count number of support for all items.
    vector<int> supportCount(itemTypes, 0);
    for(auto &transaction : transactions){
        for(auto item : transaction){
            supportCount[item]++;
        }
    }
    //construct headerTable
    for(int i = 0 ; i < itemTypes ; i++){       
        if(supportCount[i] < minSupport){continue;}
        headerTable[i] = make_pair(nullptr, supportCount[i]);
        supportTable.push_back(i);//construct supportTable
    }

    for(auto &transaction : transactions){
        sort(transaction.begin(), transaction.end(), [&supportCount](int a, int b){ //sort items in each transaction by their support count in decreasing order.
            if (supportCount[a] != supportCount[b]) {return supportCount[a] > supportCount[b];}
            return a > b;
        });
        Node* cur = root;
        //construct tree and headerTable's link through each item in each transaction
        for (auto i : transaction){
            if(supportCount[i] < minSupport){break;}

            Node* ch = cur->getChild(i);
            if (ch != nullptr) {
                ch->increase(1);
                cur = ch;
            }
            else{
                Node* newNode = new Node(i, 1, cur);
                cur->addChild(newNode);
                cur = newNode;
                Node* link = headerTable[i].first;
                if(link == nullptr){
                    headerTable[i].first = cur;
                }
                else{
                    cur->next = headerTable[i].first;
                    headerTable[i].first = cur;
                }
            }
        }
    }
}

void mineTree(HeaderTable& headerTable, Node* root, vector<int> &prefix, vector<int> supportTable){
    for(auto i : supportTable){
        vector<int> temp(prefix);
        temp.push_back(i);
        //add frequent item and correspond frequency into result
        frequentPatterns.push_back({temp,headerTable[i].second});

        vector<vector<int> > newTransactions;//conditional pattern base
        conditionalPatternBase(headerTable[i].first, newTransactions);
        HeaderTable newheaderTable;
        Node* newroot = new Node(-1, 0, nullptr);
        vector<int> newSupportTable;

        //construct conditional tree
        constructTree(newheaderTable, newroot, newTransactions,newSupportTable);

        if(newSupportTable.empty()){continue;}//no more frequent item with the condition.
        
        //mine conditional tree
        mineTree(newheaderTable, newroot, temp, newSupportTable);
    }
}

void conditionalPatternBase(Node* item, vector<vector<int> >& cond_patternbase){
    cond_patternbase.clear();
    while(item != nullptr){
        Node* cur = item;
        vector<int> cond_pattern;
        while(cur->parent->item != -1){
            cond_pattern.push_back(cur->parent->item);
            cur = cur->parent;
        }
        if(cond_pattern.empty()){
            item = item->next;
            continue;
        }
        for(int i = 0; i < item->count; i++){
            cond_patternbase.push_back(cond_pattern);
        }
        item = item->next;
    }
}
void output(vector<pair<vector<int>, int> > frequentPatterns, char**& argv){
    ofstream outputfile(argv[3]);
    for(auto frequentItem : frequentPatterns){
        for(auto item = frequentItem.first.begin(); item != frequentItem.first.end(); item ++){
            outputfile << *item;
            if(item != frequentItem.first.end() - 1) outputfile << ',';
        }
        outputfile << ':' << fixed << setprecision(4) << frequentItem.second / double(total_transaction) << endl;
    }
    outputfile.close();
}
