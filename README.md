# C++ Hash Table

A simple C++ hash table with open addressing

## Usage
```cpp
// Create hash table with initial size
HashTable ht(10);

// Insert key-value pairs
ht.insert("cat", "meow");
ht.insert("dog", "woof");

// Retrieve values
if (auto value = ht.get("cat")) {
   std::cout << *value;  // Prints "meow"
}

// Remove entries
ht.remove("cat");

// Print table contents
ht.print();
