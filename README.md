# poptrie_project
## Purdue 53600 Networks Final Project

### Subject Algorithm
`C`[Poptrie Implementation](https://github.com/pixos/poptrie/tree/master)
### Competitor Algorithms
1. `C`[Radix-tree](https://github.com/drpnd/radix-tree) [[Build Instructions]](#radix-tree)
2. `Rust`[Tree BitMap](https://github.com/JakubOnderka/treebitmap/tree/master) 
3. `C`[SAIL, Tree BitMap, Lulea, LC-Trie](https://github.com/mengxiang0811/SAIL/tree/master)
4. DXR (D16R and D18R)


### Build Instructions

#### Radix-Tree

```
cd <radix_tree>
mv ../autogen_radix_tree.sh .
chmod +x autogen_radix_tree.sh
./autogen_radix_tree.sh
```

Currently, the developers of that repo hard-coded an absolute path, but I will make it so the file is given as a command line argument to ensure ease of benchmarking.



