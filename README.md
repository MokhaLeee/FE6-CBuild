# FE6-CBuild

![Working in Process](https://img.shields.io/badge/working-in_process-blue)

Project aims to build hacks based on [FireEmblem6](https://github.com/StanHash/fe6) (insert everything through EA)

## Progress

- [x] **debug kit**
- [x] **utf8 decoding**
    - Support en & zh & jp characters

## Custom build

1. Install dependence

```bash
sudo apt-get -y install binutils-arm-none-eabi \
    gcc-arm-none-eabi build-essential cmake re2c ghc \
    cabal-install libghc-vector-dev libghc-juicypixels-dev \
    python3-pip pkg-config libpng*

pip install pyelftools PyInstaller tmx six
```

2. Install sub-modules

```bash
cd tools
git clone https://github.com/MokhaLeee/FE6-CLib.git
git clone https://github.com/StanHash/EventAssembler.git --recursive
git clone https://github.com/StanHash/FE-PyTools.git --recursive
```

3. Install DevkitPRO

```bash
wget https://apt.devkitpro.org/install-devkitpro-pacman
chmod +x ./install-devkitpro-pacman
./install-devkitpro-pacman
sudo dkp-pacman -S gba-dev

# Export vars
export DEVKITPRO=/opt/devkitpro
export DEVKITARM=${DEVKITPRO}/devkitARM
export DEVKITPPC=${DEVKITPRO}/devkitPPC
export PATH=${DEVKITPRO}/tools/bin:$PATH
```

3. Build EA

    Get into **Tools/EventAssembler** and then refer to [EA build note](https://github.com/StanHash/EventAssembler).

4. Put [**Fire Emblem: Fūin no Tsurugi**](https://datomatic.no-intro.org/index.php?page=show_record&s=23&n=0367) clean rom named **fe6.gba** in the repository directory.

4. build:

```bash
make -j
```
