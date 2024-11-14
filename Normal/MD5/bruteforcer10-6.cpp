#include <iostream>
#include <openssl/md5.h>
#include <cstring>
#include <array>
#include <string>
#include <vector>
#include <ctime>
using namespace std;

vector<string> results;  // Store permutations here

// Helper function for backtracking with fixed length
void backtrack(const vector<char>& chars, string& current, int maxLength) {
    // Base case: if the current permutation reaches maxLength, store it
    if (current.size() == maxLength) {
        results.push_back(current);
        return;
    }

    // Recur with each character in the character set
    for (char c : chars) {
        current.push_back(c);  // Add character to the current permutation
        backtrack(chars, current, maxLength);  // Recurse
        current.pop_back();  // Remove character to backtrack
    }
}
// Function to start generating permutations with a fixed length
void generatePermutations(const vector<char>& chars, int length) {
    string current;  // Holds the current permutation
    backtrack(chars, current, length);
}

// Brute force to check against an array of target digests
void bruteforce(unsigned char targetDigests[][MD5_DIGEST_LENGTH], int numTargets) {
    for (const string& perm : results) {
        unsigned char digest[MD5_DIGEST_LENGTH];
        MD5((const unsigned char*)perm.data(), perm.length(), digest);

        for (int i = 0; i < numTargets; ++i) {
            if (memcmp(targetDigests[i], digest, MD5_DIGEST_LENGTH) == 0) {
                std::cout << "hash matched password " << perm  << " with ";
                for (int j = 0; j < MD5_DIGEST_LENGTH; ++j) {
                    printf("%02x", targetDigests[i][j]);
                }
                std::cout << std::endl;
            }
        }
    }
    return;
}

int main() {
    const int numTargets = 10;  // Number of target passwords
    const char* targetPasswords[] = {"460175", "210544", "725069", "594479", "029898", "760867", "795107", "493437", "5104238", "3684201"};
    unsigned char targetDigests[numTargets][MD5_DIGEST_LENGTH];  // Array to store MD5 digests for each target password

    // Compute MD5 hash for each target password
    for (int i = 0; i < numTargets; ++i) {
        MD5((const unsigned char*)targetPasswords[i], strlen(targetPasswords[i]), targetDigests[i]);
    }

    // Output MD5 digests for verification (in hexadecimal format)
    for (int i = 0; i < numTargets; ++i) {
        std::cout << "MD5 digest for \"" << targetPasswords[i] << "\": ";
        for (int j = 0; j < MD5_DIGEST_LENGTH; ++j) {
            printf("%02x", targetDigests[i][j]);
        }
        std::cout << std::endl;
    }

    // Initialize chars vector with '1'-'9'
    vector<char> chars;
    for (char c = '1'; c <= '9'; ++c) {  // Add digits '1' through '9'
        chars.push_back(c);
    }
    chars.push_back('0'); // Adds 0

    // Set the length for permutations
    int length = 6;
    // Generate permutations of the specified length
    generatePermutations(chars, length);
    
    clock_t start_time = clock();
    // Apply brute force to check permutations against all target digests
    bruteforce(targetDigests, numTargets);
    clock_t end_time = clock();

    // Clear permutations
    results.clear();

    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("The program took %.6f seconds to execute.\n", elapsed_time);

    return 0;
}
