#include <iostream>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <time.h>
#include <vector>
#include <ctype.h>

#define DEBUG false

using namespace std;

unsigned int num_nodes = 0;
vector<string> names;
vector<vector<int>> table;
vector<vector<int>> full_table;
bool graph = true;
bool updated = false;

string getNameFromIndex(unsigned int index){
	return names[index];
}

unsigned int getIndexFromName(string name){
#if DEBUG
cout << "\tgetIndexFromName(" << name << ")... ";
#endif
	for(unsigned int i=0; i<names.size(); i++){
		if(name.compare(names[i])==0){
#if DEBUG
cout << "... #" << i << endl;
#endif
			return i;
		}
	}
#if DEBUG
cout << "... unavailable" << endl;
#endif
	return num_nodes;
}

vector<int> rowDijkstra(unsigned int from, bool do_print){
#if DEBUG
cout << "rowDijkstra(" << from << ")" << endl;
#endif
	
	vector<bool> visited = vector<bool>();
	vector<int> distance = vector<int>();
	unsigned int current = from;
	
	for(unsigned int i=0; i<num_nodes; i++){
		visited.push_back(false);
		distance.push_back(-1);
	}
	distance[from] = 0;
	
	//variable that remains false so long as there exists a single reachable node that is not marked visited
	bool all_visited = false;
	
	//Ensures all reachable nodes are visited
	while(!all_visited){
#if DEBUG
cout << "\tvisiting " << current << "(";
if(visited[current])
	cout << "visited";
else
	cout << "unvisited";
cout << ",";
if(distance[current]==-1)
	cout << "∞";
else
	cout << distance[current];
cout << ")" << endl;
#endif
		//Ensures the current node has not been visited
		if(!visited[current]){
			if(do_print)
				cout << "At \"" << getNameFromIndex(current) << "\": " << distance[current] << endl;
			
			//Loops through all the nodes, updating their saved distances through the local distance
			for(unsigned int i=0; i<num_nodes; i++){
#if DEBUG
cout << "\t\tchecking" << current << "(";
if(visited[i])
	cout << "visited";
else
	cout << "unvisited";
cout << ",";
if(distance[i]==-1)
	cout << "∞";
else
	cout << distance[current];
if(table[current][i]==-1)
	cout << "distant";
else
	cout << "local " << table[current][i];
cout << ")" << endl;
#endif
				if(table[current][i]>-1){ //ensure there is a valid local path to it
					if(do_print){
						cout << "\tTo \"" << getNameFromIndex(i) << "\":" << endl;
						cout << "\t\tLocal: " << table[current][i] << endl;
						cout << "\t\tAccum: " << table[current][i] + distance[current] << "   =   " << distance[current] << " + " << table[current][i] << endl;
						if(distance[i] == -1)
							cout << "\t\tSaved: ∞" << endl;
						else
							cout << "\t\tSaved: " << distance[current] << endl;
					}
					int local = table[current][i] + distance[current];
					if(distance[i] == -1 || local < distance[i]){
						distance[i] = local;
						if(do_print)
							cout << "\t\t\tUpdated value to " << distance[i] << endl;
					}
				} else if(do_print){
					cout << "\t\"" << getNameFromIndex(i) << "\" not local" << endl;
				}
			}
			visited[current] = true;
			all_visited = true;
			for(unsigned int i=0; i<num_nodes; i++){
				if(distance[i]>-1)
					all_visited = all_visited && visited[i];
			}
		}
		//iterates to the next index, but will loop around if necessary
		current = (current+1)%num_nodes;
	}
	if(do_print){
		cout << "\n\t";
		for(unsigned int i=0; i<num_nodes; i++){
			cout << names[i] << "\t";
		}
		cout << "\n" << endl;
		cout << "\t";
		for(unsigned int i=0; i<num_nodes; i++){
			cout << distance[i] << "\t";
		}
		cout << "\n\n" << endl;
	}
	return distance;
}

vector<int> rowDijkstra(unsigned int from){
	return rowDijkstra(from, true);
}

int dijkstra(unsigned int from, unsigned int to){
#if DEBUG
cout << "dijkstra(" << from << "," << to << ")" << endl;
#endif
	if(from == num_nodes || to == num_nodes){
		return -2;
	}
	
	cout << "Performing dijkstra's algorithm on \"" << getNameFromIndex(from) << "\" to \"" << getNameFromIndex(to) << "\"" << endl;
	
	return rowDijkstra(from, true)[to];
}

int dijkstra(string from, string to){
#if DEBUG
cout << "dijkstra(" << from << "," << to << ")" << endl;
#endif
	return dijkstra(getIndexFromName(from), getIndexFromName(to));
}

void fullDijkstra(){
#if DEBUG
cout << "fullDijkstra()" << endl;
#endif
	updated=true;
	full_table.clear();
	for(unsigned int i=0; i<num_nodes; i++){
		full_table.push_back(rowDijkstra(i, false));
	}
}

bool editNode(unsigned int from, unsigned int to, int new_distance){
	if(from == num_nodes || to == num_nodes)
		return false;
	if(new_distance==0 && from!=to)
		return false;
	if(new_distance!=0 && from==to)
		return false;
	updated = false;
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
#if DEBUG
cout << "editNode(" << from << "," << to << "," << new_distance << ")" << endl;
#endif
	return editNode(getIndexFromName(from), getIndexFromName(to), new_distance);
}

unsigned int addNode(string input){
#if DEBUG
cout << "addNode(" << input << ")" << endl;
#endif
	string name = "";
	for(unsigned int i=0; i<input.size(); i++){
		name.push_back(tolower(input.at(i)));
	}
	
	//Ensures no node with that name exists
	if(getIndexFromName(name)!=num_nodes){
		return num_nodes;
	}
	
	//Increases the number of nodes being tracked
	num_nodes++;
	updated = false;
	
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
	updated = false;
	
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
#if DEBUG
cout << "removeNode(" << name << ")" << endl;
#endif
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
		new_word = true;
	}
	return output;
}

void printDoubleVector(vector<vector<int>> vector){
	cout << "\t";
	for(unsigned int i=0; i<num_nodes; i++){
		cout << names[i] << "\t";
	}
	cout << "\n" << endl;
	for(unsigned int i=0; i<num_nodes; i++){
		cout << names[i] << "\t";
		for(unsigned int j=0; j<num_nodes; j++){
			if(vector[i][j]<0)
				cout << "\t";
			else if(vector[i][j]==0)
				cout << "-\t";
			else
				cout << vector[i][j] << "\t";
		}
		cout << "\n" << endl;
	}
}

void printGraph(){
	cout << "Graph View" << endl;
	printDoubleVector(table);
}

void printTable(){
	if(!updated)
		fullDijkstra();
	cout << "Algorithm View" << endl;
	printDoubleVector(full_table);
}

int main() { 
	fillGraph();
	string input;
	string message = "";
	vector<string> options = vector<string>();
	vector<string> desc = vector<string>();
	options.push_back("get <name1> <name2>");
	desc.push_back("Performs dijkstra's algorithm from <name1> to <name2>");
	options.push_back("add <name>");
	desc.push_back("Creates a new node named <name>");
	options.push_back("edit <name1> <name1> <distance>");
	desc.push_back("Edits the local distance between <name1> and <name2> to <distance>");
	options.push_back("remove <name>");
	desc.push_back("Removes node with name <name> from the graph");
	options.push_back("switch");
	desc.push_back("Switches from graph view to algorithm table view");
	options.push_back("exit");
	desc.push_back("Exits the program");
#if DEBUG
unsigned long cycle_num = 0;
#endif
	bool run = true;
	while(run){
#if DEBUG
cout << "Cycle: " << ++cycle_num << endl;
#endif
		if(graph)
			printGraph();
		else
			printTable();
		for(unsigned int i=0; i<options.size(); i++){
			cout << (i+1) << ".  " << options[i] << endl;
			cout << "\t\t" << desc[i] << endl;
		}
		if(message.compare("")==0)
			cout << endl;
		else
			cout << "\n" << message << endl;
		message = "";
		input = "";
		cout << "Input command:" << endl;
		getline(cin, input);
#if DEBUG
cout << "\n\n\n\n" << endl;
#else
		system("clear");
#endif
		if(input.size()>0){
#if DEBUG
cout << "input: \t" << input << endl;
#endif
			vector<string> output = parseInput(input);
#if DEBUG
cout << "output:\t";
for(unsigned int i=0; i<output.size(); i++){
	cout << "\"" << output[i] << "\" ";
}
cout << endl;
#endif
			if(output.size()>0){
				//Gets distance using algorithm
				if(output[0].compare("get")==0){
#if DEBUG
cout << "Console: get" << endl;
#endif
					if(output.size()!=3){
#if DEBUG
cout << "Usage Error" << endl;
#endif
						message = "Usage: " + options[0] + "\nYou supplied: ";
						for(unsigned int i=0; i<output.size(); i++){
							message += output[i] + " ";
						}
					}
					else {
#if DEBUG
cout << "Usage Accurate" << endl;
#endif
						int result = dijkstra(output[1], output[2]);
						if(result == -2){
							message = "Failure: Invalid node name";
						} else if(result == -1){
							message = "Success: ∞ units between " + output[1] + " and " + output[2] + ": no valid path found";
						} else {
							message = "Success: " + to_string(result) + " units between " + output[1] + " and " + output[2];
						}
					}
				}
				//Adds node
				else if(output[0].compare("add")==0){
#if DEBUG
cout << "Console: add" << endl;
#endif
					if(output.size()!=2){
#if DEBUG
cout << "Usage Error" << endl;
#endif
						message = "Usage: " + options[1] + "\nYou supplied: ";
						for(unsigned int i=0; i<output.size(); i++){
							message += output[i] + " ";
						}
					}
					else {
#if DEBUG
cout << "Usage Accurate" << endl;
#endif
						unsigned int result = addNode(output[1]);
						if(result==num_nodes){
							message = "Failure: node already exists";
						} else {
							message = "Success: Node added with index " + to_string(num_nodes);
						}
					}
				}
				//Edits node
				else if(output[0].compare("edit")==0){
#if DEBUG
cout << "Console: edit" << endl;
#endif
					if(output.size()!=4){
#if DEBUG
cout << "Usage Error" << endl;
#endif
						message = "Usage: " + options[2] + "\nYou supplied: ";
						for(unsigned int i=0; i<output.size(); i++){
							message += output[i] + " ";
						}
					}
					else{
#if DEBUG
cout << "Usage Accurate" << endl;
#endif
						int distance = stoi(output[3], NULL, 10);
						if(editNode(output[1], output[2], distance)){
							message = "Success: Distance changed to " + table[getIndexFromName(output[1])][getIndexFromName(output[2])];
						} else {
							message = "Failure: Invalid node name";
						}
					}
				}
				//Removes node
				else if(output[0].compare("remove")==0){
#if DEBUG
cout << "Console: remove" << endl;
#endif
					if(output.size()!=2){
#if DEBUG
cout << "Usage Error" << endl;
#endif
						message = "Usage: " + options[3] + "\nYou supplied: ";
						for(unsigned int i=0; i<output.size(); i++){
							message += output[i] + " ";
						}
					} else {
#if DEBUG
cout << "Usage Accurate" << endl;
#endif
						if(removeNode(output[1])){
							message = "Success: Removed node with name " + output[1];
						} else {
							message = "Failure: Node does not exist";
						}
					}
				} 
				//Switches between views
				else if(output[0].compare("switch")==0){
					graph = !graph;
					message = "Switched to ";
					if(graph)
						message += "graph view";
					else
						message += "algorithm view";
				}
				//Ends program
				else if(output[0].compare("exit")==0 || output[0].compare("quit")==0 || output[0].compare("end")==0){
#if DEBUG
cout << "Console: exit" << endl;
#endif
					run = false;
					break;
				}
				else if(output[0].compare("creator")==0 || output[0].compare("author")==0){
					message = "Michael Quinn";
				}
			}
		}
	}
	cout << "Goodbye" << endl;
}