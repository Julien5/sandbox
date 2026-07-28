# nano

Android build and run:

- connect over wifi: 
```
adb connect 192.168.1.101:41353
```

- build crate
```
~/projects/notes/tools/build.sh --target android-arm64 --mode debug
```

- set/unset simulation
```
adb shell setprop debug.nano.sim 0 # no sim 
adb shell setprop debug.nano.sim 3 # 3 loops
adb shell setprop debug.nano.sim infinity # loops forever 
```

- start on device 
```
2>&1 flutter run -d 192.168.1.101:41353 --verbose | tee /tmp/run.lo
```
