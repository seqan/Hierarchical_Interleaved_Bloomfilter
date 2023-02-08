// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2022, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2022, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <seqan3/alphabet/nucleotide/dna4.hpp>
#include <seqan3/search/dream_index/hierarchical_interleaved_bloom_filter.hpp>
#include <seqan3/test/cereal.hpp>
#include <seqan3/test/expect_range_eq.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <seqan3/core/detail/all_view.hpp>
#include <seqan3/utility/views/deep.hpp>
#include <seqan3/core/debug_stream.hpp>

#include <seqan3/test/expect_range_eq.hpp>

TEST(hibf_test, test_specific_hash_values)
{
    using namespace seqan3::literals;

    // range of range of sequences
    std::vector<std::vector<std::vector<size_t>>> hashes{{{1u,2u,3u,4u,5u,6u,7u,8u,9u,10u}}, {{1u,2u,3u,4u,5u}}};

    seqan3::hibf_config config
    {
        .input = hashes,
        .rearrange_user_bins = false
    };

    seqan3::hierarchical_interleaved_bloom_filter hibf{config};

    {
        std::vector<size_t> query{1,2,3,4,5};

        auto agent = hibf.membership_agent();
        auto result = agent.bulk_contains(query, 2);

        EXPECT_RANGE_EQ(result, (std::vector<size_t>{0u, 1u}));
    }
}

TEST(hibf_test, input_sequences_of_sequences)
{
    using namespace seqan3::literals;

    auto kmer_transformation = seqan3::views::kmer_hash(seqan3::ungapped{2u});

    // range of range of sequences
    std::vector<std::vector<std::vector<seqan3::dna4>>> seqs{{"AAAGGGGGGC"_dna4}, {"TTTTTT"_dna4}};

    seqan3::hibf_config config
    {
        .input = seqs | seqan3::views::deep{kmer_transformation},
        .rearrange_user_bins = false
    };

    seqan3::hierarchical_interleaved_bloom_filter hibf{config};

    auto agent = hibf.membership_agent();

    std::vector<seqan3::dna4> query{"AAGG"_dna4};
    auto query_kmers = query | kmer_transformation;

    auto result = agent.bulk_contains(query_kmers, 1);

    seqan3::debug_stream << result << std::endl;
}

TEST(hibf_test, input_files)
{
    seqan3::test::tmp_directory tmp{};
    std::filesystem::path f1{tmp.path() / "f1.fa"};
    std::filesystem::path f2{tmp.path() / "f2.fa"};

    {
        std::ofstream out{f1};
        out << ">seq1\nAAAGGGGGGC\n";

        std::ofstream out2{f2};
        out2 << ">seq1\nTTTTTT\n";
    }

    auto transform = seqan3::views::kmer_hash(seqan3::ungapped{2u});

    // range of range of sequences
    std::vector<std::string> filenames{f1.string(), f2.string()};
    auto file_range = filenames | std::views::transform([&transform](auto const & f)
    {
        auto record_transform = std::views::transform([&transform](auto && rec){ return rec.sequence() | transform; });
        return seqan3::detail::all(seqan3::sequence_file_input{f}) | record_transform;
    });

    seqan3::hibf_config config
    {
        .input = file_range,
        .rearrange_user_bins = false
    };

    seqan3::hierarchical_interleaved_bloom_filter hibf{config};

    auto agent = hibf.membership_agent();

    using namespace seqan3::literals;

    std::vector<seqan3::dna4> query{"AAGG"_dna4};

    auto result = agent.bulk_contains(query | transform, 1);

    seqan3::debug_stream << result << std::endl; // prints [0] since query is found in user bin 0
}