# Flowchart2Code
A C++ based cross-platform application to convert programming flowcharts to code in C, C++ and Python, made as the Advanced-Track project for the course ESC 101 under Prof. Nisheeth Srivastava. The application converts images of hand-drawn flowcharts and flowcharts in XML format. GUI of the application has been created using wxWidgets framework, OpenCV has been used for image processing and running the pre-trained neural network, and tesseract library for recognizing text in images.

## Running the Application:
There are two ways to run the application:
1. Clone the repository and then run from /bin/(Your Operating System). OR
2. Download only the Application from the given below links according to your operating system:
   * [WINDOWS](https://drive.google.com/file/d/1GHOQw_KVzaKQLkrZWPs4eTXW35f8IPbY/view?usp=sharing)
   * [LINUX](https://drive.google.com/file/d/1s4tiCQWewSPi8f7INXbmcyMWT9QXghsA/view?usp=sharing)
   * [MACOS](https://drive.google.com/file/d/1rr_0ztOAnC067Lp-M4aeG2i38P6YYSoH/view?usp=sharing)
   
## Specific Instructions to Run the Application for Linux Users:
Run the following commands once in terminal for installing dependencies: (If running Debian based linux)
```
sudo apt install leptonica-dev libdc1394-22 libavcodec-dev libavformat-dev libswscale-dev
```
For libjasper as it is not available now...
```
sudo add-apt-repository “deb http://security.ubuntu.com/ubuntu xenial-security main”
sudo apt update
sudo apt install libjasper-dev
```
Install the same libraries if running a non-Debian based linux using their corresponding package managers.
After installing the dependecies run the following code to open the application:
```./Flowchart2Code```

### Guidelines regarding how input is to be given can be found [here](https://github.com/sanyog42/Flowchart2Code/blob/master/Guidelines.pdf)

### Link to the Model used: https://github.com/mlcv-ime/faster-rcnn-graphics
