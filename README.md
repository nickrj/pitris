# Pitris
Pitris is a Tetris operating system for the Raspberry Pi 4. It is a bare metal program that does nothing but play Tetris.

#### Hardware requirements
- Raspberry Pi 4 Model B
- Micro HDMI to HDMI cable
- [USB to TTL serial cable](https://www.amazon.com/s/ref=nb_sb_noss?url=search-alias%3Delectronics&field-keywords=+USB+to+TTL+Serial+Cable)
- microSD card
  
## Installation
1. [Download the image](https://github.com/nickrj/pitris/raw/main/tetris.img) and copy it to your SD card (e.g. `/dev/sdb`):
#### WARNING: this may overwrite your hard drive if you choose the wrong device!
```bash
sudo dd if=tetris.img of=/dev/sdb
```
2. Connect your USB to TTL serial cable by following [this guide](https://cdn-learn.adafruit.com/downloads/pdf/adafruits-raspberry-pi-lesson-5-using-a-console-cable.pdf). Skip the `Enabling Serial Console` section on page 4 since this is already enabled in the OS.
3. Launch your terminal emulator.
4. Power on your Raspberry Pi 4. You should see `Press space to start the game` from your terminal emulator.


## Build (Linux)
1. Install the [ARM GNU toolkit](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) and [mtools](https://www.gnu.org/software/mtools/):
```bash
sudo apt install gcc-aarch64-linux-gnu
sudo apt install mtools
```
2. Get the code:
```bash
git clone https://github.com/nickrj/pitris
cd pitris
```
3. Download two Raspberry Pi 4 firmware files: [start4.elf](https://github.com/raspberrypi/firmware/blob/master/boot/start4.elf) and [bcm2711-rpi-4-b.dtb](https://github.com/raspberrypi/firmware/blob/master/boot/bcm2711-rpi-4-b.dtb), and place them inside `pitris`.
4. Build `tetris.img`:
```bash
make
```
