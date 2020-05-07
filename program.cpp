#include <iostream>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <time.h>
#include <vector>
#include <ctype.h>

#define DEBUG true

using namespace std;

unsigned int num_nodes = 0;
vector<string> names;
vector<vector<int>> table;

void printGraph(){
	cout << "\t";
	for(unsigned int i=0; i<num_nodes; i++){
		cout << names[i] << "\t";
	}
	cout << "\n" << endl;
	for(unsigned int i=0; i<num_nodes; i++){
		cout << names[i] << "\t";
		for(unsigned int j=0; j<num_nodes; j++){
			if(table[i][j]<0)
				cout << "\t";
			else if(table[i][j]==0)
				cout << "-\t";
			else
				cout << table[i][j] << "\t";
		}
		cout << "\n" << endl;
	}
}

string getNameFromIndex(unsigned int index){
	return names[index];
}

unsigned int getIndexFromName(string name){
	for(unsigned int i=0; i<names.size(); i++){
		if(name.compare(names[i])==0)
			return i;
	}
	return num_nodes;
}

int dijkstra(unsigned int from, unsigned int to){
	if(from == num_nodes || to == num_nodes){
		return -2;
	}
	
	cout << "Performing dijkstra's algorithm on \"" << getNameFromIndex(from) << "\" to \"" << getNameFromIndex(to) << "\"" << endl;
	
	vector<bool> visited = vector<bool>();
	vector<int> distance = vector<int>();
	unsigned int current = from;
	
	for(unsigned int i=0; i<num_nodes; i++){
		visited[i]=false;
		distance[i]=-1;
	}
	
	distance[from] = 0;
	
	//variable that remains false so long as there exists a single reachable node that is not marked visited
	bool all_visited = false;
	
	//Ensures all reachable nodes are visited
	while(!all_visited){
		
		//Ensures the current node has not been visited
		if(!visited[current]){
			cout << "At \"" << getNameFromIndex(current) << "\":" << endl;
			
			//Loops through all the nodes, updating their saved distances through the local distance
			for(unsigned int i=0; i<num_nodes; i++){
				cout << "\tLocal to \"" << getNameFromIndex(i) << "\"..." << endl;
				if(table[current][i]>-1){ //ensure there is a valid local path to it
					cout << "\t\tLocal: " << table[current][i] << endl;
					if(distance[current] == -1)
						cout << "\t\tSaved: ∞" << endl;
					else
						cout << "\t\tSaved: " << distance[current] << endl;
					int local = table[current][i] + distance[current];
					if(distance[i] == -1 || local < distance[i]){
						distance[i] = local;
						cout << "\t\t\tUpdated value to " << distance[i] << endl;
					}
				}
			}
			visited[i] = true;
			all_visited = true;
			for(unsigned int i=0; i<num_nodes; i++){
				if(distance[i]>-1)
					all_visited = all_visited && visited[i];
			}
		}
		//iterates to the next index, but will loop around if necessary
		current = (current+1)%num_nodes;
	}
	return distance[to];
}

int dijkstra(string from, string to){
	return getDistance(getIndexFromName(from), getIndexFromName(to));
}

bool editNode(unsigned int from, unsigned int to, int new_distance){
	if(from == num_nodes || to == num_nodes)
		return false;
	if(new_distance==0 && from!=to)
		return false;
	if(new_distance!=0 && from==to)
		return false;
	
	if(new_distance<-1){
		return editNode(from,to,-1);
	}
	else{
		table[from][to]=new_distance;
		table[to][from]=new_distance;
	}
	return true;
}	

bool editNode(string from, string to, int new_distance){
	return editNode(getIndexFromName(from), getIndexFromName(to), new_distance);
}

unsigned int addNode(string input){
	string name = "";
	for(unsigned int i=0; i<input.size(); i++){
		name.push_back(tolower(input.at(i)));
	}
	
	//Ensures no node with that name exists
	if(getIndexFromName(name)!=num_nodes){
		return -1;
	}
	
	//Increases the number of nodes being tracked
	num_nodes++;
	
	//Adds name to name list
	names.push_back(name);
	
	//Updates all other distance lists
	for(unsigned int i=0; i<table.size(); i++){
		table[i].push_back(-1);
	}
	
	//Creates own distance list
	vector<int> new_node = vector<int>();
	new_node.resize(num_nodes, -1); //represents being unknown distance from other nodes
	new_node.back() = 0; //represents itself being 0 from itself
	table.push_back(new_node);
	
	return getIndexFromName(name); //ensures the node has successfully been added
}

bool removeNode(unsigned int index){
	if(index == num_nodes)
		return false;
	
	//Decreases the number of tracked nodes
	num_nodes--;
	
	//Removes name from name list
	names.erase(names.begin()+index);
	
	//Removes own distance list
	table.erase(table.begin()+index);
	
	//Updates all other distance lists
	for(unsigned int i=0; i<table.size(); i++){
		table[i].erase(table[i].begin()+index);
	}
	
	return true;
}

bool removeNode(string name){
	return removeNode(getIndexFromName(name));
}

void fillGraph(){
	addNode("t"); 
	addNode("u");
	addNode("v");
	addNode("w");
	addNode("x");
	addNode("y");
	addNode("z");
	
	editNode("t", "u", 2);
	editNode("t", "v", 4);
	editNode("t", "y", 7);
	
	editNode("u", "v", 3);
	editNode("u", "w", 3);
	
	editNode("v", "w", 4);
	editNode("v", "x", 3);
	editNode("v", "y", 8);
	
	editNode("w", "x", 6);
	
	editNode("x", "y", 6);
	editNode("x", "z", 8);
	
	editNode("y", "z", 12);
}

vector<string> parseInput(string input){
	bool new_word = true;
	vector<string> output = vector<string>();
	string current = "";
	
	//Iterates through the input by each character
	for(unsigned int i = 0; i<input.size(); i++){
		char c = tolower(input.at(i));
		
		//If it is a whitespace
		if(c == ' ' || c == '\t' || c == '\n'){
			
			//If there was a word being worked on
			if(!new_word){
				if(current.compare("")!=0){
					output.push_back(current);
					current = "";
				}
				new_word = true;
			}
		} 
		//If it is not a whitespace
		else {
			new_word = false;
			current.push_back(c);
		}
	}
	
	//If there was a word being worked on
	if(!new_word && current.compare("")!=0){
		output.push_back(current);
		current="";
		new_Word = true;
	}
	return output;
}

int main() { 
	fillGraph();
	string input;
	string message = "";
	vector<string> options = vector<string>();
	options.push_back("get <name1> <name2>\n\tPerforms dijkstra's algorithm from <name1> to <name2>")
	options.push_back("add <name>\n\tCreates a new node named <name>");
	options.push_back("edit <name1> <name1> <distance>\n\tEdits the local distance from <name1> to <name2> to <distance>");
	options.push_back("remove <name>\n\tRemoves node with name <name> from the graph");
	options.push_back("exit\n\tExits the program");
	while(true){
		printGraph();
		std::cout << messsage << endl;
		for(unsigned int i=0; i<options.size(); i++){
			cout << (i+1) << ".  " << options[i] << endl;
		}
		input = "";
		cin >> input;
		cout << "\n\n\n\n" << endl;
		vector<string> output = parseInput(input);
		if()
		
		if(input.compare("exit")==0)
			break;
	}
	cout << "Goodbye" << endl;
}