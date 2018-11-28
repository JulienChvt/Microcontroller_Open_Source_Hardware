# Connected Gas Sensor

Project done for the **MICROCONTROLLERS AND OPEN-SOURCE HARDWARE** course of **INSA Toulouse**.

**Authors :** Jean-Baptiste Laffosse - Julien Chouvet

# General

The aim of this project is to build a **connected gas sensor**.

To be more specific, we use **RN2483** chip, connected to an       **Arduino Uno**, to send on the **LoRa Network** the values returned by the **MQ-9 gas sensor**. Thanks to **The Things Network** and **Node-RED** we create a **dashboard** as a **user interface**. In this dashboard, the user can **monitor the value of the gas sensor** and **calibrate** it.

We also add a **gas alert function** which is triggered when the value return by the MQ-9 goes over a threshold value. As we cannot attach an interrupt on a analog input, to do this, we used a **Schmitt trigger** connected to a numerical pin of the Arduino Uno. Thus, when an alert is detected (rising or falling edge, mode **CHANGE**), the interruption triggered a function. The user is informed of this alert by a **buzzer** and by a **message displayed on the dashboard**.

Finally, we develop a shield that we can plug on the **Arduino Uno** microcontroller. So we use **KiCAD** to sketch all the circuits of the shield.


## Hardware Components

**- Arduino Uno** : https://store.arduino.cc/usa/arduino-uno-rev3

**- MQ-9 Gas Sensor** : http://www.mouser.com/catalog/specsheets/Seeed_101020045.pdf

**- Microchip RN2483** : http://ww1.microchip.com/downloads/en/DeviceDoc/50002346C.pdf

**- Operational Amplifier LM741** : http://www.ti.com/lit/ds/symlink/lm741.pdf 


## Arduino code

For this project we used an **Arduino Uno board**.

We developed the code on the **Arduino IDE** : https://www.arduino.cc/en/Main/Software

The source code of the project is available on the *GasSensor_TTN_LoRa directory*.

You can find more information on this code on our report.

## Schmitt trigger

We use a Schmitt trigger to detect when the value of the gas sensor goes over a threshold value.

The design and the LTSpice simulation of this electronic circuit are available on the *Schmitt_Trigger directory*.

You can find more information on the Schmitt trigger on our report.

## The Things Network

The Things Network (TTN) is a worldwide community LoRa network provided an end-to-end stack for IoT. 
To configure TTN follow these steps :

 1. Go to https://www.thethingsnetwork.org/ and create an account
 2. Create an **application** : https://www.thethingsnetwork.org/docs/applications/add.html
 3. Add a **device** in your application : https://www.thethingsnetwork.org/docs/devices/registration.html
 **Note :** we use **ABP** activation method

You can find more information on how we used TTN to send/receive messages to/from the Arduino, on the Arduino code and also on the report.
You can also find the **Decoder** and **Validator function** on the *TTN directory*.

## Node-RED

Node-RED is a flow-based development tool for visual programming developed originally by IBM for wiring together hardware devices, APIs and online services as part of the Internet of Things. (source : https://en.wikipedia.org/wiki/Node-RED#cite_note-2)

We used it to create a dashboard, in which the user can see the value of the gas sensor and can also ask to calibrate the sensor.

To install Node-RED for TTN follow these steps :

 1. Follow this tutorial to install Node.js and NodeRED : https://nodered.org/docs/getting-started/installation

 2. To send/receive messages to/from devices through TTN, you need to install the TTN node for NodeRed :

        cd  $HOME/.node-red
        npm install node-red-contrib-ttn
        

 3. To create a dashboard, you need to install a specific module :

		cd  $HOME/.node-red
        npm install node-red-dashboard
    
To run Node-RED, open a prompt and type *node-red*. Then open a browser and type the following URL : *http://127.0.0.1:1880/*

You can find the Node-RED flow made for this project in the *Node_RED directory*.
Have a look at our report for more information.    

## KiCAD

KiCad is an **open-source software tool** for the creation of **electronic schematic diagrams** and **PCB artwork**. The main avantage with KiCAD is that the software offers a **singular interface** for all the steps needed during the **development of the shield**. This is why we choose KiCAD to develop our shield. 

You can find all the documentation needed here : 
http://docs.kicad-pcb.org/stable/en/getting_started_in_kicad.html

To develop the shield or open our files, you need to follow these steps :

1. First, you need to download KiCAD at the following address and choose your operating system. **KiCAD** will allow you to open the project file **.pro** of the shield. 
http://www.kicad-pcb.org/download/

2. You can start sketching your circuit thanks to **Eeschema**. If your component does not exist you can create it or import it from an external library. With **Eeschema**, you can open all the **.sch, .lib, .net files**, and like this see our circuit contained in the **.sch file**.

3. **Annotate**, and perform the **rule check**. Make sure you don't have any errors. Then you can **associate each component with their footprint**.

4. Generate the **Netlist** of your schematic, in fact you will generate a **.net file**.

5. You can now create your PCB. For this open **PCBnew** and read the **Netlist**. Perform the **rule check** and make sure you don't have any errors. PCBnew allows you to open all the ** *.kicad_pcb and *.kicad_mod files**, and our PCB is contained in the **.kicad_pcb file**.

6. Export your **Gerber file** from PCBnew. If you want, you can inspect your file from **GerbView**. All the gerber files generated can have either the default **.gbr** extension, or the **.g** extension (one for each layer) :
http://docs.kicad-pcb.org/stable/en/getting_started_in_kicad.html#generate-gerber-files


On the *KiCAD directory* you can find two projects :

 1. Design of an Arduino Board with the Schmitt Trigger and a socket for the *MQ-9* gas sensor. This design corresponds to the project describe so far.
 2.  Design of an Arduino Uno board with the gas sensor and a transimpedance montage.

