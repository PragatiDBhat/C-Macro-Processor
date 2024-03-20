#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

struct Macro
{
    string name;
    vector<string> arguments;
    string definition;
};

struct MacroName
{
    string name;
};

unordered_map<string, Macro> macroMap;
unordered_map<string, MacroName> macroNam;

void updateMacroMap(const string& name, const vector<string>& arguments, const string& definition)
{
    macroMap[name] = {name, arguments, definition};
    macroNam[name] = {name};
}

bool hasArguments(const string& line, const string& name)
{
    size_t pos = line.find(name);
    if (pos != string::npos)
    {
        pos += name.length();
        while (pos < line.length() && isspace(line[pos]))
        {
            pos++;
        }
        return pos < line.length() && line[pos] == '(';
    }
    return false;
}

bool isInsideDoubleQuotes(const string& line, size_t pos)
{
    int quotesCount = 0;
    for (size_t i = 0; pos < line.length() && i < pos; ++i)
    {
        if (line[i] == '\"')
        {
            quotesCount++;
        }
    }
    return (quotesCount % 2 == 1);
}

void replaceMacroWithArguments(string& line, const Macro& macro, const vector<string>& actualArguments)
{
    string macroCall = macro.name + "(";
    for (const auto& arg : actualArguments)
    {
        macroCall += arg + ",";
    }
    if (!actualArguments.empty())
    {
        macroCall.pop_back();
    }
    macroCall += ")";
    if (actualArguments.size() != macro.arguments.size())
    {
        line = "Error: Mismatched number of arguments for macro '" + macro.name + "' call: " + macroCall;
        return;
    }
    string replacedLine = macro.definition;
    for (size_t i = 0; i < macro.arguments.size(); ++i)
    {
        size_t pos = replacedLine.find(macro.arguments[i]);
        while (pos != string::npos)
        {
            replacedLine.replace(pos, macro.arguments[i].length(), actualArguments[i]);
            pos = replacedLine.find(macro.arguments[i], pos + actualArguments[i].length());
        }
    }
    size_t pos = line.find(macroCall);
    while (pos != string::npos)
    {
        if (!isInsideDoubleQuotes(line, pos))
        {
            line.replace(pos, macroCall.length(), replacedLine);
            pos = line.find(macroCall, pos + replacedLine.length());
        }
        else
        {
            pos = line.find(macroCall, pos + 1);
        }
    }
}

void processLine(string& line)
{
    for (const auto& pair : macroMap)
    {
        const Macro& macro = pair.second;
        if (hasArguments(line, macro.name))
        {
            size_t start = line.find('(') + 1;
            size_t end = line.find(')');
            string argList = line.substr(start, end - start);
            istringstream argStream(argList);
            vector<string> actualArguments;
            string actualArgument;
            while (getline(argStream, actualArgument, ','))
            {
                actualArguments.push_back(actualArgument);
            }
            replaceMacroWithArguments(line, macro, actualArguments);
        }
        else if (line.find(macro.name) != string::npos)
        {
            size_t pos = line.find(macro.name);
            while (pos != string::npos)
            {
                if (!isInsideDoubleQuotes(line, pos))
                {
                    if (macroMap.find(macro.name) != macroMap.end())
                    {
                        line.replace(pos, macro.name.length(), macro.definition);
                    }
                    else
                    {
                        // Handle macro call without definition
                        line = "Error: Macro '" + macro.name + "' used without definition.";
                    }
                    pos = line.find(macro.name, pos + macro.definition.length());
                }
                else
                {
                    pos = line.find(macro.name, pos + 1);
                }
            }
        }
    }
}

int main()
{
    ifstream inputFile("input.txt");
    ofstream outputFile("output.txt");
    ofstream namFile("NAMTAB.txt");
    ofstream defFile("DEFTAB.txt");
    ofstream argFile("ARGTAB.txt");

    if (!inputFile.is_open() || !outputFile.is_open() || !namFile.is_open() || !defFile.is_open() || !argFile.is_open())
    {
        cerr << "Error opening files" << endl;
        exit(1);
    }

    string line;
    bool isSkipping = false;

    while (getline(inputFile, line))
    {
        istringstream iss(line);
        string directive;
        iss >> directive;

        if (directive == "#define")
        {
            string name, definition;
            vector<string> arguments;

            iss >> name;

            size_t pos = name.find('(');
            if (pos != string::npos)
            {
                string argList = name.substr(pos + 1, name.find(')') - pos - 1);
                name = name.substr(0, pos);
                istringstream argStream(argList);
                string argument;
                while (getline(argStream, argument, ','))
                {
                    arguments.push_back(argument);
                }
            }

            getline(iss, definition);
            updateMacroMap(name, arguments, definition);
            namFile << name << endl;
            defFile << name << " " << definition << endl;
            argFile << name;
            for (const auto &arg : arguments)
            {
                argFile << " " << arg;
            }
            argFile << endl;
        }
        else if (directive == "#ifdef" || directive == "#ifndef")
        {
            string macroName;
            iss >> macroName;
            isSkipping = (directive == "#ifndef" && macroMap.find(macroName) == macroMap.end()) ||
                         (directive == "#ifdef" && macroMap.find(macroName) != macroMap.end());
            cout << isSkipping;
            if (isSkipping)
            {
                getline(inputFile, line);
                processLine(line);
                outputFile << line << endl;
                do
                {
                    istringstream iss(line);
                    iss >> directive;
                    if (directive == "#endif")
                    {
                        isSkipping = false;
                        getline(inputFile, line);
                        break;
                    }
                } while (getline(inputFile, line));

                processLine(line);
                outputFile << line << endl;
                continue;
            }
            if (!isSkipping)
            {
                while (getline(inputFile, line))
                {
                    istringstream iss(line);
                    iss >> directive;
                    if (directive == "#else")
                    {
                        isSkipping = false;
                        break;
                    }
                }
                continue;
            }
        }
        else if (directive == "#else" && isSkipping)
        {
            getline(inputFile, line);
            processLine(line);
            outputFile << line << endl;
            while (getline(inputFile, line))
            {
                istringstream iss(line);
                iss >> directive;
                if (directive == "#endif")
                {
                    isSkipping = false;
                    getline(inputFile, line);
                    break;
                }
            }
            continue;
        }
        else if (directive == "#else" && !isSkipping)
        {
            outputFile << line << endl;
            getline(inputFile, line);
            processLine(line);
            continue;
        }
        else if (directive == "#endif")
        {
            isSkipping = false;
            getline(inputFile, line);
            processLine(line);
        }
        else if (!isSkipping)
        {
            // Process the line only if not skipping due to conditional directives
            processLine(line);
        }
        // Print the processed line into the output file
        outputFile << line << endl;
    }

    inputFile.close();
    outputFile.close();
    namFile.close();
    defFile.close();
    argFile.close();

    ifstream namTabFile("NAMTAB.txt");
    ifstream defTabFile("DEFTAB.txt");
    ifstream argTabFile("ARGTAB.txt");

    if (!namTabFile.is_open() || !defTabFile.is_open() || !argTabFile.is_open())
    {
        cerr << "Error opening one or more files for reading" << endl;
        exit(1);
    }

    cout << "\nContents of NAMTAB.txt:" << endl;
    string namTabLine;
    cout << "Name:" << endl;
    while (getline(namTabFile, namTabLine))
    {
        cout << namTabLine << endl;
    }
    namTabFile.close();

    cout << "\nContents of DEFTAB.txt:" << endl;
    string defTabLine;
    cout << "Name:" << "\t" << "Definition:" << endl;
    while (getline(defTabFile, defTabLine))
    {
        istringstream defTabStream(defTabLine);
        string name, definition;
        defTabStream >> name >> definition;
        cout << name << "\t";
        cout << definition << endl;
    }
    defTabFile.close();

    cout << "\nContents of ARGTAB.txt:" << endl;
    string argTabLine;
    cout << "Name:" << "\t" << "Arguments:" << endl;
    while (getline(argTabFile, argTabLine))
    {
        istringstream argTabStream(argTabLine);
        string name;
        argTabStream >> name;
        cout << name << "\t";
        string argument;
        while (argTabStream >> argument)
        {
            cout << " " << argument;
        }
        cout << endl;
    }
    argTabFile.close();

    return 0;
}
