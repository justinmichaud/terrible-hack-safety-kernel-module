## About
This is the most innovative way to prevent ico fraud and protect your money on the volatile crypto market. It causes a Blue Safety nOtification that is Decentralized (BSOD) when you type the word "blockchain."

## Warning
This is a kernel module. It is very dangerous to run. Use at your own risk. Also, if you follow the secure boot signing instructions below, keep in mind that adding a new MOK key allows anyone with the key to completely bypass secure boot.

## How it works
I used https://github.com/jarun/keysniffer as a base to register a keyboard listener (see keyboard.c). When it detects the correct word has been typed, it adds a "just in queue" workqueue task to start the BSOD (otherwise, this will hang the keyboard listener, since it is an interrupt). See terminal.c for that. Finally, it calls the userspace commands chvt to change the terminal, and fbi to write directly to the frame buffer to display the image.

Useful links:
- https://www.oreilly.com/library/view/linux-device-drivers/0596005903/ch07.html
- https://github.com/fffaraz/kernel/blob/master/ldd3-examples/examples/misc-modules/jiq.c

## Usage
- Install FBI (framebuffer image viewer)
- Change path in terminal.c to point to the absolute location of bsod.png on your machine. 
- You may also need to change the tty from 3 in the code. Press CTRL+Alt+F[1-8] to see which tty is available. It should have a text-mode login prompt.
- Build and load the module (see below)
- Type "blockchain"
- You should get a "bsod". After 5 seconds, it will go away and you will see the tty. You can press Ctrl+Alt+F2 (on Fedora, your machine may differ) to get back to your graphical tty. One of CTRL+ALT+F1-F8 should have your existing graphical session, so try them all. Make sure you release and re-pres ctrl+alt every time if you want to cycle through them.

Tested on Fedora 28, 4.16.15-300.fc28.x86_64

## How to build with secure boot
Generate a key:
```
openssl req -x509 -new -nodes -utf8 -sha256 -days 36500 -batch -config secure_boot.config -outform DER -out public_key.der -keyout private_key.priv
```
Add this key to your Machine Owner Key list (Danger! Make sure you only do this temporarily!):
```
sudo mokutil -#-import public_key.der
```
*Remove it when you are done* (if you delete it accidentally there are some steps online to get it back):
```
sudo mokutil --delete public_key.der
```

## How to build and insert into your running kernel
```
make
sudo make load
```

## To unload
```
sudo make unload
```

## To view info & debug
```
modinfo terrible.ko
```

When loaded, use dmesg to view output. Enable debug in the makefile for more output.
