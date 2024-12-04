#!/bin/bash

cd ../modified_radix_tree/build
# echo -e "\n\n\n\t\t\tTesting preprocessed-ipv4.rib.20170101.0000.txt\n\n\n"
# ./radix_test_basic ../../tests/tests_dump/ipv4/preprocessed-ipv4.rib.20170101.0000.txt ../../output/radixtree_preprocessed-ipv4.rib.20170101.0000.out.txt

# ./radix_test_basic ../../tests/tests_dump/ipv4/preprocessed-ipv4.rib.20170109.1600.txt ../../output/radixtree-preprocessed-ipv4.rib.20170109.1600.out.txt

# ./radix_test_basic ../../tests/tests_dump/ipv4/preprocessed-ipv4.rib.20170112.0800.txt ../../output/poptrie-preprocessed-ipv4.rib.20170112.0800.out.txt
# ./radix_test_basic ../../tests/tests_dump/ipv4/preprocessed-ipv4.rib.20170101.0000.txt ../output/radix-tree-preprocessed-ipv4.rib.20170101.0000.txt

./radix_test_basic ../../tests/tests_dump/ipv4/linx-rib.20141217.0000-p52.txt ../../output/radix-linx-rib.20141217.0000-p52.out.txt

# echo -e "\n\n\n\t\t\tTesting preprocessed-ipv4.rib.20170109.1600.txt\n\n\n"
# ./radix_test_basic ../../tests/tests_dump/ipv4/preprocessed-ipv4.rib.20170109.1600.txt
# echo -e "\n\n\n\t\t\tTesting preprocessed-ipv4.rib.20170112.0800.txt\n\n\n"
# ./radix_test_basic ../../tests/tests_dump/ipv4/preprocessed-ipv4.rib.20170112.0800.txt
# echo -e "\n\n\n\t\t\tTesting ipv4.rib20141217.0000.txt\n\n\n"
# ./radix_test_basic ../../tests/tests_dump/original_tests/norm_ipv4.txt
