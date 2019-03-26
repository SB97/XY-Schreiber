G00 S1; endstops
G00 E0; no extrusion
G01 S1; endstops
G01 E0; no extrusion
G21; millimeters
G91 G0 F300.0 Z20.000; pen park !!Zsafe
G90; absolute
G28 X; home
G28 Y; home
G28 Z; home
G00 F300.0 Z35.000; pen park !!Zpark
G00 F2400.0 Y0.000; !!Ybottom
G00 F2400.0 X0.000; !!Xleft
G00 F2400.0 X8.315 Y91.848; move !!Xleft+8.315 Ybottom+91.848
G00 F300.0 Z15.000; pen down !!Zwork
G01 F2100.0 X83.155 Y91.848; draw !!Xleft+83.155 Ybottom+91.848
G01 F2100.0 X83.155 Y11.717; draw !!Xleft+83.155 Ybottom+11.717
G01 F2100.0 X8.315 Y11.717; draw !!Xleft+8.315 Ybottom+11.717
G01 F2100.0 X8.315 Y91.848; draw !!Xleft+8.315 Ybottom+91.848
G00 F300.0 Z35.000; pen park !!Zpark
