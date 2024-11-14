#include <iostream>
#include <openssl/sha.h>
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

// Modified brute force to check against an array of target digests
void bruteforce(unsigned char targetDigests[][SHA256_DIGEST_LENGTH], int numTargets) {
    for (const string& perm : results) {
        unsigned char digest[SHA256_DIGEST_LENGTH];
        SHA256((const unsigned char*)perm.data(), perm.length(), digest);

        for (int i = 0; i < numTargets; ++i) {
            if (memcmp(targetDigests[i], digest, SHA256_DIGEST_LENGTH) == 0) {
                std::cout << "hash matched password " << perm  << " with ";
                for (int j = 0; j < SHA256_DIGEST_LENGTH; ++j) {
                    printf("%02x", targetDigests[i][j]);
                }
                std::cout << std::endl;
            }
        }
    }
    return;  // No matches found
}


int main() {
    const int numTargets = 10;  // Number of target passwords
    const char* targetPasswords[] = {"YVPS", "NURT", "PWJX", "LSAG", "XLRL", "HGDU", "VDCT", "BHKU", "CFJM", "VUBM"};
    unsigned char targetDigests[numTargets][SHA256_DIGEST_LENGTH];  // Array to store SHA256 digests for each target password

    // Compute SHA256 hash for each target password
    for (int i = 0; i < numTargets; ++i) {
        SHA256((const unsigned char*)targetPasswords[i], strlen(targetPasswords[i]), targetDigests[i]);
    }

    // Output SHA256 digests for verification (in hexadecimal format)
    for (int i = 0; i < numTargets; ++i) {
        std::cout << "SHA256 digest for \"" << targetPasswords[i] << "\": ";
        for (int j = 0; j < SHA256_DIGEST_LENGTH; ++j) {
            printf("%02x", targetDigests[i][j]);
        }
        std::cout << std::endl;
    }

    // Initialize chars vector with 'A'-'Z'
    vector<char> chars;
    for (char c = 'A'; c <= 'Z'; ++c) {
        chars.push_back(c);
    }

    // Set the length for permutations
    int length = 4;
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
