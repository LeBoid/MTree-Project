#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>
using namespace std;

// Exception Classes
class DuplicateInsertion : public exception {
public:
    const char* what() const noexcept override {
        return "Duplicate value insertion is not allowed.";
    }
};

class NotFoundException : public exception {
public:
    const char* what() const noexcept override {
        return "Value not found in the tree.";
    }
};

// MTree Class
template <typename DT>
class MTree {
private:
    int M;                        // Maximum children per node
    vector<DT> values;            // Values in the node (sorted)
    vector<MTree*> children;      // Child pointers

    // Utility: Check if the node is a leaf
    bool is_leaf() const {
        return children.empty();
    }

public:
    // Constructor
    MTree(int m) : M(m) {}

    // Destructor
    ~MTree() {
        for (auto child : children) {
            delete child;
        }
    }

    // Method: Build the tree from a sorted array
    void buildTree(const vector<DT>& input_values) {
        if (input_values.size() <= M - 1) {
            // If the array size is small, store all values in this leaf node
            values = input_values;
        } else {
            // Divide the array into M parts and create child nodes
            int D = input_values.size() / M;
            for (int i = 0; i < M; i++) {
                int start = D * i;
                int end = (i == M - 1) ? input_values.size() - 1 : start + D - 1;
                if (i < M - 1) {
                    values.push_back(input_values[end]); // Store split values
                }
                vector<DT> child_values(input_values.begin() + start, input_values.begin() + end + 1);
                MTree* child = new MTree(M);
                child->buildTree(child_values);
                children.push_back(child);
            }
        }
    }

    // Method: Insert a value into the tree
    void insert(const DT& value) {
    if (std::find(values.begin(), values.end(), value) != values.end()) {
        throw DuplicateInsertion();
    }
    if (is_leaf()) {
        values.push_back(value);
        sort(values.begin(), values.end());
    } else {
        for (int i = 0; i < values.size(); i++) {
            if (value < values[i]) {
                children[i]->insert(value);
                return;
            }
        }
        children.back()->insert(value);
    }
}

    // Method: Find a value in the tree
    bool find(const DT& value) const {
        // Check the current node's values
    if (std::find(values.begin(), values.end(), value) != values.end()) {
        return true;
    }
    // Traverse children if not a leaf
    if (!is_leaf()) {
        for (int i = 0; i < values.size(); i++) {
            if (value < values[i]) {
                return children[i]->find(value);
            }
        }
        return children.back()->find(value);
    }
    return false;
}

    // Method: Remove a value from the tree
    void remove(const DT& value) {
        if (is_leaf()) {
            // If leaf, remove the value
            auto it = std::find(values.begin(), values.end(), value);
            if (it != values.end()) {
                values.erase(it);
            } else {
                throw NotFoundException();
            }
        } else {
            // Traverse to the appropriate child
            for (int i = 0; i < values.size(); i++) {
                if (value < values[i]) {
                    children[i]->remove(value);
                    return;
                }
            }
            children.back()->remove(value);
        }
    }

    // Method: Rebuild the tree from all leaf values
    void rebuildTree() {
    vector<DT> all_values = collect_values();  // Collect all leaf values
    std::sort(all_values.begin(), all_values.end());
    all_values.erase(std::unique(all_values.begin(), all_values.end()), all_values.end()); // Remove duplicates
    values.clear();                          // Clear current node values
    for (auto child : children) {
        delete child;                          // Delete child nodes
    }
    children.clear();                          // Clear child pointers
    buildTree(all_values);                     // Rebuild tree
}

    // Method: Collect all values from the leaf nodes
    vector<DT> collect_values() const {
        if (is_leaf()) return values; // If leaf, return values
        vector<DT> result;
        for (const auto& child : children) {
            vector<DT> child_values = child->collect_values();
            result.insert(result.end(), child_values.begin(), child_values.end());
        }
        return result;
    }
};


// Main program
int main() {
    int n; // Number of elements in the initial sorted array
    int MValue, numCommands, value;
    char command;

    // Read the initial sorted array
    cin >> n;
    vector<int> mySortedValues(n);
    for (int i = 0; i < n; i++) {
        cin >> mySortedValues[i];
    }

    // Read the value of M and create the tree
    cin >> MValue;
    MTree<int>* myTree = new MTree<int>(MValue);
    myTree->buildTree(mySortedValues);

    // Read and process commands
    cin >> numCommands;
    for (int i = 0; i < numCommands; i++) {
        cin >> command;
        switch (command) {
        case 'I': { // Insert
            cin >> value;
            try {
                myTree->insert(value);
                cout << "The value = " << value << " was inserted." << endl;
            } catch (DuplicateInsertion&) {
                cout << "The value = " << value << " already in the tree." << endl;
            }
            break;
        }
        case 'R': { // Remove
            cin >> value;
            try {
                myTree->remove(value);
                cout << "The value = " << value << " has been removed." << endl;
            } catch (NotFoundException&) {
                cout << "The value = " << value << " not found." << endl;
            }
            break;
        }
        case 'F': { // Find
            cin >> value;
            if (myTree->find(value)) {
                cout << "The element with value = " << value << " was found." << endl;
            } else {
                cout << "The element with value = " << value << " not found." << endl;
            }
            break;
        }
        case 'B': { // Rebuild
            myTree->rebuildTree();
            cout << "The tree has been rebuilt." << endl;
            break;
        }
        default:
            cout << "Invalid command!" << endl;
        }
    }

    // Output the final list of values
    vector<int> finalValues = myTree->collect_values();
    cout << "Final list: ";
    for (const auto& val : finalValues) {
        cout << val << " ";
    }
    cout << endl;

    delete myTree; // Clean up memory
    return 0;
}


/* I used LLM tools to assist in writing and refining our classes and methods. Here's the documentation:

Prompts and Suggestions:

"Generate a method for inserting values into an MTree class."

"How can I implement a search method for the MTree class?"

"Provide a split_node method for MTree class that maintains tree balance."

Rationale:

These prompts were chosen to help structure the class methods and ensure they work cohesively. For example, generating an insertion method helped define how values are added and balanced within the tree.

Incremental Development:

We started by generating the basic structure of the MTree class.

Next, we incrementally refined each method, ensuring they integrate smoothly with the main program.

We used prompts to generate specific methods, such as insert, remove, search, and split_node, and then refined them for functionality.
*/
