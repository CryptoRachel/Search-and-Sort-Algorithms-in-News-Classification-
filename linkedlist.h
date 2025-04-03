#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>
#include <fstream>

using namespace std;

template <typename T>
struct Article
{
    T Title;
    T Content;
    T Category;
    T Date;
    T Label;
    Article* next;

    Article() = default;
    Article(T t, T con, T cat, T d, T l)
        :Title(t), Content(con), Category(cat), Date(d), Label(l), next(nullptr){}
};

template <typename T>
class LinkedList
{
    private:
        Article<T>* head;
        Article<T>* tail;  
        size_t size;  

    public:
        class Iterator 
        {
            private:
                Article<T>* current;

            public:
                Iterator(Article<T>* node) : current(node) {}

                T& operator*() const { return current -> Title;} //Return the title of the current node.
                Article<T>* operator -> () { return current;} //Return the pointer of the current node.
                Iterator& operator++()
                {
                    if (current) current = current -> next;
                    return *this;
                }

                Iterator operator++(int)
                {
                    Iterator temp = *this;
                    ++(*this);
                    return temp;
                }

                bool operator==(const Iterator& other) const //Compare ==
                {  
                    return current == other.current;
                }

                bool operator!=(const Iterator& other) const //Compare !=
                {  
                    return current != other.current;
                }

                Article<T>* getHead() { return head; }
        };
        
        LinkedList() : head(nullptr), tail(nullptr), size(0) {}

        ~LinkedList()
        {
            while (head != nullptr)
            {
                popfront();
            }
        }

        //Adding acticles to the linkedlist tail，for adding new articles data continuously.
        void pushback(T Title, T Content, T Category, T Date, T Label)
        {
            Article<T>* newArticle = new Article<T>(Title, Content, Category, Date, Label);
            //If the list is empty, the new articles become head and tail. 
            if (head == nullptr)
            {
                head = tail = newArticle;
            }
            //If not, add the new articles to the end of the list.
            else
            {
                tail -> next = newArticle;
                tail = newArticle;
            }
            size++;
        }

        //Remove first article from the list.
        void popfront()
        {
            if (head == nullptr)
            {
                throw out_of_range("Pop front failed due to the list is empty.");
            }
            Article<T>* temp = head;
            head = head -> next;
            if(head == nullptr)
            {
                tail = nullptr;
            }
            delete temp;
            size--;
        }
        
        //Searching the point form and return true and false.
        bool contains(const T& Title) const
        {
            for (auto it = begin(); it != end(); ++it)
            {
                if (*it == Title)
                {
                    return true;
                }
            }
            return false; //if didn't searh the match value.
        }

        //Get the point form in the linked lists and display all of the searhng result.
        Article<T>* get(size_t index) const
        {
            if(index >= size)
            {
                throw out_of_range("Index out of range.");
            }
            Article<T>* current = head;
            for (size_t i = 0; i < index; i++)
            {
                current = current -> next;
            }
            return current;
        }

        //Function to get the specific point location. Similar function with get but returns the node.
        Article<T>* getNode(size_t index) const
        {
                return get(index);
        }

        //Insert the specific data to the specific(random) place.
        void insert(size_t index, T Title, T Content, T Category, T Date, T Label)
        {
            if(index > size)
            {
                throw out_of_range("Index out of range.");
            }

            if (index == 0)
            {
               Article<T>* newArticles = new Article<T>(Title, Content, Category, Date, Label);
               newArticles -> next = head;
               head = newArticles;

               if(tail == nullptr)
               {
                   tail = newArticles;
               }
           }
           else if(index == size)
           {
               pushback(Title, Content, Category, Date, Label);
               return;
           }
           else
           {
               Article<T>* prev = get(index - 1);
               Article<T>* newArticle = new Article<T>(Title, Content, Category, Date, Label);
               newArticle -> next = prev -> next;
               prev -> next = newArticle;
           }
            size++;
        }

        //Erase the articles at a specific index.
        void erase(size_t index)
        {
            if (index >= size)
            {
                throw out_of_range("Index out of range.");
            }
        
            if (index == 0)
            {
                popfront();
            }
            else
            {
                Article<T>* prev = get(index - 1);
                Article<T>* to_delete = prev -> next;
                prev -> next = to_delete -> next;
                delete to_delete;
                size--;

                //If the last node was deleted updated the tail.
                if (prev -> next == nullptr)
                {
                    tail = prev;
                }
            }
        }

        //Get the linkedlists size
        size_t getSize() const
        {
            return size;
        }

        //Clear the linkedlists.
        void clear()
        {
            while (head)
            {
                popfront();
            }
            size = 0;
        }

        //Get the iterator which point to the list head.
        Iterator begin() const
        {
            return Iterator(head);
        }

        //Get the iterator which point to the list tail.
        Iterator end() const
        {
            return Iterator(nullptr);
        }

        //Display all articles in the list.
        void display() const
        {
            Article<T>* temp = head;
            while (temp != nullptr)
            {
                cout << "----------------------------------------------------------" << endl
                     << "Title:" << temp -> Title << "\n"
                     << "Content:" << temp -> Content << "\n"
                     << "Category:" << temp -> Category << "\n"
                     << "Date:" << temp -> Date << "\n"
                     << "Label:" << temp -> Label << "\n\n"
                     << "----------------------------------------------------------" << endl;
                temp = temp -> next;
            }
        }

        Article<T>* getHead() const {
            return head;
        }
    
        //Set the node's head.
        void setHead(Article<T>* newHead) {
            head = newHead;
        } 
};

#endif