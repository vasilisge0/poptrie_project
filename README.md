# poptrie_project
## Purdue 53600 Networks Final Project

### Subject Algorithm
`C`[Poptrie Implementation](https://github.com/pixos/poptrie/tree/master)
### Competitor Algorithms
1. `C`[Radix-tree](https://github.com/drpnd/radix-tree)
2. `Rust`[Tree BitMap](https://github.com/JakubOnderka/treebitmap/tree/master) 
3. `C`[SAIL, Tree BitMap, Lulea, LC-Trie](https://github.com/mengxiang0811/SAIL/tree/master)
4. DXR (D16R and D18R)


### Build Instructions

Make sure you are in the root directory of this repository and run:
```
docker build -t networks_final .
```
This will build the docker image. Then run the command below:
```
docker run --rm -it networks_final
```
This will automatically run the `make_all.sh` script and drop you into the terminal where you can run tests.




