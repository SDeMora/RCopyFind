// Function: HeapSort

// Purpose: Sorts two tables together to put the first table in numerical order

// Details: Sorts both tableA and tableB together, putting tableA in numerical order and taking tableB
//			with it.

// Description: An implementation of the classic HeapSort algorithm. The first half of the routine puts
// the heap in order so that the value at the top of each two-branch node is greater than either of the two
// values in the branches below it. The second half of the routine takes values off the top of the heap
// and then resorts the heap so as to fill the opening and promote the remaining values. Ultimately, the
// entire heap is put into ascending numerical order

void HeapSort(unsigned long *tableA, int *tableB, int n)
{
	unsigned long tempA;							// tempA can hold a hash-coded word temporarily
	int tempB;										// tempB can hold a word number temporarily
	int nr;											// number of remaining entries in the heap
	int index1,index2,indexc,indexstart;			// pointers into the heap

	indexstart = (n >> 1);							// start at the lowest two levels of the heap	
	index1 = (n >> 1);								// index1 points to second-to-lowest level of heap
	index2 = (index1 << 1);							// index2 points to lowest level of heap

	for ( indexc=indexstart;indexc>=1;indexc--)		// loop and work backwards to top of heap
	{
		index1 = indexc;							// pointer to the current node
		index2 = (indexc << 1);						// pointer to left branch below node
		
		while ( index2 <= n )						// while we haven't gone off bottom of heap
		{
			if( index2 < n )						// if there are two values branching below this node,
			{
				if( tableA[index2]					// choose the largest value
					< tableA[index2+1] )
				{
					index2++;						// and point to it
				}
			}
			if( tableA[index1] < tableA[index2] )	// if the node is less than the largest value below,
			{
				tempA=tableA[index1];				// swap node and largest value below, part 1
				tempB=tableB[index1];				// swap word number table entry to match, part 1
				tableA[index1]=tableA[index2];		// swap part 2
				tableB[index1]=tableB[index2];		// swap part 2
				tableA[index2]=tempA;				// swap part 3
				tableB[index2]=tempB;				// swap part 3
				index1 = index2;					// pointer to the next lower level of heap
				index2 = (index2 << 1);				// pointer to the left branch below that node
			}
			else									// else, this node is properly ordered, so move along
			{
				break;
			}
		}
	}


	for ( nr=n-1;nr>=1;nr-- )						// start at end of heap and move backward to start
	{
		tempA=tableA[1];							// swap top node and value beyond end of shortened heap, part1
		tempB=tableB[1];							// swap word number table entry to match, part 1
		tableA[1]=tableA[nr+1];						// swap part 2
		tableB[1]=tableB[nr+1];						// swap part 2
		tableA[nr+1]=tempA;							// swap part 3
		tableB[nr+1]=tempB;							// swap part 3

		index1 = 1;									// pointer to the current node (top of heap)
		index2 = 2;									// pointer to left branch below node
		
		while ( index2 <= nr )						// while we haven't gone off bottom of heap
		{
			if( index2 < nr )						// if there are two values branching below this node,
			{
				if( tableA[index2]					// choose the largest value
					< tableA[index2+1] )
				{
					index2++;						// and point to it
				}
			}
			if( tableA[index1] < tableA[index2] )	// if the node is less than the largest value below,
			{
				tempA=tableA[index1];				// swap node and largest value below, part 1
				tempB=tableB[index1];				// swap word number table entry to match, part 1
				tableA[index1]=tableA[index2];		// swap part 2
				tableB[index1]=tableB[index2];		// swap part 2
				tableA[index2]=tempA;				// swap part 3
				tableB[index2]=tempB;				// swap part 3
				index1 = index2;					// pointer to the next lower level of heap
				index2 = (index2 << 1);				// pointer to the left branch below that node
			}
			else									// else, this node is properly ordered, so move along
			{
				break;
			}
		}
	}

	return;											// both tables are all sorted, so return
}