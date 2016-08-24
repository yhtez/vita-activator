# Vita Activator
Use this to get activation data on a HENkaku enabled PlayStation Vita

# Requirements


# Compiling
Either download latest vita-activator.vpk under releases  
or  
Clone repo to your pc  
```
git clone http://github.com/ZombieWizzard/vita-activator.git
cd vita-activator
```
and build from sources
```
make clean
make (all|vpk|eboot|velf) CONF=RELEASE
```
or to disable SSL certification (useful for debugging with a proxy)
```
make clean
make (all|vpk|eboot|velf) CONF=DEBUG
```
vita-activator.vpk and eboot.bin will be in build folder

# Usage
Once installed run Vita Activator and then follow on screen instructions

# License
You are free to use this code for any purposes
