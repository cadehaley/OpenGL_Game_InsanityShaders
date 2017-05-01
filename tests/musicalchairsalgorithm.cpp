#include <iostream>
#include <vector>
#include <stdio.h>

using namespace std;

void reformatElements(std::vector<int> &in_elements);

int main(int argc, char * argv[])
{

	
	vector<int> elements;

	elements.push_back(3);
	elements.push_back(5);
	elements.push_back(2);
	elements.push_back(4);
	elements.push_back(1);
	elements.push_back(6);
	elements.push_back(3);
	elements.push_back(5);

	reformatElements(elements);

	return 0;
}

// Increment elements such that index order remains the same but
// all numbers are made unique "musical chairs algorithm"

void reformatElements(std::vector<int> &in_elements){
	
	int size = in_elements.size();

	vector<int> freq(size);
	vector<int> offset(size);
	
	int shifts = 0;
	// Fill freq (index = number)
	for (int i = 0; i<size; i++){
		if ((freq[in_elements[i]]) != 0){
			shifts++;
		}
		freq[in_elements[i]]++;
	}
	// Determine offset for each num	
	for (int i = size-1; i>=0; i--){
		if (freq[i]>1){
			shifts -= (freq[i]-1);
		}
		offset[i] = shifts;
	}
	// Create new elements vector
	vector<int> elements;
	int changed_elem;
	for (int i = 0; i<size; i++){
		changed_elem = in_elements[i]+offset[in_elements[i]];
		elements.push_back(changed_elem);
	}
	
	// Assign duplicate elements their unique numbers
	vector<bool> indexfilled(size);

	for (int i = 0; i<elements.size(); i++){
		while (indexfilled[elements[i]]){
			elements[i]++;
		}
		indexfilled[elements[i]] = true;
	}

	// Print loop
	printf("\nElements: ");
	for (int i = 0; i<size; i++){printf(" %d", in_elements[i]);}


	printf("\nFreq:   ");
	for (int i = 0; i<size; i++){printf(" %d", freq[i]);}


	printf("\nOffset: ");
	for (int i = 0; i<size; i++){printf(" %d", offset[i]);}


	printf("\nNew Elements: ");
	for (int i = 0; i<size; i++){printf(" %d", elements[i]);}
}
