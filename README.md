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

Currently, the developers of radix-tree and poptrie repos hard-coded an absolute path, but I will make it so the file is given as a command line argument to ensure ease of benchmarking.

Make sure you install all of the tools necessary to use automake tools

```
sudo apt update
sudo apt install autoconf automake libtool m4
```

#### The `make_all.sh` script

```
./make_all.sh
```

Look inside the actual scripts to see how each individual repository is built.




