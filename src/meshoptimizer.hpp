#pragma once

#include <cstddef>
#include <vector>

// Vertex transform cache optimizer using the "Linear-Speed Vertex Cache Optimisation" by Tom Forsyth
//   https://tomforsyth1000.github.io/papers/fast_vert_cache_opt.html
// Reorders indices to reduce the number of GPU vertex shader invocations
//
// destination must contain enough space for the resulting index buffer
// cache_size is a trade off for algorithm performance vs ACMR, and is limited to 32
void optimizePostTransformTomF(unsigned short* destination, const unsigned short* indices, size_t index_count, size_t vertex_count, unsigned int cache_size = 16);
void optimizePostTransformTomF(unsigned int* destination, const unsigned int* indices, size_t index_count, size_t vertex_count, unsigned int cache_size = 16);

// Vertex transform cache optimizer using the Tipsify algorithm by Sander et al
//   http://gfx.cs.princeton.edu/pubs/Sander_2007_%3ETR/tipsy.pdf
// Reorders indices to reduce the number of GPU vertex shader invocations
//
// destination must contain enough space for the resulting index buffer
// cache_size should be less than the actual GPU cache size to avoid cache thrashing
// clusters is an optional output for the overdraw optimizer (below)
void optimizePostTransformTipsify(unsigned short* destination, const unsigned short* indices, size_t index_count, size_t vertex_count, unsigned int cache_size = 16, std::vector<unsigned int>* clusters = 0);
void optimizePostTransformTipsify(unsigned int* destination, const unsigned int* indices, size_t index_count, size_t vertex_count, unsigned int cache_size = 16, std::vector<unsigned int>* clusters = 0);

// Overdraw optimizer using the Tipsify algorithm
// Reorders indices to reduce the number of GPU vertex shader invocations and the pixel overdraw
//
// destination must contain enough space for the resulting index buffer
// indices must contain index data that is the result of optimizePostTransformTipsify (*not* the original mesh indices!)
// clusters must contain cluster data that is the result of optimizePostTransformTipsify
// vertex_positions should have float3 position in the first 12 bytes of each vertex - similar to glVertexPointer
// cache_size should be less than the actual GPU cache size to avoid cache thrashing
// threshold indicates how much the overdraw optimizer can degrade vertex cache efficiency (1.05 = up to 5%) to reduce overdraw more efficiently
void optimizeOverdrawTipsify(unsigned short* destination, const unsigned short* indices, size_t index_count, const void* vertex_positions, size_t vertex_positions_stride, size_t vertex_count, const std::vector<unsigned int>& clusters, unsigned int cache_size = 16, float threshold = 1);
void optimizeOverdrawTipsify(unsigned int* destination, const unsigned int* indices, size_t index_count, const void* vertex_positions, size_t vertex_positions_stride, size_t vertex_count, const std::vector<unsigned int>& clusters, unsigned int cache_size = 16, float threshold = 1);

// Vertex fetch cache optimizer
// Reorders vertices and changes indices to reduce the amount of GPU memory fetches during vertex processing
//
// desination must contain enough space for the resulting vertex buffer
// indices is used both as an input and as an output index buffer
void optimizePreTransform(void* destination, const void* vertices, unsigned short* indices, size_t index_count, size_t vertex_count, size_t vertex_size);
void optimizePreTransform(void* destination, const void* vertices, unsigned int* indices, size_t index_count, size_t vertex_count, size_t vertex_size);

struct PostTransformCacheStatistics
{
	unsigned int hits, misses;
	float hit_percent, miss_percent;
	float acmr; // transformed vertices / triangle count
};

// Vertex transform cache analyzer
// Returns cache hit statistics using a simplified FIFO model
// Results will not match actual GPU performance
PostTransformCacheStatistics analyzePostTransform(const unsigned short* indices, size_t index_count, size_t vertex_count, unsigned int cache_size = 32);
PostTransformCacheStatistics analyzePostTransform(const unsigned int* indices, size_t index_count, size_t vertex_count, unsigned int cache_size = 32);
