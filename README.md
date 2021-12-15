## Tympan-PC Pipeline

There are two main components in the pipeline code: 
 - The Arduino code which contains audio elements and USB transmission. 
 - The Python script which receives audio input and sends commands to the Tympan via serial transmission. 
 
The **LowPass_USB_Output** and the **ClapDetectorWhiteNoise** folders contain the Tympan code for low pass filtering and clap detection respectively :
 - LowPass_USB_Output : This code was used to demonstrate USB transmission. The cutoff frequency can be varied using the potentiometer on the Tympan. For more details refer to this *[tutorial](https://gitlab.critias.ca/critias_projects/stage/stage_nithin_raj/try/-/wikis/Transmitting-Audio-From-the-Tympan-to-a-PC)* in the wiki.
 - ClapDetectorWhiteNoise : This code plays white noise when a clap is detected by the Python script running on the external PC. 
 
To test the clap detection script:
 * Run the ClapDetectorWhiteNoise code on the Tympan. 
 * Open the **external_scripts** folder, and install the python modules by running **pip3 install -r requirements.txt**
 * Now, install the pi-clap module by running **pip3 install -e git+https://github.com/danibene/pi-clap#egg=pi_clap**
 * Run the **clap.py** script (found in the *external_scripts* folder) 
 * Press c to confirm the default settings (recalibrator is a bit buggy for a chunk size of 1024) and clap away!
 * If you clap twice, *'m'* is sent as the serial command (indicating masking noise toggle) to the Tympan. This should toggle the masking white noise in the audio output.  
