# MotionCueingInterface

The Motion Cueing Interface plugin is a c++ code sample to help motion system manufacturers with the creation of a plugin that will interface with motion platforms. 

In the realm of motion systems, Unreal is the Host, providing data to the Controller, a motion platform. The controller needs to get data at high frequency for quality purposes (100-1000Hz). Unfortunately, the usual Unreal project will run at a lower frequency, usually between 30-60 Hz.

The idea is to collect the necessary data from a virtual moving object in Unreal at low frequency. Then we interpolate between “Previous Data Set” and “Current Data Set” and send it at high frequency to the controller. This solution adds a lag equal to the low frequency interval (33ms/16ms ---> 30/60 Hz). If lag becomes an issue, we can instead interpolate between “Current Data Set” and a “Prediction Data Set”, which is not implemented for now.
