This is the code for a portable Carbon Dioxide Sensor designed to be carried in a pressure sealed container in a cave.

The aim of the detector is to use the SCD4X sensor which is temperature and humidity compensated internally to provide accurate readings in high humidity cave environments.

Primary goal is to sample air quality beyond sumps as a safety step for cave divers and explorers.

Device is very simple uses the follwing components:

Board:
NodeMCU 1.0 ESP-12E

CO2 Sensor:
SCD41 (by sensirion)

OLED display:
SSD1306

Buzzer: 
Basically any piezo buzzer 

Basic guedelines for elevated CO2 effects on humans:

 * 0-400.    - NORMAL OUTDOOR AIR
 * 400-1000  - NORMAL FOR INDOOR
 * 1000-2000 - Common complaints of drowsiness or poor air quality
 * 2000-5000 - Headaches, fatigue, stagnant, stuffiness, poor concentration, loss of focus, increased heart rate, nausea
 * >50,000 ( >5% )  - Toxicity due to oxygen deprivation occurs
 * >100,000ppm ( >1e0% ) - Oxygen deprivation in seconds: convulsions, coma, and death
