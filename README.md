

# StratoSoar MK3 - UAVs For the Masses

<!-- **TL;DR**: The StratoSoar project is a low-cost, lightweight UAV designed to be dropped from a weather balloon  at any altitude, aiming for autonomous, efficient flight to selected GPS coordinates. The hope is to get a UAV platform in the hands of educators, researchers, and hobbyists alike since StratoSoar is much smaller, lighter, and cheaper than conventional UAVs.  -->

**TL:DR**: The StratoSoar project is a low-cost, lightweight Unmanned Aerial Vechile (UAV) designed for deployment from a weather balloon at various altitudes, aiming for autonomous and efficient flight to designated GPS coordinates. By offering a more affordable, compact, and lightweight alternative to conventional UAVs, StratoSoar aims to empower educators, researchers, and hobbyists with an accessible platform for exploration and experimentation.

## Get Up To Speed on The Project

<!-- **WORK IN PROGRESS** - This project is still in the works. I don't recommend using it yet because it has the potential of being dangerous.

**NEW** - Writing code and making the PCB. 

**STATE as of 7/1/24** - Just starting development! -->

<!-- <div style="max-width: 500px; margin: 0 auto;">
  <div style="position: relative; padding-bottom: 56.25%; height: 0; overflow: hidden; max-width: 100%; height: auto;">
    <iframe style="position: absolute; top: 0; left: 0; width: 100%; height: 100%;" src="https://www.youtube.com/embed/TiqkcGWG4g8" title="StratoSoar MK2/3: A Review" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>
  </div>
</div>

<br /> -->

<img src="https://github.com/crnicholson/StratoSoar-MK3/raw/main/Media/glider.png" alt="Glider" width="300"/> | <img src="https://github.com/crnicholson/StratoSoar-MK3/raw/main/Media/mk3.0.png" alt="PCB" width="300"/>
-- | --
<i>StratoSoar glider.</i> | <i>StratoSoar MK3.0 circuit board.</i>
<img src="https://github.com/crnicholson/StratoSoar-MK3/raw/main/Media/cfd.png" alt="Glider" width="300"/> | <img src="https://github.com/crnicholson/StratoSoar-MK3/raw/main/Media/soldering.png" alt="PCB" width="300"/>
<i>Computational fluid dynamic sims.</i> | <i>Soldering of the board.</i>

## What Is the StratoSoar Project?

StratoSoar is a low-power, adaptive UAV that is dropped from a weather balloon at approximately 5,000 feet, with plans to reach even higher altitudes soon. The main goal of StratoSoar is to fly to selected GPS coordinates efficiently or choose a closer, more realistic landing site without human interaction. The current aim for StratoSoar is to operate on a single 600 mAh 15 gram LiPo battery for a 10+ hour flight. 

## Now, What Is a UAV?

UAVs, or Unmanned Aerial Vehicles, are autonomous aircraft, usually in the form of planes or drones. They have many applications, such as [delivering medicine, food, and supplies](https://www.flyzipline.com/) to remote areas, participating in aerial combat with the military, and conducting aerial imaging.

### Common Types of UAVs Include:

- Quadcopter (typical drones)
- Fixed-wing aircraft (like StratoSoar)

## StratoSoar vs. Conventional UAVs

### Traditional UAVs Are Typically:

- Large (5-15 meter wingspan)
- Heavy (2-20 kg)
- Expensive ($10,000-10,000,000 USD)
- Closed source and propietary

### StratoSoar Aims to Overcome These Limitations With:

- Weight: less than 250 grams
- Wingspan: 80 cm
- Cost: less than $150 USD
- Open source and accesible

## What Are the Uses of StratoSoar?

I'm currently considering two main concepts: an educational kit for students and hobbyists or a platform designed to study the aerodynamics of the stratosphere, a field with limited existing resources. Additionally, I'm exploring other ideas such as reusable radiosondes, systems for remote delivery of medicine and parcels, and advanced atmospheric monitoring solutions.

<!-- ## What Includes the StratoSoar Project? What Are the Different Parts of it?

StratoSoar has many different parts, and they all combine to make this project. 

- **Code** - All of the code directory, and the programs needed to run it. These include the main `.ino`s and the accompanying `.cpp` and `.h` files, along with the test files for the IMU and GPS.
- **PCB** - The circuit board is the basis of this project, it interfaces the code with the glider. StratoSoar includes the circuit board files and the schematics and everything needed to build a PCB.
- **3D Files** - StratoSoar requires some 3D printed parts to function, but these files also serve as an interactive build guide. StratoSoar includes the 3D files needed to print parts for the glider and the 3D files to assemble the glider.
- **Media** - The images and such of StratoSoar. 
- **Cutdown Mechanism** - This is the thing that helps you release the glider from the balloon payload - or whatever you want to drop it from. This includes all of the above things but will be in a separate GitHub repository.  -->

## How Does StratoSoar Work at a High Level?

<!-- StratoSoar is attached to a weather balloon via the cutdown mechanism. Once the balloon reaches the desired altitude, the cutdown mechanism drops the glider. The glider then descends, steering its way to specified coordinates. 

StratoSoar takes pride in being highly configurable and hackable, so there are many ways you change how it comes down built into the code. 

StratoSoar uses data from an IMU that goes through a Kalman filter to receive accurate AHRS, or Attitude Heading Reference System. This can tell StratoSoar the pitch, yaw, and roll of the glider. Then, StratoSoar integrates a GPS module to get the GPS coordinates. Both the GPS and the AHRS are used to compute the azimuth to find the turning angle of the glider. The turning angle tells the glider how much it needs to turn to be facing the correct yaw that will get the glider to the target location. Additionally, an algorithm is used to get the distance between the glider location's and the target location. 

With the turning angle computed, a PID mechanism is used to control the servo attached to the rudder of the glider. This PID mechanism learn from it's mistakes and can accurately aim the glider in the correct direction. 

Additionally, the glider beacons it's location at a configurable interval with LoRa for tracking. It also allows for a camera module to be added on to take photos and video. 

The glider will fly to the selected coordinates, and depending on the user configuration, will either spiral down or release a parachute above the coordinates once it has reached them to safely descend. Additionally, if the glider is too low in altitude, prior to the flight the user can enter closer coordinates as backup landing locations.  -->

StratoSoar MK3 is an advanced autonomous glider designed to operate at high altitudes, such as those reached by weather balloons. The glider is attached to a weather balloon via a cutdown mechanism. Once the balloon reaches the desired altitude, the cutdown mechanism releases the glider, initiating its descent. The glider then navigates autonomously to specified coordinates, using a combination of sensors, advanced algorithms, and low-power electronics.

### Core Components and Functionality:

- **Autopilot System**:
    - **GPS and IMU Integration**: StratoSoar uses a custom GPS module and an IMU, both designed for low-power operation. The IMU data is processed through a Kalman filter to provide accurate Attitude Heading Reference System (AHRS) data, including pitch, yaw, and roll. This information, combined with GPS coordinates, enables precise navigation.
    - **Advanced Waypoint Navigation**: The waypoint navigation system is designed for complex flight paths, incorporating advanced turning algorithms to ensure smooth and accurate direction changes.
    - **Servo Control with PID**: A PID control mechanism governs the rudder’s servo, ensuring precise adjustments to the glider's course based on real-time feedback.

- **Communication and Data Logging**:
    - **LoRa Telemetry**: The glider continuously beacons its location via LoRa communication, which includes forward error correction for reliable data transmission. This allows for real-time tracking and data collection.
    - **Data Logging**: Critical flight data, including sensor readings and GPS coordinates, are logged using a combination of SD card and EEPROM storage, optimized for low power consumption.
    - **Camera**: If configured, the glider uses a custom designed camera module to take photos at specific coordinates or record the entire flight.  

- **Modularity and Customization**:
    - **Hackability**: StratoSoar is designed to be highly configurable and hackable. Users can customize its descent strategy, whether by spiral descent, parachute deployment, or alternative landing locations. The codebase is modular, allowing for easy modifications or the addition of new features.
    - **Camera Module**: An optional low-power camera module can be added to capture photos or video during the flight, with Python scripts available for post-processing the footage.

- **Ground Station and Data Handling**:
    - **Ground Station**: The ground station, built on an ESP32 platform, receives telemetry data from the glider via LoRa. It logs this data locally and communicates with a server using HTTP requests. A Flask-based uploader script then transfers the data to SondeHub for analysis and storage.

- **PCB Design**:
    - **StratoSoar MK3 PCB**: The main PCB, optimized for power efficiency, size, and cost, integrates over 100 components, managing the various systems onboard.
    - **Additional PCBs**: Supporting PCBs, such as those for the GPS module, LoRa communication, camera, and IMU, are designed for specific tasks, enhancing the glider's overall functionality.

### Operation and Recovery:

Upon release from the weather balloon, StratoSoar begins its guided descent following waypoints or single target coordinate. It calculates the optimal flight path using its GPS and AHRS data, making real-time adjustments to its trajectory through the PID-controlled servo system. Depending on user configurations, the glider can either glide smoothly to the target coordinates, initiate a spiral descent, or deploy a parachute for a safe landing.

If the glider's altitude drops below a certain threshold, backup landing coordinates can be used to ensure a safe recovery. Throughout the flight, the glider’s location is continuously transmitted, allowing for easy tracking and recovery.

## File System and More Project Info

I'm only listing the key items, leaving out all the unessecary stuff. This should give you a general sense of the project and all it's features. Note that this is also only stuff I did during Hack Club Arcade.

- `Code/autopilot`: (5000 lines of code!)
    - `autopilot.ino`: The script that ties all of these files together.
    - `gps.cpp`: GPS code custom-made for low-power.
    - `waypoint.cpp`: Custom waypoint flight with advanced turning.
    - `lora.cpp`: LoRa communications with complex forward error correction.
    - `sd.cpp` + `eeprom.cpp`: Data logging in a low-power manner.
    - `servo.cpp`: Ultra low-power servo functions.
    - `bme280.cpp`: Weather data sensor.
    - `imu.cpp`: An interial measurement unit running a Kalman filter.
- `Code/groundStation`: 
    - `groundStation.ino`: The code for the receiving station of the LoRa telemetry, uses ESP32 and HTTP requests to communicate with the server, also locally logs data. 
    - `uploader/flaskUploader.py`: A Flask server that uses an API to put all the data on SondeHub. 
- `Code/camera`: Utilization of a low-power camera module and Python scripts for conversion.
- `Code/calculator`: Python script to parametrically create flying wing glider. 
- `PCB/StratoSoar-MK3.0`: The main PCB consisting of over 100 components, optimized for power, size, and cost.
- `PCB/gpsBreakout`: A PCB designed for testing a cheap, $2 Chinese GPS module.
- `PCB/loRa`: The ground station to receive the glider's data. 
- `PCB/camera`: A breakout board for a low-power camera module. 
- `PCB/imuBreakout`: One of the first breakout boards made for a new Interial Measurement Unit. 
- `3D Files/cardboardGlider`: Parametric Fusion 360 desgin for a folding origami glider. 
- `3D Files/StratoSoar Rev2 v36`: High efficieny glider.

## A Quick Note on MK1, MK2, Naming, and Revisions

StratoSoar MK1 was the precursor and the spark to StratoSoar MK2. StratoSoar MK1 was officially developed from October 1st to July 30th, although StratoSoar MK2 was unofficially being worked on in the months of March through June. StratoSoar MK1 failed to fly autonomously, and it also failed to release from the balloon. Major upgrades have been made since then.

StratoSoar MK2 officially started on October 1st, and will end has now ended. It was a success. 

StratoSoar MK3 officially started on June 1st, and will end in quite some time when I launch it from a weather balloon. A new major revision (eg. MK4) will come out every time when I release the glider from the balloon in the Summer. After the Summer, work on the next revision begins, with many updates and new features. 

<!-- A new sub-revision (for lack of better word) comes out every time I make a new PCB. For example, my first version of my PCB was MK2.0. At the time of writing, the current version of StratoSoar is MK2.1. A new version of 3D model is denoted by the Rev number. Rev starts at 0. 0 and 1 are not shown in this repo because they didn't turn out very good. The physical gliders are denoted by the 3D model number. Sometimes a small change is made with glider so the name of the glider is Rev x.x, for example. Sometimes the same happens with the 3D model, again it would be x.x. To put this in practice, the final glider may be MK2.5 Rev 7.3.1. The code has no version history in the way the 3D models and the PCB do. -->

Also, I use "we" a lot in this writing. This usually refers to myself or this project, but it also refers to my small team over at New England Weather Balloon Society.

One last thing. When I refer to StratoSoar, I am referring to the project as a whole, but more specifically, the most recent version. 

<!-- ## Watch my Talk About StratoSoar MK2 The North East HamXpo! -->

## Is This Legal?
Yes! Since the glider is unpowered, and during flight you have no control of where the glider goes, it qualifies as a free flight glider. Free flight gliders have very little regulations. You can think of them as a paper airplane. 

## Can I Buy a Kit or a Complete Product?

Currently, StratoSoar is not available to purchase online as a kit or complete product. We have plans in the next few years to release a kit for schools, education centers, hobbyists and a complete product for industry at an affordable price. You are welcome to make one in the mean time!

## How Can I Make One? Are There Docs?

One of the goals of StratoSoar was to make an affordable and open source system for anyone to perform stratospheric research, so all the files are available for free. This means that you can construct StratoSoar from scratch. **A guide to do this will be available soon.**

<!-- It is still in the works, but you can check out `DOCS.md` under the `Docs` folder.**  -->

## Helpful Sources and Credits

I'm in the midst of compiling this.

## Cutdown Mechanism

[See my other WIP GitHub repo.](https://github.com/crnicholson/Cutdown-Mechanism)

## Funding and Support

We currently have <u>**very**</u> limited funding to complete this project. We would **love any donations or sponsorships** we could get to elevate (no pun intended) this project to the next level. If you don't want to donate, but want to help out, we would love your support with other areas of this project. Reach out to me if you have interest at:

- [newbsballoons@gmail.com](mailto:newbsballoons@gmail.com) (organization email, recommended).

- [charlienicholsonr@gmail.com](mailto:charlienicholsonr@gmail.com) (personal email).

## Special Thanks

This project would not be possible without the following individuals:

- Max Kendall (W0MXX) for working closely together with me.

- Mike Malis for helping with the design of the glider. 

- Brett Miwa for schematic review, PCB design, electrical theory, and boost converter circuitry design.

- Bob Phinney (K5TEC) for providing NEST and securing funding.

- Mark Jessop (VK5QI) for answering all of my questions.

- Mike Hojnowski (KD2EAT) for PCB design and hardware/software debugging.

- Paul Hamilton for helping me with FAA regulations.

- My parents!

And these entities:

- Hack Club, for providing support for projects like this. 

- New England Sci-Tech (NEST) for providing a maker space to work, funding, and hosting NEWBS.

- New England Weather Balloon Society (NEWBS) for expertise and the opportunity.

- Bagel Fund, for providing funding. 

## Affiliate Programs and Projects

- **Hack Club** - Hack Club is a non-profit designed for teenagers that provides support for projects like this. 
- **NEWBS** - Max and Seth Kendall of NEWBS, or New England Weather Balloon Society, are helping me launch my project from a balloon. They are dedicated to helping people in New England launch weather balloons. 
- **New England Sci-Tech** - Bob Phinney of NEST is hosting NEWBS. NEST is a great maker space. If you are in the Greater Boston Area, go check them out!
- **StratoScience** - This is the official class that I'm in helping me launch my project. This is a high altitude engineering class for teens. Check them out if your are in the Greater Boston area!

## Still Have Questions?

Do you have any questions? Are the docs incomplete and you want to ask anything? Do you just want to say hi (or how cool my project is!)?

Reach out to me at [charlienicholsonr@gmail.com](mailto:charlienicholsonr@gmail.com).

## License

Both the software and documentation are under the [GNU GPL v3 license](https://choosealicense.com/licenses/gpl-3.0/). The hardware and the 3D files are under the [CERN Open Hardware Licence Version 2 - Strongly Reciprocal](https://choosealicense.com/licenses/cern-ohl-s-2.0/). The media is under the [CC BY 4.0 DEED](https://creativecommons.org/licenses/by/4.0/).
