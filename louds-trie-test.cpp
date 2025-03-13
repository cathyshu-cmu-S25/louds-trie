#include "louds-trie.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <string.h>
#include <algorithm>
#include <set>

using namespace std;
using namespace louds;

// Helper function to print the contents of a trie
void print_trie(const Trie& trie) {
    cout << "Trie stats: " << trie.n_keys() << " keys, " 
         << trie.n_nodes() << " nodes, " 
         << trie.size() << " bytes" << endl;
    
    // We could add a function to print the keys, but for these tests
    // we'll rely on lookup to verify correctness
}

// Test basic functionality
void test_basic() {
    cout << "\n=== Testing Basic Functionality ===" << endl;
    
    Trie trie;
    vector<string> keys = {"apple", "banana", "cherry", "date", "elderberry", "fig"};
    
    // Add keys in order
    for (const auto& key : keys) {
        trie.add(key);
    }
    
    // Build the trie
    trie.build();
    
    print_trie(trie);
    
    // Test lookups
    for (size_t i = 0; i < keys.size(); ++i) {
        int64_t id = trie.lookup(keys[i]);
        cout << "Lookup '" << keys[i] << "': " << id << endl;
        assert(id >= 0 && "Expected key not found!");
    }
    
    // Test a key that shouldn't be there
    assert(trie.lookup("grape") == -1 && "Found key that shouldn't exist!");
    cout << "Lookup 'grape': " << trie.lookup("grape") << " (expected -1)" << endl;
    
    cout << "Basic functionality test passed!" << endl;
}

// Test merging functionality
void test_merge() {
    cout << "\n=== Testing Merge Functionality ===" << endl;
    
    // Create first trie with some keys
    Trie trie1;
    vector<string> keys1 = {"apple", "cherry", "fig", "grape", "lemon"};
    for (const auto& key : keys1) {
        trie1.add(key);
    }
    trie1.build();
    
    // Create second trie with some keys (some overlapping)
    Trie trie2;
    vector<string> keys2 = {"banana", "cherry", "date", "fig", "kiwi"};
    for (const auto& key : keys2) {
        trie2.add(key);
    }
    trie2.build();
    
    cout << "Trie 1:" << endl;
    print_trie(trie1);
    
    cout << "Trie 2:" << endl;
    print_trie(trie2);
    
    // Merge using the static method
    Trie* merged_trie = Trie::merge_trie(trie1, trie2);
    
    cout << "Merged Trie:" << endl;
    print_trie(*merged_trie);
    
    // Expected merged keys (unique, sorted)
    vector<string> expected_keys = {"apple", "banana", "cherry", "date", "fig", "grape", "kiwi", "lemon"};
    
    // Verify all expected keys are in the merged trie
    for (const auto& key : expected_keys) {
        int64_t id = merged_trie->lookup(key);
        cout << "Lookup '" << key << "': " << id << endl;
        assert(id >= 0 && "Expected key not found in merged trie!");
    }
    
    // Check that the number of keys matches expected
    assert(merged_trie->n_keys() == expected_keys.size() && 
           "Merged trie has incorrect number of keys!");
    
    // Clean up
    delete merged_trie;
    
    // Now test the instance merge method
    Trie trie3;
    vector<string> keys3 = {"apple", "orange", "pear", "quince"};
    for (const auto& key : keys3) {
        trie3.add(key);
    }
    trie3.build();
    
    cout << "Trie 3 (before merge):" << endl;
    print_trie(trie3);
    
    // Merge trie2 into trie3
    trie3.merge(trie2);
    
    cout << "Trie 3 (after merge with Trie 2):" << endl;
    print_trie(trie3);
    
    // Expected merged keys for trie3 (unique, sorted)
    vector<string> expected_keys3 = {"apple", "banana", "cherry", "date", "fig", "kiwi", "orange", "pear", "quince"};
    
    // Verify all expected keys are in trie3
    for (const auto& key : expected_keys3) {
        int64_t id = trie3.lookup(key);
        cout << "Lookup '" << key << "': " << id << endl;
        assert(id >= 0 && "Expected key not found in trie3 after merge!");
    }
    
    // Check that the number of keys matches expected
    assert(trie3.n_keys() == expected_keys3.size() && 
           "Trie3 has incorrect number of keys after merge!");
    
    cout << "Merge functionality test passed!" << endl;
}

// Test empty trie edge cases
void test_empty_tries() {
    cout << "\n=== Testing Empty Trie Edge Cases ===" << endl;
    
    // Empty trie merging with non-empty trie
    Trie empty_trie;
    empty_trie.build();
    
    Trie non_empty;
    vector<string> keys = {"apple", "banana", "cherry"};
    for (const auto& key : keys) {
        non_empty.add(key);
    }
    non_empty.build();
    
    // Merge empty into non-empty
    Trie* merged1 = Trie::merge_trie(empty_trie, non_empty);
    cout << "Empty + Non-empty:" << endl;
    print_trie(*merged1);
    
    // Verify all keys from non-empty are in merged trie
    for (const auto& key : keys) {
        assert(merged1->lookup(key) >= 0 && "Key from non-empty trie not found after merge!");
    }
    
    // Merge non-empty into empty
    Trie* merged2 = Trie::merge_trie(non_empty, empty_trie);
    cout << "Non-empty + Empty:" << endl;
    print_trie(*merged2);
    
    // Verify all keys from non-empty are in merged trie
    for (const auto& key : keys) {
        assert(merged2->lookup(key) >= 0 && "Key from non-empty trie not found after merge!");
    }
    
    // Merge two empty tries
    Trie empty_trie2;
    empty_trie2.build();
    
    Trie* merged3 = Trie::merge_trie(empty_trie, empty_trie2);
    cout << "Empty + Empty:" << endl;
    print_trie(*merged3);
    
    // Clean up
    delete merged1;
    delete merged2;
    delete merged3;
    
    cout << "Empty trie edge cases test passed!" << endl;
}

// Test large trie merging for performance
void test_large_tries() {
    cout << "\n=== Testing Large Trie Merging ===" << endl;
    
    // Generate a lot of random strings for trie1
    vector<string> keys1;
    for (int i = 0; i < 1000; ++i) {
        string key = "key_a_" + to_string(i);
        keys1.push_back(key);
    }
    sort(keys1.begin(), keys1.end());
    
    // Generate a lot of random strings for trie2
    vector<string> keys2;
    for (int i = 0; i < 1000; ++i) {
        string key = "key_b_" + to_string(i);
        keys2.push_back(key);
    }
    sort(keys2.begin(), keys2.end());
    
    // Add some overlapping keys
    for (int i = 0; i < 100; ++i) {
        string key = "key_common_" + to_string(i);
        keys1.push_back(key);
        keys2.push_back(key);
    }
    sort(keys1.begin(), keys1.end());
    sort(keys2.begin(), keys2.end());
    
    // Create tries
    Trie trie1;
    for (const auto& key : keys1) {
        trie1.add(key);
    }
    trie1.build();
    
    Trie trie2;
    for (const auto& key : keys2) {
        trie2.add(key);
    }
    trie2.build();
    
    cout << "Trie 1: " << trie1.n_keys() << " keys" << endl;
    cout << "Trie 2: " << trie2.n_keys() << " keys" << endl;
    
    // Merge tries
    cout << "Merging large tries..." << endl;
    Trie* merged = Trie::merge_trie(trie1, trie2);
    
    cout << "Merged trie: " << merged->n_keys() << " keys" << endl;
    
    // Compute expected number of keys
    set<string> all_keys;
    for (const auto& key : keys1) all_keys.insert(key);
    for (const auto& key : keys2) all_keys.insert(key);
    
    // Verify merged trie has the right number of keys
    assert(merged->n_keys() == all_keys.size() && 
           "Merged trie has incorrect number of keys!");
    
    // Verify a few random keys
    int check_count = 0;
    for (const auto& key : all_keys) {
        if (check_count % 100 == 0) {
            assert(merged->lookup(key) >= 0 && "Expected key not found in merged trie!");
        }
        check_count++;
    }
    
    // Clean up
    delete merged;
    
    cout << "Large trie merging test passed!" << endl;
}

// Test prefix lookup behavior
void test_prefix_lookups() {
    cout << "\n=== Testing Prefix Lookup Behavior ===" << endl;
    
    Trie trie;
    vector<string> keys = {"a", "apple", "application", "banana", "bat", "batch"};
    
    for (const auto& key : keys) {
        trie.add(key);
    }
    trie.build();
    
    // Test that prefixes of keys return -1 if not exact matches
    assert(trie.lookup("app") == -1 && "Prefix 'app' should not be found!");
    assert(trie.lookup("appl") == -1 && "Prefix 'appl' should not be found!");
    assert(trie.lookup("ba") == -1 && "Prefix 'ba' should not be found!");
    
    // But exact matches should work
    assert(trie.lookup("a") >= 0 && "Key 'a' should be found!");
    assert(trie.lookup("apple") >= 0 && "Key 'apple' should be found!");
    
    cout << "Prefix lookup test passed!" << endl;
}

int main() {
    // Run all tests
    test_basic();
    test_merge();
    test_empty_tries();
    test_prefix_lookups();
    test_large_tries();
    
    cout << "\nAll tests passed!" << endl;
    return 0;
}