#!/bin/bash

cd ../modified_sail/build
echo -e "\n\n\n\t\t\tTesting preprocessed-ipv4.rib.20170101.0000.txt\n\n\n"
./SAIL ../updates1.txt ../../tests/tests_dump/ipv4/SAIL_preprocessed-ipv4.rib.20170101.0000.txt
echo -e "\n\n\n\t\t\tTesting preprocessed-ipv4.rib.20170109.1600.txt\n\n\n"
./SAIL ../updates1.txt ../../tests/tests_dump/ipv4/SAIL_preprocessed-ipv4.rib.20170109.1600.txt
echo -e "\n\n\n\t\t\tTesting preprocessed-ipv4.rib.20170112.0800.txt"
./SAIL ../updates1.txt ../../tests/tests_dump/ipv4/SAIL_preprocessed-ipv4.rib.20170112.0800.txt
echo -e "\n\n\n\t\t\tTesting ipv4.rib20141217.0000.txt\n\n\n"
./SAIL ../updates1.txt ../../tests/tests_dump/original_tests/sail_ipv4_DIFF.txt
