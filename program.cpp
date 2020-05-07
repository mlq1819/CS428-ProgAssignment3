#include <iostream>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <time.h>
#include <vector>

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
	cout << "\t" << endl;
	cout << "\t";
	for(unsigned int i=0; i<num_nodes; i++){
		cout << "-\t";
	}
	cout << "\t" << endl;
	for(unsigned int i=0; i<num_nodes; i++){
		cout << names[i] << " |\t";
		for(unsigned int j=0; j<num_nodes; j++){
			if(table[i][j]<0)
				cout << "\t";
			else if(table[i][j]==0)
				cout << "\\\t";
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

int getDistance(unsigned int from, unsigned int to){
	return table[from][to];
}

int getDistance(string from, string to){
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

unsigned int addNode(string name){
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

int main() { 
	fillGraph();
	printGraph();
}