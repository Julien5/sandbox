# GPX Segments Tool

## What does it do ?

The tool parses the GPX files from a directory. 
It cleans the content, because:
- One route might be present in several gpx files. (This happens if you don't remove the files from the GPS device after a backup).
- The device is typically turned on *before* the start of a tour and turned off *after* its end. These parts of the data are removed.

Then the tool classifies the tour into three categories: running, cycling, and "none". The classification is based on the average speed.

For each category, the tool tries to find out the segments in your data where you have been mutliple times. It will find short segments which have been visited many times, and larger segments (which have been less visited). It displays some statistics about the segments.

## How To Use

### Prerequisite

python modules:
- gpxpy
- pyproj

The tools general gnuplot files in `/tmp` for visualization.

You need to have the GPX files you want to analyse in one directory, which will be searched recursively for `Track_*.gpx` and `Current.gpx` files (the way my etrex names the tracks).

## Example

Assuming i have some GPS files under `~/tracks` in my home directory:
```
read files..
clean tracks..
categorizing..
OK
# category cycling
2022.10.19-005  | 12:51 |  26.1 km | 01:01 | 25.6 kmh |
2022.10.16-003  | 08:48 |  26.1 km | 01:04 | 24.2 kmh |
2022.10.15-002  | 17:34 |  31.4 km | 01:27 | 21.5 kmh |
2022.10.22-006  | 17:25 | 342.5 km | 25:30 | 13.4 kmh |
total cycling   :  426.2 km | 29.1h
-------------------------------------------------------
2022.10.15-002  | 18:28 |   1.4 km | 00:02 | 29.1 kmh |
2022.10.22-006  | 17:25 |   1.4 km | 00:04 | 18.5 kmh |
2022.10.22-006  | 18:53 |   1.4 km | 00:03 | 21.7 kmh |
/tmp/cycling-segment-0.gnuplot #visits:  2
------------------------------------------------------------
2022.10.15-002  | 18:28 |   1.3 km | 00:02 | 29.0 kmh |
2022.10.16-003  | 09:47 |   1.2 km | 00:02 | 30.3 kmh |
2022.10.19-005  | 13:47 |   1.2 km | 00:02 | 29.0 kmh |
2022.10.22-006  | 17:26 |   1.2 km | 00:03 | 18.5 kmh |
2022.10.22-006  | 18:53 |   1.2 km | 00:02 | 24.8 kmh |
/tmp/cycling-segment-1.gnuplot #visits:  4
------------------------------------------------------------

# category running
2022.10.15-001  | 15:48 |   6.4 km | 01:23 |  4.7 kmh |
2022.10.17-004  | 12:25 |   6.9 km | 00:46 |  9.0 kmh |
2022.10.11-000  | 16:52 |   7.1 km | 00:49 |  8.6 kmh |
total running   :   20.4 km |  3.0h
-------------------------------------------------------
2022.10.15-001  | 15:48 |   3.5 km | 00:38 |  5.5 kmh |
2022.10.15-001  | 17:02 |   0.5 km | 00:06 |  4.9 kmh |
2022.10.15-001  | 17:09 |   0.1 km | 00:01 |  3.7 kmh |
2022.10.17-004  | 12:25 |   3.4 km | 00:22 |  8.8 kmh |
2022.10.17-004  | 12:55 |   1.3 km | 00:08 |  9.4 kmh |
2022.10.17-004  | 13:11 |   0.1 km | 00:00 |  8.4 kmh |
/tmp/running-segment-0.gnuplot #visits:  2
------------------------------------------------------------
2022.10.11-000  | 16:52 |   1.8 km | 00:12 |  9.1 kmh |
2022.10.11-000  | 17:23 |   2.6 km | 00:18 |  8.4 kmh |
2022.10.17-004  | 12:25 |   1.9 km | 00:12 |  9.3 kmh |
2022.10.17-004  | 12:55 |   2.6 km | 00:16 |  9.3 kmh |
/tmp/running-segment-1.gnuplot #visits:  2
------------------------------------------------------------
2022.10.11-000  | 16:52 |   1.8 km | 00:12 |  9.1 kmh |
2022.10.11-000  | 17:23 |   1.4 km | 00:10 |  7.8 kmh |
2022.10.11-000  | 17:41 |   0.1 km | 00:00 |  8.1 kmh |
2022.10.15-001  | 15:48 |   2.0 km | 00:24 |  4.9 kmh |
2022.10.15-001  | 17:02 |   0.5 km | 00:06 |  4.9 kmh |
2022.10.15-001  | 17:09 |   0.1 km | 00:01 |  3.7 kmh |
2022.10.17-004  | 12:25 |   1.9 km | 00:12 |  9.3 kmh |
2022.10.17-004  | 12:55 |   1.3 km | 00:08 |  9.4 kmh |
2022.10.17-004  | 13:11 |   0.1 km | 00:00 |  8.4 kmh |
/tmp/running-segment-2.gnuplot #visits:  3
------------------------------------------------------------
```

### Category statistics

Each tour is on one line with
- date and a running counter (set in cleanup, see above, TODO: remove it)
- start time
- distance
- average speed

```
# category cycling
2022.10.16-002  | 08:49 |  25.7 km | 01:03 | 24.4 kmh |
2022.10.19-004  | 12:52 |  25.7 km | 01:00 | 25.6 kmh |
2022.10.15-001  | 15:49 |  37.7 km | 03:11 | 11.8 kmh |
2022.10.22-005  | 17:26 | 342.2 km | 25:29 | 13.4 kmh |
total cycling   :  431.3 km | 30.7h
```

### Segment statistics

Each tour might enter the segment area and leave it several times, that is why there are multiple lines with the same tour. Each line has_
- date and a running counter (set in cleanup, see above, TODO: remove it)
- enter time
- distance
- time spent inside the segment
- average speed inside the segment

```
-------------------------------------------------------
2022.10.15-001  | 15:49 |   2.1 km | 00:25 |  5.0 kmh |
2022.10.15-001  | 17:01 |   0.6 km | 00:06 |  5.0 kmh |
2022.10.15-001  | 18:28 |   1.4 km | 00:02 | 29.1 kmh |
2022.10.22-005  | 17:26 |   2.1 km | 00:06 | 20.0 kmh |
2022.10.22-005  | 18:49 |   2.1 km | 00:06 | 19.2 kmh |
/tmp/cycling-segment-0.gnuplot #visits:  2
```

### Images
```
$ gnuplot /tmp/cycling-segment-0.gnuplot
```

![image](./cycling-segment-0.png)
