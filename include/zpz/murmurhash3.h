#ifndef _zpz_murmurhash_h_
#define _zpz_murmurhash_h_

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.
//
// The github repo is aappleby/smhasher
//
// I took the 32 bits functions and did minor reformatting.


#include <cstdint>  // uint8_t, uint32_t, uint64_t
#include <tuple>


namespace zpz
{

inline uint32_t rotl32 ( uint32_t x, int8_t r )
{
    return (x << r) | (x >> (32 - r));
}


// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here
//
// TODO:
//   Author says 'if your platform needs ...'.
//   Does my platform need this?
inline uint32_t getblock32 ( const uint32_t * p, int i )
{
    return p[i];
}


// Finalization mix - force all bits of a hash block to avalanche
inline uint32_t fmix32 ( uint32_t h )
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}


void MurmurHash3_x86_32 ( const void * key, int len,
                        uint32_t seed, void * out ) 
{
    const uint8_t * data = (const uint8_t*)key;
    const int nblocks = len / 4;
    uint32_t h1 = seed;
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);
    for(int i = -nblocks; i; i++)
    {
        uint32_t k1 = getblock32(blocks,i);
        k1 *= c1;
        k1 = rotl32(k1,15);
        k1 *= c2;
        h1 ^= k1;
        h1 = rotl32(h1,13); 
        h1 = h1*5+0xe6546b64;
    }

    const uint8_t * tail = (const uint8_t*)(data + nblocks*4);
    uint32_t k1 = 0;
    switch(len & 3)
    {
        case 3: k1 ^= tail[2] << 16;
        case 2: k1 ^= tail[1] << 8;
        case 1: k1 ^= tail[0];
        k1 *= c1; k1 = rotl32(k1,15); k1 *= c2; h1 ^= k1;
    };

    h1 ^= len;
    h1 = fmix32(h1);
    *(uint32_t*)out = h1;
}


int32_t murmurhash3_32(char const * key, int len, int seed = 0) {
    /*
    key : bytes or string encoded as bytes.

    seed : int, optional default is 0
        integer seed for the hashing algorithm.

    The results is casted to a signed int
        from -(2 ** 31) to 2 ** 31 - 1
    */
    int32_t out;
    MurmurHash3_x86_32(key, len, seed, &out);
    return out;
}



// This is used in 'feature hashing',
// see
//   sklearn.feature_extraction.hashing.FeatureHasher
//   sklearn.feature_extraction._hashing.transform
std::pair<int, int> hash(char const * name, int len, int n_out)
{
    // Returns index and sign.
    auto h = murmurhash3_32(name, len);
    auto idx = std::abs(h) % n_out;
    if (h < 0) {
        // Flip sign to improve inner product preservation in the hashed space
        return std::make_pair(idx, -1);
    }
    return std::make_pair(idx, 1);
}


}  // namespace zpz
#endif  // _zpz_murmurhash_h_
