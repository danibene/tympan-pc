import pyaudio
import wave

p = pyaudio.PyAudio()
dev_id = -1;
# used to get the device index of the Tympan
for i in range(p.get_device_count()):
    dev = p.get_device_info_by_index(i).get('name')
    print(str(dev))
    if("Teens" in str(dev)):
        dev_id = i
        print(i, p.get_device_info_by_index(i).get('name'))
        break
    elif("default" in dev):
        dev_id = i
        break
   # print(i, p.get_device_info_by_index(i).get('name'))

form_1 = pyaudio.paInt16 # 16-bit resolution
chans = 2 # 1 channel
samp_rate = 44117 #sampling rate
chunk = 128
record_secs = 5 # seconds to record
print(dev_id)
dev_index = dev_id # device index found by p.get_device_info_by_index(ii)
filename = 'test4.wav' # name of .wav file

# create pyaudio stream
stream = p.open(format = form_1,rate = samp_rate,channels = chans, \
                    input_device_index = dev_index,input = True, \
                    frames_per_buffer=chunk)
frames = []
# Store data in chunks for 3 seconds
for i in range(0, int(samp_rate / chunk * record_secs)):
    data = stream.read(chunk)
    frames.append(data)

# Stop and close the stream 
stream.stop_stream()
stream.close()
# Terminate the PortAudio interface
p.terminate()

print('Finished recording')

# Save the recorded data as a WAV file
wf = wave.open(filename, 'wb')
wf.setnchannels(chans)
wf.setsampwidth(p.get_sample_size(form_1))
wf.setframerate(samp_rate)
wf.writeframes(b''.join(frames))
wf.close()
