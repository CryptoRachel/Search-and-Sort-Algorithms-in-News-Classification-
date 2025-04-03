#include "linkedlist.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <map>

#include <chrono> 
#include <windows.h>      
#include <psapi.h>

using namespace std;
using namespace chrono;

struct WordFrequency 
{
    string word;
    int frequency;
    WordFrequency* next;
    WordFrequency(const string& w, int freq)
        : word(w), frequency(freq), next(nullptr) {}
};

class WordFrequencyList {
private:
    WordFrequency* head;
public:
    WordFrequencyList() : head(nullptr) {}

    // Add a word or update its frequency
    void addWord(const string& word) 
    {
        WordFrequency* current = head;
        WordFrequency* prev = nullptr;
        while (current != nullptr) {
            if (current->word == word) 
            {
                current->frequency++;
                return;
            }
            prev = current;
            current = current->next;
        }
        // If not found, add a new node
        WordFrequency* newNode = new WordFrequency(word, 1);
        if (prev == nullptr)
            head = newNode;
        else
            prev->next = newNode;
    }

    // Sort by frequency from highest to lowest (simple bubble sort implementation)
    void sortByFrequency() 
    {
        if (!head || !head->next) return;
        bool swapped;
        do {
            swapped = false;
            WordFrequency* current = head;
            WordFrequency* prev = nullptr;
            while (current && current->next) 
            {
                if (current->frequency < current->next->frequency) 
                {
                    WordFrequency* temp = current->next;
                    current->next = temp->next;
                    temp->next = current;
                    if (prev == nullptr)
                        head = temp;
                    else
                        prev->next = temp;
                    prev = temp;
                    swapped = true;
                } 
                else 
                {
                    prev = current;
                    current = current->next;
                }
            }
        } while (swapped);
    }

    WordFrequency* getHead() {
        return head;
    }
};

// Helper function: convert a string to lowercase
string toLowercase(const string& str) 
{
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

string getMonthAbbreviation(int month) 
{
    const string months[] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                              "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
    return (month >= 1 && month <= 12) ? months[month - 1] : "UNK";
}

// Helper function: extract words from content and update the word frequency list
void extractWords(const string& content, WordFrequencyList& wordFreqList) {
    stringstream ss(content);
    string word;
    while (ss >> word) 
    {
        // Remove punctuation from the beginning of the word
        while (!word.empty() && ispunct(word.front())) 
        {
            word.erase(word.begin());
        }
        // Remove punctuation from the end of the word
        while (!word.empty() && ispunct(word.back())) 
        {
            word.pop_back();
        }
        if (!word.empty()) 
        {
            word = toLowercase(word);
            wordFreqList.addWord(word);
        }
    }
}

size_t getCurrentMemoryUsage() 
{
    #ifdef _WIN32
    PROCESS_MEMORY_COUNTERS memInfo;
    GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo));
    return memInfo.WorkingSetSize;  // Return memory usage in bytes
    #else
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss * 1024;  // Return bytes
    #endif
}

// Function to parse a CSV line, taking into account commas within quoted fields.
void parse_csv(const string& line, string& Title, string& Content, string& Category, string& Date, string& Label)
{
    stringstream ss(line);
    bool inQuotes = false;
    char prevChar = '\0';
    string field;
    int fieldIndex = 0;

    for (char ch : line)
    {
        if(ch == '"' && prevChar != '\\')
        {
            inQuotes = !inQuotes;
        }
        else if (ch == ',' && !inQuotes)
        {
            switch (fieldIndex)
            {
                case 0: Title = field;
                    break;
                case 1: Content = field;
                    break;
                case 2: Category = field;
                    break;
                case 3: Date = field;
                    break;
                case 4: Label = field;
                    break;
                default: break;
            }
            field.clear();
            fieldIndex++;
        }
        else
        {
            field += ch;
        }
        prevChar = ch;
    }

    if(fieldIndex == 4)
    {
        Label = field;
    }
}

// Import CSV file data into a linked list.
void import_csv_to_linkedlist(const string& mergefile, LinkedList<string>& list) 
{
    ifstream merge_file(mergefile);
    if (!merge_file.is_open()) 
    {
        throw runtime_error("Failed to open the file for import.");
    }

    string line;

    // Skip header (do not import header)
    getline(merge_file, line);

    while (getline(merge_file, line)) 
    {
        string Title, Content, Category, Date, Label;
        parse_csv(line, Title, Content, Category, Date, Label);

        // Import data into the linked list.
        list.pushback(Title, Content, Category, Date, Label); 
    }
    merge_file.close();
}

// Get the complete date data.
tm extract_date(string Date)
{
    tm tm = {};
    istringstream ss;

    // Check for comma to determine format
    if (Date.find(',') != string::npos) 
    {
        // Format: "January 15, 2016"
        ss.str(Date);
        ss >> get_time(&tm, "%B %d, %Y");
    } 
    else 
    {
        // Ensure day is two digits for formats like "8-Dec-17"
        if (Date[1] == '-') // If the second character is '-', it's a single-digit day
        {
            Date.insert(0, "0"); // Prepend '0' to make it two digits
        }

        // Format: "08-Dec-17"
        ss.str(Date);
        ss >> get_time(&tm, "%d-%b-%y");
    }

    if (ss.fail()) 
    {
        throw runtime_error("Failed to parse date: " + Date);
    }

    return tm;
}

// Compare two dates.
bool compare_date(const string& date1, const string& date2)
{
    tm tm1 = extract_date(date1);
    tm tm2 = extract_date(date2);

    // First, compare the year.
    if (tm1.tm_year != tm2.tm_year)
    {
        return tm1.tm_year < tm2.tm_year;
    }

    // Second, compare the month.
    if (tm1.tm_mon != tm2.tm_mon)
    {
        return tm1.tm_mon < tm2.tm_mon;
    }

    // Lastly, compare the day.
    return tm1.tm_mday < tm2.tm_mday;
}

// Sorting algorithm: Use TimSort to sort the linked list by date (ascending).
class TimSort {
    public:
        bool is_less_than(const string& a, const string& b) {
            return compare_date(a, b);
        }
    
        void insertion_sort(Article<string>*& head) {
            if (!head || !head->next) return;
    
            Article<string>* sorted = nullptr;
            Article<string>* current = head;
    
            while (current) {
                Article<string>* next = current->next;
    
                if (!sorted || is_less_than(current->Date, sorted->Date)) {
                    current->next = sorted;
                    sorted = current;
                } else {
                    Article<string>* temp = sorted;
                    while (temp->next && is_less_than(temp->next->Date, current->Date)) {
                        temp = temp->next;
                    }
                    current->next = temp->next;
                    temp->next = current;
                }
                current = next;
            }
            head = sorted;
        }
    
        // Merge two sorted linked lists.
        Article<string>* merge(Article<string>* left, Article<string>* right) {
            Article<string> dummy;
            Article<string>* tail = &dummy;
    
            while (left && right) {
                if (is_less_than(left->Date, right->Date)) 
                {
                    tail->next = left;
                    left = left->next;
                } 
                else 
                {
                    tail->next = right;
                    right = right->next;
                }
                tail = tail->next;
            }
    
            if (left) tail->next = left;
            if (right) tail->next = right;
    
            return dummy.next;
        }
    
        void timSort(Article<string>*& head) 
        {
            if (!head || !head->next) return;
    
            const int RUN = 32;
            const int MAX_RUN = 1500;
            Article<string>* run[MAX_RUN];
            int runCount = 0;
    
            Article<string>* current = head;
    
            while (current) {
                Article<string>* runHead = current;
                Article<string>* runTail = current;
                int count = 1;
    
                while (runTail->next && count < RUN) 
                {
                    runTail = runTail->next;
                    count++;
                }
    
                Article<string>* nextRUN = runTail->next;
                runTail->next = nullptr;
    
                insertion_sort(runHead);
                run[runCount++] = runHead;
    
                current = nextRUN;
            }
    
            // Merge sorted runs
            while (runCount > 1) {
                int newRunCount = 0;
                for (int i = 0; i < runCount; i += 2) 
                {
                    if (i + 1 < runCount)
                        run[newRunCount++] = merge(run[i], run[i + 1]);
                    else
                        run[newRunCount++] = run[i];
                }
                runCount = newRunCount;
            }
            head = run[0];  // Update the linked list head
        }
    };

// Searching algorithm: Linear search
class LinearSearch 
{
    public:
        //--------------- 1. Count the total number of news articles (both fake and true)--------------------
        void countNews(LinkedList<string>& list) 
        {
            int trueCount = 0, fakeCount = 0;
            Article<string>* current = list.getHead();
            while (current != nullptr) 
            {
                // Convert Label to lowercase for comparison
                string label = current->Label;
                transform(label.begin(), label.end(), label.begin(), ::tolower);
                if (label == "true")
                    trueCount++;
                else if (label == "fake")
                    fakeCount++;
                current = current->next;
            }
            cout << string(15,'-') << "Total News" << string(15,'-') << endl;
            cout << "Total true news: " << trueCount << endl;
            cout << "Total fake news: " << fakeCount << endl;
        }
    
        //.----------- 2. Calculate the percentage of fake news in political news for 2016.-------------------
        double percentageFakePolitical2016(LinkedList<string>& list) 
        {
            // Trim leading and trailing whitespace.
            auto trim = [](const string &s) -> string 
            {
                size_t start = s.find_first_not_of(" \t\n\r");
                if (start == string::npos) return "";
                size_t end = s.find_last_not_of(" \t\n\r");
                return s.substr(start, end - start + 1);
            };
        
            // Remove all spaces from the string.
            auto removeSpaces = [](const string &s) -> string 
            {
                string res;
                for (char c : s) 
                {
                    if (!isspace(c)) 
                    {
                        res.push_back(c);
                    }
                }
                return res;
            };
        
            int totalPolitical2016 = 0;
            int fakePolitical2016 = 0;
            Article<string>* current = list.getHead();
            while (current != nullptr) 
            {
                // Process date: assume format "DD-MM-YY", check if year is "16"
                string dateStr = trim(current->Date);
                if (dateStr.size() >= 8 && dateStr.substr(6, 2) == "16") 
                {
                    // Normalize category: remove spaces and convert to lowercase
                    string categoryNormalized = toLowercase(removeSpaces(current->Category));
                    // Consider both "politics" and "politicsNews" as political news.
                    if (categoryNormalized == "politics" || categoryNormalized == "politicsnews") 
                    {
                        totalPolitical2016++;
                        // Normalize label: remove spaces and convert to lowercase.
                        string labelNormalized = toLowercase(removeSpaces(current->Label));
                        if (labelNormalized == "fake")
                            fakePolitical2016++;
                    }
                }
                current = current->next;
            }
            if (totalPolitical2016 == 0)
                return 0.0;
            return (static_cast<double>(fakePolitical2016) / totalPolitical2016) * 100;
        }        
        
        //.----- 3. In fake government news, count the most frequent words in the content and output the top 10..------
        void top10FrequentWordsInGovernmentFakeNews(LinkedList<string>& list) 
        {
            WordFrequencyList wordFreqList;
            Article<string>* current = list.getHead();
            while (current != nullptr) 
            {
                // For government news: check if Category contains "government" (case-insensitive) and Label is "fake"
                string categoryLower = toLowercase(current->Category);
                string labelLower = toLowercase(current->Label);
                if (categoryLower.find("government") != string::npos && labelLower == "fake") 
                {
                    // Extract words from content and update word frequency statistics.
                    extractWords(current->Content, wordFreqList);
                }
                current = current->next;
            }
            // Sort by frequency from highest to lowest.
            wordFreqList.sortByFrequency();
            // Output the top 10 words.
            cout << string(15,'-') << "Top 10 frequent words in fake government news." << string(15,'-') << endl;
            WordFrequency* w = wordFreqList.getHead();
            int count = 0;
            while (w != nullptr && count < 10) 
            {
                cout << string(15, '-') << endl;
                cout << "Top " << (count + 1) << ": " << w->word << " : " << w->frequency << " times." << endl;
                w = w->next;
                count++;
            }
        }

        //--------- 4. Calculate the percentage of fake political news articles for each month in 2016..------------
        void percentageFakePoliticalByMonth(LinkedList<string>& list) 
        {
            map<int, int> totalPoliticalPerMonth;
            map<int, int> fakePoliticalPerMonth;
        
            Article<string>* current = list.getHead();
        
            while (current != nullptr) 
            {
                // Extract year and month from the date
                tm date = extract_date(current->Date);  // Ensure extract_date() parses the date correctly
                int year = date.tm_year + 2000; // Adjust tm_year to represent the actual year
                int month = date.tm_mon + 1;    // tm_mon starts at 0, so add 1
        
                // Only consider political news from 2016
                if (year == 2016 && (current->Category == "politics" || current->Category == "politicsNews")) 
                {
                    totalPoliticalPerMonth[month]++;
        
                    // Count fake news, ignoring case and spaces
                    string label = current->Label;
                    for (char& c : label) c = tolower(c);
                    label.erase(0, label.find_first_not_of(" \t")); // Remove leading spaces
                    label.erase(label.find_last_not_of(" \t") + 1);    // Remove trailing spaces
        
                    if (label == "fake") 
                    {
                        fakePoliticalPerMonth[month]++;
                    }
                }
                current = current->next;
            }
        
            cout << string(15,'-') << "Percentage of Fake Political News in 2016" << string(15,'-') << endl;
            for (int month = 1; month <= 12; month++) 
            {
                int total = totalPoliticalPerMonth[month];
                int fake = fakePoliticalPerMonth[month];
                double percentage = (total == 0) ? 0.0 : (static_cast<double>(fake) / total) * 100;
                cout << setw(3) << getMonthAbbreviation(month) << " | " << string(static_cast<int>(percentage), '*') << " " 
                     << fixed << setprecision(0) << percentage << "%" << endl;
            }
        }    
        
        //.------- 5. Search articles by keyword in the Content field and display the top 3 matching articles..------------
        void searchArticlesByKeyword(const LinkedList<string>& newsList) 
        {
            // Prompt user for search criteria.
            cout << "Enter keyword (or leave empty to skip): ";
            string keyword;
            cin.ignore();
            getline(cin, keyword);
            keyword = toLowercase(keyword);
        
            cout << "Enter category (or leave empty to skip): ";
            string category;
            getline(cin, category);
            category = toLowercase(category);
        
            cout << "Enter year (or leave empty to skip): ";
            string year;
            getline(cin, year);
        
            cout << "\nMatching articles (showing top 3):\n";
        
            // Traverse the linked list and output at most 3 matching articles.
            Article<string>* current = newsList.getHead();
            int count = 0;
            while (current != nullptr && count < 3) 
            {
                bool matchKeyword = keyword.empty() || toLowercase(current->Content).find(keyword) != string::npos;
                bool matchCategory = category.empty() || toLowercase(current->Category) == category;
                bool matchYear = year.empty() || current->Date.find(year) != string::npos;
        
                if (matchKeyword && matchCategory && matchYear) 
                {
                    cout << "Title: " << current->Title << "\n"
                         << "Category: " << current->Category << "\n"
                         << "Date: " << current->Date << "\n"
                         << "Label: " << current->Label << "\n\n";
                    count++;
                }
                current = current->next;
            }
            if (count == 0) 
            {
                cout << "No matching articles found." << endl;
            }
        }        
};
