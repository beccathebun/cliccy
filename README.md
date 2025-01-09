## Cliccy

### Description

fun program for cuties :3

### Getting started
#### Linux

**Note:**
  - There may be some dependencies you need, just follow compiler errors ^^;

To bootstrap builder **(only need to do this once)**:
```bash
cc -o build build.c
#(alternatively)
make builder
```

To build app:
```bash
./build <debug?>
```
On first run the app will add `cliccy.toml` to `~/.config/cliccy/cliccy.toml` if it doesn't already exist, with default values.

#### Windows
***Probably only works with building in wsl currently.***

To bootstrap builder **(only need to do this once)**:
```bash
cc -o build build.c
#(alternatively)
make builder
```

To build app:
```bash
./build --win <debug?>
```
On first run the app will add `cliccy.toml` to `%LocalAppData%\cliccy\cliccy.toml` if it doesn't already exist, with default values.
### Features
opens links,sends notifications,line writing, random dialogs..

### Links

 - [clay](https://github.com/nicbarker/clay)
 - [raylib](https://github.com/raysan5/raylib)
 - [toml-c](https://github.com/arp242/toml-c)
 - [wintoastlibc](https://github.com/AlienCowEatCake/WinToastLibC)

### License

This project sources are licensed under an unmodified zlib/libpng license, which is an OSI-certified, BSD-like license that allows static linking with closed source software. Check [LICENSE](LICENSE) for further details.

