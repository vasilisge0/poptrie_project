cd ../modified_poptrie/build/;
# valgrind --tool=massif ./poptrie_test_basic ../tests/linx-rib.20141217.0000-p46.txt
# valgrind --tool=massif --time-unit=ms --alloc-fn=buddy_init --pages-as-heap=no --detailed-freq=1 --max-snapshots=1000 ./poptrie_test_basic ../tests/linx-rib.20141217.0000-p46.txt
# valgrind --tool=massif --:xtree-leak=yes ./poptrie_test_basic ../tests/ipv4.rib20141217.0000.txt;
# perf stat -e cycles,instructions,cache-misses if=/dev/zero of=/dev/null count=100000 ./poptrie_test_basic linx-rib.20141217.0000-p46.txt;
# heaptrack ./poptrie_test_basic ../tests/linx-rib.20141217.0000-p46.txt

# heaptrack .libs/poptrie_test_basic ../tests/linx-rib.20141217.0000-p46.txt

./poptrie_test_basic ../tests/linx-rib.20141217.0000-p46.txt

# ./poptrie_test_basic ../tests/linx-rib.20141217.0000-p46.txt
 cd ../../tests;


      # exec heaptrack "$progdir/$program" ${1+"$@"}
    #   exec valgrind --tool=massif --time-unit=ms --pages-as-heap=no --detailed-freq=1 --max-snapshots=1000 "$progdir/$program" ${1+"$@"}

      # exec "$progdir/$program" ${1+"$@"}
      # exec valgrind --tool=massif --time-unit=ms --pages-as-heap=no --detailed-freq=1 --max-snapshots=1000  "$progdir/$program" ${1+"$@"}
      # exec perf mem record -a "$progdir/$program" ${1+"$@"} 0.0001
    #   exec perf stat "$progdir/$program" ${1+"$@"} 0.0001

      # exec "$progdir/$program" ${1+"$@"}
      # exec valgrind --tool=massif --time-unit=ms --pages-as-heap=no --detailed-freq=1 --max-snapshots=1000  "$progdir/$program" ${1+"$@"}
      # exec perf mem record -a "$progdir/$program" ${1+"$@"} 0.0001
      # exec perf stat "$progdir/$program" ${1+"$@"} 0.0001
      # exec heaptrack "$progdir/$program" ${1+"$@"} 0.0001
