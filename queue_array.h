//
// Created by jake on 9/7/19.
//

#ifndef QUEUE_ARRAY_H
#define QUEUE_ARRAY_H
/*
  Here is some example code for Program 1.  You don't have to use any of
  it, but if you are having trouble, it might get you started.
  The etype used through this is the template class your need to create
  in this class, etype means Example TYPE
*/
#include <cstdlib>  //This is for the exit command.
#include <queue>
#include <iostream>

/*
 *  In the class below, it declares a "queue<etype>" array.
 *  you don't need to use the STL queue (but you can), choose a data structure
 * that you think works.   If you choose to write you own queue class, then
 * it has better follow all the requirements to a queue, to include the O time
 * minimum timing requirements of c++
*/


template <class etype>
class QueueArray
{
public:
    QueueArray(int =10 );
    ~QueueArray();
    int Asize();
    etype Dequeue();
    int DequeueIndex(int index);
    int Enqueue(const etype &item,const int index);
    int QAsize();
    int Qsize(int index) ;
    etype* Qstate(int index);

private:
    int size;      //size of the array
    std::queue <etype>* array;   //the array of queues.  It must be an array (NO not a vector)
    int totalItems; //total number of items stored in the queues

};

//Constructor for the queue array.  It sets the default size
//to 10, initializes the private variables size and totalItems
template <class etype>
QueueArray<etype>::QueueArray(int sz)
{
    this->size = sz;
    this->totalItems=0;
    array = new std::queue<etype>[size];
    if( array == NULL)
    {
        std::cout << "Not enough memory to create the array" << std::endl;
        exit(-1);
    }
}

// this should return the size of the array
template <class etype>
int QueueArray<etype>::Asize(){
    int Nsize = this->size;
    return Nsize;
}

// this should add the item to the queue at the array index
template <class etype>
int QueueArray<etype>::Enqueue(const etype &item ,const int index){

    if(index > this->size){
        return -1;
    }else if(index<size && index >=0) {
        array[index].push(item);
        return 1;
    }

    return 0;
}

// return the size of the queue that is at the index in the array
template <class etype>
int QueueArray<etype>::Qsize(int index){

    if(index> size || index < 0){
        return -1;
    }
    return array[index].size();
}
// should dequeue an item from the first non empty array
template <class etype>
etype QueueArray<etype>::Dequeue(){

    for (int i=0; i< size; i++){
        if(!array[i].empty()){
            etype temp = array[i].front(); // first grab the element at the front
            array[i].pop(); // then pop the item off the queue
            return temp;
        }
    }
    return 0;
}
template <class etype>
int QueueArray<etype>::DequeueIndex(int index){

    for (int i=0; i< size; i++){
        if(!array[i].empty() && index == i){
            int temp = array[i].front(); // first grab the element at the front
            array[i].pop(); // then pop the item off the queue
            return temp;
        }
    }
    return 0;
}
// loops through and adds up all of the items in all of the queues
template <class etype>
int QueueArray<etype>::QAsize(){

    int total=0;
    for(int i=0; i< size; i++){
        total = total + array[i].size();
    }
    return total;
}


// this copies the items in the queue at the index into an array returns pointer to array
template <class etype>
etype* QueueArray<etype>::Qstate(int index){

    int qSize = Qsize(index); // this uses the qsize function to get qSize

    int space = sizeof(etype)*qSize; // gets the size of this data type and how much of it

    etype* arrayNew = NULL; // point this array at null

    arrayNew = new etype[space]; // make it the proper size

    for(int i=0; i< qSize; i++){ // loop through the queue

        arrayNew[i] = array[index].front(); // copy front item from queue into array
        array[index].push(arrayNew[i]);  // put this item onto back of the queue
        array[index].pop(); // remove it from the front of the queue
    }
    return arrayNew;  // returns the pointer to the array

}
// destructor
template <class etype>
QueueArray<etype>::~QueueArray(){

}

#endif //OSPROGRAM1_QUEUE_ARRAY_H
