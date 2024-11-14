#include <cuda_runtime.h>
#include <iostream>
#include <cstring>
#include <array>
#include <string>
#include "config.h"
#include <vector>
#include <openssl/sha.h>
#include "sha256.cuh"
#include <time.h>
using namespace std;
#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest


vector<string> results;  // Store permutations here

// Helper function for backtracking with fixed length
void backtrack(const vector<char>& chars, string& current, int maxLength, vector<string>& results) {
    // Base case: if the current permutation reaches maxLength, print it
    if (current.size() == maxLength) {
        results.push_back(current);
        return;
    }

    // Recur with each character in the character set
    for (char c : chars) {
        current.push_back(c);  // Add character to the current permutation
        backtrack(chars, current, maxLength, results);  // Recurse
        current.pop_back();  // Remove character to backtrack
    }
}
// Function to start generating permutations with a fixed length
void generatePermutations(const vector<char>& chars, int length, vector<string>& results) {
    string current;  // Holds the current permutation
    backtrack(chars, current, length, results);
}

extern void mcm_cuda_sha256_hash_batch(BYTE* in, WORD inlen, BYTE* out, WORD n_batch);

void bruteforce(unsigned char targetDigests[][SHA256_DIGEST_LENGTH], int numTargets, char* guessPool, int guessPoolSize, int length){
      // Allocate device memory for output data (MD5 hashes)
    BYTE* cuda_outdata;
    cudaMallocManaged(&cuda_outdata, SHA256_BLOCK_SIZE * guessPoolSize * sizeof(BYTE));

    // Launch the batch MD5 hashing function
    WORD batch_size = guessPoolSize;  // Process all guesses in one batch
    mcm_cuda_sha256_hash_batch(reinterpret_cast<BYTE*>(guessPool), length, cuda_outdata, batch_size);

    // Check the MD5 hashes for a match with the target digest
    for (int i = 0; i < guessPoolSize; ++i) {
        BYTE* digest = cuda_outdata + i * SHA256_BLOCK_SIZE;  // Retrieve the ith digest from cuda_outdata

        // Check the current digest against each target digest
        for (int j = 0; j < numTargets; ++j) {
            if (memcmp(targetDigests[j], digest, SHA256_BLOCK_SIZE) == 0) {
                // Assuming `guessPool[i]` holds the permutation that produced this digest
                std::cout << "Hash matched for password \"";

                for (int l = 0; l < length; ++l) {
                    std::cout << guessPool[i * length + l];
                }

                std::cout << "\" with hash: ";
                for (int k = 0; k < SHA256_BLOCK_SIZE; ++k) {
                    printf("%02x", targetDigests[j][k]);
                }
                std::cout << std::endl;
            }
        }
    }

    // Clean up the allocated device memory
    cudaFree(cuda_outdata);
}



int main() {

    const int numTargets = 10;  // Number of target passwords
    const char* targetPasswords[] = {"YVPS", "NURT", "PWJX", "LSAG", "XLRL", "HGDU", "VDCT", "BHKU", "CFJM", "VUBM"};
    unsigned char targetDigests[numTargets][SHA256_DIGEST_LENGTH];  // Array to store MD5 digests for each target password

    for (int i = 0; i < numTargets; ++i) {
        SHA256((const unsigned char*)targetPasswords[i], strlen(targetPasswords[i]), targetDigests[i]);
    }

    // Output MD5 digests for verification (in hexadecimal format)
    for (int i = 0; i < numTargets; ++i) {
        std::cout << "MD5 digest for \"" << targetPasswords[i] << "\": ";
        for (int j = 0; j < SHA256_DIGEST_LENGTH; ++j) {
            printf("%02x", targetDigests[i][j]);
        }
        std::cout << std::endl;
    }

    
    // Initialize chars vector with 'A'-'Z'
    vector<char> chars;
    for (char c = 'A'; c <= 'Z'; ++c) {
        chars.push_back(c);  // Add uppercase letters
    }
    


    vector<string> results;  // Store permutations here
    // Generate and print permutations of the specified length

    clock_t start_time;
    clock_t end_time;
    double elapsed_time;

    int length = 4;
    char* guessPool;
    int guessPoolSize;

    // Generate permutations of the specified length
    generatePermutations(chars, length, results);

    // Clear permutations for the next iteration
    guessPoolSize = results.size();
    cudaMallocManaged(&guessPool, guessPoolSize * length * sizeof(char));

    for (int i = 0; i < guessPoolSize; i++) {
        memcpy(guessPool + i * length, results[i].c_str(), length * sizeof(char));
    }
    start_time = clock();
    // Apply brute force to check permutations against all target digests
    bruteforce(targetDigests, numTargets, guessPool, guessPoolSize, length);
    end_time = clock();
    elapsed_time = elapsed_time + (double)(end_time - start_time) / CLOCKS_PER_SEC;

    cudaFree(guessPool);
    
    results.clear();
    printf("The program took %.6f seconds to execute.\n", elapsed_time);
    return 0;
}