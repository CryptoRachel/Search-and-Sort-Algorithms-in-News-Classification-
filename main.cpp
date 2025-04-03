#include <iostream>
#include "articles.h"

using namespace std;

//Function for testing if the sorting algorithm function was work.
void printTopFive(Article<string>* head) 
{
    Article<string>* current = head;
    int count = 0;

    while (current && count < 5) 
    { 
        //Only display the first 5 nodes.
        cout << "Date: " << current->Date << " | Title: " << current->Title << endl;
        current = current->next;
        count++;
    }
    if (count == 0) 
    {
        cout << "Linkedlists was empty." << endl;
    }
}

//Function for Save timsort output into csv file
void save_sorted_data_to_csv(LinkedList<string>& newsList, const string& filename) 
{
    ofstream outFile(filename);

    if (!outFile.is_open()) 
    {
        cerr << "Error: Could not open file for writing." << endl;
        return;
    }

    outFile << "Date,Title,Content,Category,Label\n"; // Writing headers

    Article<string>* current = newsList.getHead();
    while (current) 
    {
        outFile << current->Date << ","
                << "\"" << current->Title << "\","
                << "\"" << current->Content << "\","
                << current->Category << ","
                << current->Label << "\n";
        current = current->next;
    }

    outFile.close();
    cout << "Sorted data saved to " << filename << endl;
}

int main() 
{
//----------------------------------------Linked list and Sorting algorithm----------------------------------------
    LinkedList<string> newsList;
    // Import data from CSV file
    try 
    {
        import_csv_to_linkedlist("merge.csv", newsList);
    } 
    catch (const runtime_error& e) 
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    

    // Perform Timsort on the linked list
    TimSort timSorter;
    Article<string>* head = newsList.getHead();
    timSorter.timSort(head);
    newsList.setHead(head); // Update the head pointer after sorting

    // ----- Sorting algorithm measurement -----
    size_t memoryBeforeSort = getCurrentMemoryUsage();
    auto startSort = high_resolution_clock::now();

    auto endSort = high_resolution_clock::now();
    size_t memoryAfterSort = getCurrentMemoryUsage();

    double timeTakenSort = duration<double, milli>(endSort - startSort).count();
    size_t memoryUsedSort = (memoryAfterSort > memoryBeforeSort) ? (memoryAfterSort - memoryBeforeSort) : 0;

    // Output sorting performance results
    cout << "Timsort Sorting Time: " << timeTakenSort << " ms" << endl;
    cout << "Timsort Memory Usage: " << memoryUsedSort / 1024 << " KB" << endl;
    
    //Save the sorted data to a new CSV file
    save_sorted_data_to_csv(newsList, "sorted_news.csv"); 
    
    //Testing if the sorting operated correctly.
    cout << "----- Sorted Articles (First 5) -----" << endl;
    printTopFive(newsList.getHead());

//-----------------------------------------------------------------------------------------------------------------
/*    
//Display the menu for the user's choice
    int choice;
    cout << endl << string(30,'-') << "MENU" << string(30,'-') << endl;
    cout << "1. Total number of news." << endl;
    cout << "2. Percentage of the political news in fake news in 2016." << endl;
    cout << "3. Top 10 frequent words in government fake news." << endl;
    cout << "4. Percentage of fake political news articles for each month in the year 2016." << endl;
    cout << "5. Search articles by insert keywords." << endl;
    cout << string(60,'-') << endl;
    cout << "Enter your choice: ";
    cin >> choice;
    cout << string (60,'-') << endl;

    LinearSearch searcher;

    // Execute the functionality based on the user's choice
    switch(choice)
    {
        case 1:
            {
            //Record memory and time usage before searching
            size_t memoryBeforeSearch = getCurrentMemoryUsage();
            auto startSearch = high_resolution_clock::now();

            // Total number of news (both fake and true)
            searcher.countNews(newsList);

            //Record memory and time usage after searching
            auto endSearch = high_resolution_clock::now();
            size_t memoryAfterSearch = getCurrentMemoryUsage();

            double timeTakenSearch = duration<double, milli>(endSearch - startSearch).count();
            size_t memoryUsedSearch = memoryAfterSearch - memoryBeforeSearch;

            cout << "Linear Search Total News Time: " << timeTakenSearch << " ms" << endl;
            cout << "Linear Search Memory Usage: " << memoryUsedSearch / 1024 << " KB" << endl;
            }
            break;
        case 2:
            {
                // Percentage of fake political news in 2016
                double percentage = searcher.percentageFakePolitical2016(newsList);
                cout << "Percentage of fake political news in 2016: " << percentage << "%" << endl;
            }
            break;
        case 3:
            // Top 10 frequent words in fake government news
            searcher.top10FrequentWordsInGovernmentFakeNews(newsList);
            break;
        case 4:
            // Percentage of fake political news articles for each month in 2016
            searcher.percentageFakePoliticalByMonth(newsList);
            break;
        case 5:
            // Search articles by inserted keywords
            searcher.searchArticlesByKeyword(newsList);
            break;
        default:
            cout << "Invalid choice." << endl;
    }    
            */

    return 0; 
}
