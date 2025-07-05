# Steganography Project in C

My second project developed at Emertxe Information Technologies

## 🛠️ Technologies Used

* C Programming
* File Handling (BMP & Text Files)
* Bitwise Operations
* Structures
* String Manipulation
* Steganography (LSB Technique)

---

## 📖 Project Overview

This is a command-line based **Steganography Application** written in C.
It hides secret text data inside BMP images using **Least Significant Bit (LSB)** encoding.
The project demonstrates knowledge of bitwise operations, file I/O, image data manipulation, and modular programming.

---

## ✨ Features

1⃣ **Encode Secret Data**

* Hide text file inside BMP image
* Encodes file extension and file size
* Bitwise manipulation of image bytes

2⃣ **Decode Secret Data**

* Extract hidden data from Stego image
* Retrieves file extension, size, and content

3⃣ **Image Preservation**

* Output Stego image has same size as original BMP
* Visual distortions may occur (expected in raw BMP modification)

4⃣ **File Validations**

* Checks BMP format
* Validates input file paths

---

##📂 How to Compile

```bash
gcc *.c -o stego_app
```

---

## 💻 How to Run

**Encoding Secret File**

```bash
./stego_app -e source.bmp secret.txt stego.bmp
```

**Decoding Secret File**

```bash
./stego_app -d stego.bmp output.txt
```

---

## 👨‍💻 Author

Surya Jeyaraman

