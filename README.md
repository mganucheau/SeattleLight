SeattleLight
============
Arduino code for Rebar Group's SeattleLight project - http://rebargroup.org/  
by Matt Ganucheau , Mary Franck and Benjamin Chun

Libraries Used:  
WiFlyHQ - https://github.com/harlequin-tech/WiFlyHQ  
LPD8806 - https://github.com/adafruit/LPD8806  

Files:   
hub             - code for the sequencing hub  
planter         - code for each planter  
lights_com_test - used to test the LPD8806 over a direct serial connection  

------------------------------  
Commands for the Planters  
  
Z - Clears the strip  
H - Color Chase Mode  
W - Color Wipe Mode  
C - Rainbow Mode  
T - Tinkle Mode  
O - Solid ColorMode  
M - Screensaver Mode  

U - Fades brightness up  
D - Fades brightness down  

R0 - R9  - sets Red Value  
G0 - G9  - sets Green Value  
B0 - B9  - sets Blue Value  
P0 - P9  - sets Speed  
Z0 - Z9  - sets Brightness  

S0 - All Sides  
S1 - First Side  
S2 - Second Side  
S3 - Third Side  
S4 - Last Side  

N1 - First Corner  
N2 - Second Corner  
N3 - Third Corner  
N4 - Last Corner  
