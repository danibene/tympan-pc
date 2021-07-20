## Tympan-PC Pipeline

There are two main components in the pipeline code : 
 - The Arduino code which contains audio elements and USB transmission. 
 - The Python script which receives audio input and sends commands to the Tympan via serial transmission. 
 
The **LowPass_USB_Output** and the **ClapDetectorWhiteNoise** folders contain the Tympan code for low pass filtering and clap detection respectively :
 - LowPass_USB_Output : This code was used to demonstrate USB transmission. The cutoff frequency can be varied using the potentiometer on the Tympan. For more details refer to this *[tutorial](https://critias.ca/dokuwiki/doku.php?id=i:abc:arp:arp_4_0:tympan_usb)* in the CRITIAS wiki.
 - ClapDetectorWhiteNoise : This code (yet to be completed) plays white noise as and when a clap is detected by the Python script running on the external PC. 
 
To test the clap detection script:
 * Run the ClapDetectorWhiteNoise code on the Tympan. 
 * Open the **external_scripts** folder, and install the python modules by running **pip3 install -r requirements.txt**
 * Now, install the pi-clap module by running **pip3 install -e git+https://gitlab.critias.ca/critias_projects/stage/stage_nithin_raj/pi-clap#egg=pi_clap**
 * Run the **clap.py** script (found in the *external_scripts* folder) 
 * Press c to confirm the default settings (recalibrator is a bit buggy for a chunk size of 1024) and clap away!
 * If you clap once, you should also see *One clap detected* on the serial monitor found in the Arduino IDE. 

Make sure that the **clap.py** code and the Arduino code have the same sampling rate. The default sampling rate for USB transmission is *44,117 Hz*. In case you wish to change the sampling rate, follow this *[tutorial](https://critias.ca/dokuwiki/doku.php?id=i:abc:arp:arp_4_0:tympan_usb)*. 
