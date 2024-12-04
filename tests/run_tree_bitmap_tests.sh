cd ../modified_tree_bitmap/;

# ./prefix_lookup ../tests/linx-rib.20141217.0000-p46.txt ../output/poptrie-linx-rib.20141217.0000-p46-0.out.txt
# ./prefix_lookup ../tests/tests_dump/ipv4/preprocessed-ipv4.rib.20170109.1600.txt ../output/poptrie-preprocessed-ipv4.rib.20170109.1600.out.txt
# ./prefix_lookup ../tests/tests_dump/ipv4/preprocessed-ipv4.rib.20170112.0800.txt ../output/poptrie-preprocessed-ipv4.rib.20170112.0800.out.txt
# ./prefix_lookup ../tests/tests_dump/ipv4/preprocessed-ipv4.rib.20170101.0000.txt ../output/poptrie-preprocessed-ipv4.rib.20170101.0000.txt
./prefix_lookup ../tests/tests_dump/ipv4/linx-rib.20141217.0000-p52.txt ../output/tree-bitmap-linx-rib.20141217.0000-p52.out.txt

cd ../tests;
