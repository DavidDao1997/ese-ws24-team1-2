# Image generation

To make FSM diffs more trackable we commit the generated FSM pngs to [/images/FSM](/images/FSM). 

The following FSMs are tracked:

 - [System](/images/FSM/00_System.png)
 - [Evaluate](/images/FSM/01_evaluate.png)
 - [Festo1 Errors](/images/FSM/Festo1/00_Errors1.png) / [Festo2 Errors](/images/FSM/Festo2/00_Errors2.png)
 - Sector FSMs
    - [Festo1 Ingress](/images/FSM/Festo1/10_Ingress1.png) / [Festo2 Ingress](/images/FSM/Festo2/10_Ingress2.png)
    - [Festo1 HeightMeasurement](/images/FSM/Festo1/11_HeightMeasurement1.png) / [Festo2 HeightMeasurement](/images/FSM/Festo2/11_HeightMeasurement2.png)
    - [Festo1 Sorting](/images/FSM/Festo1/12_Sorting1.png) / [Festo2 Sorting](/images/FSM/Festo2/12_Sorting2.png)
    - [Festo1 Ingress](/images/FSM/Festo1/13_Egress1.png) / [Festo2 Egress](/images/FSM/Festo2/13_Egress2.png)
 - Actuators
    - [Festo1 Motor](/images/FSM/Festo1/Actuators/00_Motor1.png) / [Festo2 Motor](/images/FSM/Festo2/Actuators/00_Motor2.png)
    - [Festo1 LightGreen](/images/FSM/Festo1/Actuators/01_LampGreen1.png) / [Festo2 LightGreen](/images/FSM/Festo2/Actuators/01_LampGreen2.png)
    - [Festo1 LightYellow](/images/FSM/Festo1/Actuators/02_LampYellow1.png) / [Festo2 LightYellow](/images/FSM/Festo2/Actuators/02_LampYellow2.png)
    - [Festo1 LightRed](/images/FSM/Festo1/Actuators/03_LampRed1.png) / [Festo2 LightRed](/images/FSM/Festo2/Actuators/03_LampRed2.png)

## How to update FSMs

After changes are made to the FSM.ysc and the generated code, rightClick the changed region in itemis and 

![screenshot-generate-region.png](/images/screenshot-generate-region.png)