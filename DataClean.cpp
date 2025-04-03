#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>

using namespace std;

void trim(string& str) 
{
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    str = (start == string::npos) ? "" : str.substr(start, end - start + 1);
}

//Function to split csv file into a array.
bool splitCSVLine(const string &line, string columns[], int &colCount, char delimiter = ',') 
{
    stringstream ss(line);
    string value;
    colCount = 0;

    while (getline(ss, value, delimiter)) {
        trim(value); // Remove spaces from the value
        columns[colCount++] = value;

        // Prevent exceeding the fixed-size array
        if (colCount >= 4) break;
    }
    return (colCount == 4); // Ensure exactly 4 columns are read
}

//Function to remove empty rows from fake CSV file
void remove_empty_row(const string& fakefile, const string& fakefiles)
{
    ifstream inputfile(fakefile);
    ofstream outputfile(fakefiles);

    if(!inputfile.is_open() || !outputfile.is_open())
    {
        cout << "Failed to open the file to remove empty row." << endl;
        return;
    }

    string line;
    bool fake_header = true;

    while (getline(inputfile, line)) 
    {
        string columns[4];
        int colCount = 0;

        if(!splitCSVLine(line, columns, colCount)) continue;

        if(fake_header)
        {
            outputfile << line << "\n" ;
            fake_header = false;
            continue;
        }

        bool empty_column = false;
        for (int i = 0; i < colCount; i++) 
        {
            if (columns[i].empty()) 
            {
                empty_column = true;
                break;
            }
        }

        if (!empty_column) 
        {
            outputfile << line << "\n";
        }
    }
    inputfile.close();
    outputfile.close();
}

void merge_csv(const string& truefile, const string& fakefiles, const string& mergefile)
{
    ifstream true_file(truefile);
    ifstream fake_file(fakefiles);
    ofstream merge_file(mergefile);
    string line;

    if(!true_file.is_open() || !fake_file.is_open() || !merge_file.is_open())
    {
        cout << "Failed to open the files." << endl;
        return;
    }
    else
    {
        merge_file << "Title,Content,Category,Date,Label\n";

        bool true_header = true;
        while (getline(true_file, line))
        {
            if (true_header) 
            { 
                true_header = false;
                continue;
            }
            merge_file << line << ",True\n";
        }
        true_file.close();
        
        bool fake_header = true;
        while (getline(fake_file, line))
        {
            if (fake_header) 
            { 
                fake_header = false;
                continue;
            }
            merge_file << line << ",Fake\n";
        }
        fake_file.close();

        merge_file.close();

        cout << "Mergefile is successfully created." << endl;
    }
}

int main()
{
    /*
    string inputfile = "fake.csv";
    string outputfile = "fakes.csv";
    remove_empty_row(inputfile, outputfile);
    */

    string truefile = "true.csv";
    string fakefile = "fakes.csv";
    string mergefile = "merge.csv";
    merge_csv(truefile, fakefile, mergefile);

    
    return 0;
}
